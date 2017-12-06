/*************************************************/
/*                  INCLUDE                      */
/*************************************************/
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "user_interface.h"
#include "state.h"
#include "device.h"
#include "param.h"
#include "protocol.h"
#include "mqtt.h"
#include "UART.h"

/*************************************************/
/*                  LOCAL  SYMBOL                */
/*************************************************/
typedef enum {
  STATE_CONFIG  = 0,
  STATE_CONTROL = ~STATE_CONFIG,
}fsm_t;

/*************************************************/
/*                  EXTERN VARIABLE              */
/*************************************************/
led_status_t gLedFlag;
/*************************************************/
/*                  LOCAL  VARIABLE              */
/*************************************************/
fsm_t gState = STATE_CONTROL;
extern led_status_t gLedFlag;
uint32_t _time = 0;
/*************************************************/
/*                  FUCTION PROTOTYPE            */
/*************************************************/
void Wifi_Connect (void);
void stateUpdate(void);
void stateConfig(void);
void stateControl(void);
/*************************************************/
/*                  MAIN FUNCTION                */
/*************************************************/
/**
 * @brief       main finite state machine
 * @param       none
 * @retval      none
 */
void stateMachine(void)
{
    switch(gState)
    {
        case STATE_CONFIG:
            stateConfig();
        break;
        case STATE_CONTROL:
            stateControl();
        break;
        default:
        break;
    }
}

/**
 * @brief       update button control status
 * @param       none
 * @retval      none
 */
void stateUpdate(void)
{
    /* gState update (if, elseif,...) */
    /* check config button is pressed or not in control state */
    if (true == buttonConfigCheck())
      gState = STATE_CONFIG;
}

/**
 * @brief       use smart config to config wifi
 * @param       none
 * @retval      none
 */
void stateConfig(void)
{  
  /* stop to sure can start again */
  /* in some situation, config with wrong pass can make smartConfig work wrong */
  WiFi.stopSmartConfig();
  gLedFlag = LED_STATUS_OFF;
  _time = millis();
  WiFi.mode(WIFI_STA);
  if(digitalRead(BUTTON_CONFIG_PIN) != LOW)
  {
  #ifdef DEBUG
      Serial.println("\r\nPROCESS: Start config...");
  #endif
    WiFi.beginSmartConfig();
    while(1)
    {
      delay(1000);
      if (true == buttonConfigCheck())
      {
          gLedFlag = LED_STATUS_ON;
          while(digitalRead(BUTTON_CONFIG_PIN) == LOW){
              delay(1);
          }
          gState = STATE_CONTROL;
          WiFi.stopSmartConfig();
          return;
      }
      if (WiFi.smartConfigDone())
      {
      #ifdef DEBUG
        Serial.println("SUCCESS: Config done!!!\r\n");
      #endif
        break;
      }
    }
    /* write 0x05 to address 0x10 to inform that configed */
    gState = STATE_CONTROL;
  }
}

/**
 * @brief       check connect to router, server
 * @param       none
 * @retval      none
 */
void stateControl(void)
{
    gLedFlag = LED_STATUS_ON;
    /* Check router connect */
    if (WiFi.status() == WL_CONNECTED)
    {
      /* Check connect server */
      if (!mqttConnected())
      {
        #ifdef DEBUG
          Serial.println("MQTT Server not connect...");
        #endif
        if (mqttConnect())
        {
          mqttSubscribe();
          #ifdef DEBUG
            Serial.println("MQTT Server connected, subscribe to topic");
          #endif
        }
      }
      else
      {
        mqttLoop();
      }
    }
    else Wifi_Connect();
    /* check UART hw buffer */
    UART_Event();
}

/**
 * @brief       connect to the last wifi connected
 * @param       none
 * @retval      none
 */
void Wifi_Connect (void)
{
  gLedFlag = LED_STATUS_BLINK;
  WiFi.begin();
  #ifdef DEBUG
    WiFi.printDiag(Serial);
    Serial.print("\r\nWiFi connecting");
  #endif
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    #ifdef DEBUG
      Serial.print('.');
    #endif
    if (true == buttonConfigCheck())
    {
      gState = STATE_CONFIG;
      return;
    }
  }
  mqttConnect();
  mqttSubscribe();
  #ifdef DEBUG
    Serial.println("\r\nWiFi connected!!!");
  #endif
  mqttPubTest();
  #ifdef DEBUG
    Serial.println("publish test");
  #endif
}


