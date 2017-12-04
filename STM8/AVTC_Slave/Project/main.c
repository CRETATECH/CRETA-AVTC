#include "button.h"

uint8_t debug = 0;
bool debug1 = false;
uint32_t freq = 0;

void main( void )
{
    CLK_HSICmd(ENABLE);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    CLK_AdjustHSICalibrationValue(CLK_HSITRIMVALUE_0);
    freq = CLK_GetClockFreq();

    tickerInit();
    buttonInit(WATER_SENSOR);
    gpioPinMode(GPIO_PORTD, GPIO_PIN_4, GPIO_OUTPUT);
    enableInterrupts();
    while(1) {
        debug = buttonReadLevel(WATER_SENSOR);
        if(debug == GPIO_HIGH) {
            gpioWritePin(GPIO_PORTD, GPIO_PIN_4, GPIO_LOW);
        }
        else {
            gpioWritePin(GPIO_PORTD, GPIO_PIN_4, GPIO_HIGH);
        }
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
  /*
  while (1)
  {
  }
  */
}
#endif