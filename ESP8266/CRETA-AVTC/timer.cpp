/*************************************************/
/*                  INCLUDE                      */
/*************************************************/
#include <ESP8266WiFi.h>
#include "user_interface.h"
#include "timer.h"
#include "param.h"
#include "device.h"
/*************************************************/
/*                  LOCAL  SYMBOL                */
/*************************************************/
/**
 * @brief Software timer
 */
typedef struct
{
    uint32_t interval;
    uint32_t timeout;
    uint8_t  started;
    void     (*callback)(void);
    void*    param;
}SWTimer_t;

/*************************************************/
/*                  EXTERN VARIABLE              */
/*************************************************/
extern led_status_t gLedFlag;
/*************************************************/
/*                  LOCAL  VARIABLE              */
/*************************************************/
os_timer_t gTimer;
SWTimer_t SWTimer[SW_TIMER_MAX_NUM] = {{0}};
/*************************************************/
/*                  FUCTION PROTOTYPE            */
/*************************************************/
void TimerISRHandler (void);
/*************************************************/
/*                  MAIN FUNCTION                */
/*************************************************/
/**
 * @brief: initialize main timer, 50ms interval
 */
void Timer_Init (void)
{
  os_timer_disarm(&gTimer);
  os_timer_setfn(&gTimer, (os_timer_func_t *)TimerISRHandler, NULL);
  os_timer_arm(&gTimer, 50, 1);
}

/**
 * @brief: ISR function of main timer, control software timer 
 */
void TimerISRHandler (void)
{
  /* Control Led */
  static char j = 0;
  j++;
  if (j == 4)
  {
    if (gLedFlag == LED_STATUS_BLINK)
      Led_Toggle();
    else if (gLedFlag == LED_STATUS_ON)
      Led_On();
    else if (gLedFlag == LED_STATUS_OFF)
      Led_Off();
  }

  for (int i = 0; i < SW_TIMER_MAX_NUM; i++)
  {
    if ((SWTimer[i].callback != NULL) && (SWTimer[i].started == 1))
    {
      /* check timeout condition */
      if (millis() > SWTimer[i].timeout)
      {
        /* call callback */
        SWTimer[i].callback();
        /* delete SW timer */
        SWTimer[i].started  = 0;
        SWTimer[i].timeout  = 0;
        SWTimer[i].interval = 0;
        SWTimer[i].callback = NULL;
        SWTimer[i].param    = NULL;
      }
    }
  }
}

int createSWTimer (uint32_t interval, void (*Callback)(void), void* param)
{
  if ((interval == 0) || (NULL == Callback))
    return -1;
  for (int i = 0; i < SW_TIMER_MAX_NUM; i++)
  {
    if (SWTimer[i].callback == NULL)  // check for available SWTimer
    {
      /* setting param of sw timer */
      SWTimer[i].interval = interval;
      SWTimer[i].timeout  = 0;
      SWTimer[i].started  = 0;
      SWTimer[i].callback = Callback;
      SWTimer[i].param    = param;
      return i;
    }
  }
  return -1;
}

int runSWTimer (int id)
{
  if ((id > SW_TIMER_MAX_NUM) || (id < 0))
    return -1;
  if (SWTimer[id].callback != NULL)
  {
    SWTimer[id].timeout = millis();
    SWTimer[id].timeout += SWTimer[id].interval; // Set timeout for timer
    SWTimer[id].started = 1;
  }
  return 0;
}

void haltSWTimer (int id)
{
  SWTimer[id].started = 0;
}

int deleteSWTimer (int id)
{
  if ((id > SW_TIMER_MAX_NUM) || (id < 0))
    return -1;
  if (SWTimer[id].callback != NULL)
  {
    SWTimer[id].callback = NULL;
    SWTimer[id].param    = NULL;
  }
}

