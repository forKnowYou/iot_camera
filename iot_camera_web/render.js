let http = require('http')
let fs = require('fs')
let dgram = require('dgram')

let myTools = require('./myTools').myTools

let httpServer = http.createServer()
let udpServer = dgram.createSocket('udp4')
udpServer.bind(8100)

let web = fs.readFileSync('web.html').toString()
let img0 = fs.readFileSync('i0.jpg')
let img1 = fs.readFileSync('i1.jpg')
let img2 = fs.readFileSync('i2.jpg')

let toggleImg = false

let irBuf = []
let lastImg = undefined

for(let i = 0; i < 3; i ++) {
  irBuf.push({
    packs: [],
    shouldEnd: false,
    packsTotal: 0
  })
}

httpServer.listen(8000, () => {
  console.log(`httpServer listening`)
})

httpServer.on('request', (req, rep) => {
  console.log(`httpServer get request(${req.method}): `)
  console.log(req.url)
  if(req.url == '/') {
    rep.writeHeader(200, {'Content-Type' : 'text/html ; charset=utf-8'})
    rep.write(web)
  } else if(req.url.indexOf('i0.jpg')) {
    img0 = toggleImg ? img1 : img2
    toggleImg = toggleImg ? false : true
    rep.writeHeader(200, {'Content-Type' : 'image/jpg'})
    rep.write(lastImg ? lastImg : img0)
    console.log('write img:' + (img0 === img1 ? 'img1' : 'img2'))
  }
  rep.end()
})

udpServer.on('listening', () => {
  console.log('udpServer listening')
})

let lastJpegId = 0
let lastLastJpegId = 0
let saveFirstJpeg = false

udpServer.on('message', (msg, rinfo) => {
  console.log(`udpServer get message(${msg.length})`)
  let s = msg.toString()
  console.log(`head of 15 bytes: ${s.slice(0, 16)}`)
  if(s.indexOf('JPEG:') >= 0) {
    let jpegId = parseInt(s.slice(5, 7))
    let jpegCount = s.slice(8, 11)
    let jpegTotal = parseInt(s.slice(12, 15))
    let currentBufId = jpegId % 3
    if(lastJpegId !== jpegId) {
      lastLastJpegId = lastJpegId;
      lastJpegId = jpegId;
      irBuf[currentBufId].packs = []
      irBuf[currentBufId].shouldEnd = false
      irBuf[currentBufId].packsTotal = 0
    }
    irBuf[currentBufId].packs.push(msg)
    console.log(`receive jpegId: ${jpegId}, jpegCount: ${jpegCount}, jpegTotal: ${jpegTotal}, currentBufLength: ${irBuf[currentBufId].packs.length}`)
    if(jpegCount === 'END') {
      irBuf[currentBufId].shouldEnd = true
      irBuf[currentBufId].packsTotal = jpegTotal
    }
    if(irBuf[currentBufId].shouldEnd === true) {
      if(irBuf[currentBufId].packs.length === irBuf[currentBufId].packsTotal) {
        myTools.consoleWithColor(`one img receive compelete, packs count: ${irBuf[currentBufId].packsTotal}`, 'yellow')
        irBuf[currentBufId].packs.sort((a, b) => {
          let aCount = a.slice(8, 11).toString()
          let bCount = b.slice(8, 11).toString()
          if(aCount === 'END')
            return 1
          if(bCount === 'END')
            return -1
          return parseInt(aCount) - parseInt(bCount)
        })
        let content = Buffer.from('')
        irBuf[currentBufId].packs.map((item) => {
          content = Buffer.concat([content, item.slice(16)])
          // if(! saveFirstJpeg) {
          //   let imgBuffer = ''
          //   console.log(``)
          //   console.log(`current buffer len: ${item.length}`)
          //   item.map((item2) => {
          //     if(item2 < 16)
          //       imgBuffer += '0'
          //     imgBuffer += item2.toString(16) + ' '
          //   })
          //   console.log(imgBuffer)
          // }
        })
        myTools.consoleWithColor(`img file length: ${content.length}`, 'yellow')
        if(! saveFirstJpeg) {
          fs.writeFileSync('i9.jpg', content)
        }
        saveFirstJpeg = true
        lastImg = content
        irBuf[currentBufId].packs = []
      }
    }
  }
})
