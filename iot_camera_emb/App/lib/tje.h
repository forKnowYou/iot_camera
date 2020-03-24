#ifndef TINY_JPEG_ENCODER
#define TINY_JPEG_ENCODER

#ifdef __cplusplus
extern "C" {
#endif

#define TJE_IMPLEMENTATION

#ifndef TJE_HEADER_GUARD
#define TJE_HEADER_GUARD

// - tje_encode_to_file -
//
// Usage:
//  Takes bitmap data and writes a JPEG-encoded image to disk.
//
//  PARAMETERS
//      dest_path:          filename to which we will write. e.g. "out.jpg"
//      width, height:      image size in pixels
//      num_components:     3 is RGB. 4 is RGBA. Those are the only supported values
//      src_data:           pointer to the pixel data.
//
//  RETURN:
//      0 on error. 1 on success.

int tje_encode_to_file(const char* dest_path,
                       const int width,
                       const int height,
                       const int num_components,
                       const unsigned char* src_data);

// - tje_encode_to_file_at_quality -
//
// Usage:
//  Takes bitmap data and writes a JPEG-encoded image to disk.
//
//  PARAMETERS
//      dest_path:          filename to which we will write. e.g. "out.jpg"
//      quality:            3: Highest. Compression varies wildly (between 1/3 and 1/20).
//                          2: Very good quality. About 1/2 the size of 3.
//                          1: Noticeable. About 1/6 the size of 3, or 1/3 the size of 2.
//      width, height:      image size in pixels
//      num_components:     3 is RGB. 4 is RGBA. Those are the only supported values
//      src_data:           pointer to the pixel data.
//
//  RETURN:
//      0 on error. 1 on success.

int tje_encode_to_file_at_quality(const char* dest_path,
                                  const int quality,
                                  const int width,
                                  const int height,
                                  const int num_components,
                                  const unsigned char* src_data);

// - tje_encode_with_func -
//
// Usage
//  Same as tje_encode_to_file_at_quality, but it takes a callback that knows
//  how to handle (or ignore) `context`. The callback receives an array `data`
//  of `size` bytes, which can be written directly to a file. There is no need
//  to free the data.

typedef void tje_write_func(void* context, void* data, int size);
typedef void tje_get_next_line_fun(unsigned char *src_data, int size);

int tje_encode_with_func(tje_write_func* func,
                         tje_get_next_line_fun *fun_get_next_line,
                         void* context,
                         const int quality,
                         const int width,
                         const int height,
                         const int num_components,
                         unsigned char *src_data);

#endif // TJE_HEADER_GUARD

#ifdef __cplusplus
}
#endif

#endif
