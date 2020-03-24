#include "ledApp.h"

osThreadId ledThread;

void LedTask(void const * argument)
{
  for(;;) {
    led0.ON();
    osDelay(500);
    led0.OFF();
    osDelay(500);
  }
}

void LedAppInit()
{
  led0.begin();
  led1.begin();
  led1.OFF();
  osThreadDef(LedTask, LedTask, osPriorityLow, 0, 128);
  ledThread = osThreadCreate(osThread(LedTask), NULL);
  taskENABLE_INTERRUPTS();
}
