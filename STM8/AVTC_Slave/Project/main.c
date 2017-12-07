#include "task.h"
#include "ds18b20.h"

float t;

void main( void )
{
    //taskInit();
    gpioPinMode(DS18B20_PORT, DS18B20_PIN, GPIO_MODE_OUT_OD_LOW_FAST);
    ds18b20Init();
    while(1) {
        t = ds18b20ReadTemp();
        //taskDev2Reg();
        //taskSerialCmd();
    }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif