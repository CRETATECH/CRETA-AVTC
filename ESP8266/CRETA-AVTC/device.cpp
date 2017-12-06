/*************************************************/
/*                  INCLUDE                      */
/*************************************************/
#include <ESP8266WiFi.h>
#include "device.h"
#include "param.h"

/*************************************************/
/*                  LOCAL  VARIABLE              */
/*************************************************/
bool isButtonPressed = false;

/*************************************************/
/*             FUNCTION PROTOTYPE                */
/*************************************************/

/*************************************************/
/*                  MAIN FUNCTION                */
/*************************************************/
void Button_Init (void)
{
  pinMode(BUTTON_CONFIG_PIN, INPUT_PULLUP);
}

void Led_Init (void)
{
  pinMode(LED_WIFI_PIN, OUTPUT);
}

void Led_On (void)
{
  digitalWrite(LED_WIFI_PIN, LOW);
}

void Led_Off (void)
{
  digitalWrite(LED_WIFI_PIN, HIGH);
}

void Led_Toggle (void)
{
  int temp = digitalRead(LED_WIFI_PIN);
  digitalWrite(LED_WIFI_PIN, ~temp);
}

/**
 * @brief       Button config check
 * @param       None
 * @retval      true
 *              false
 */
bool buttonConfigCheck (void)
{
    static uint8_t buttonLastStatus = HIGH;
    static uint32_t buttonLastPressed = 0;
    uint8_t buttonStatus = digitalRead(BUTTON_CONFIG_PIN);
    if(buttonStatus != buttonLastStatus){
        if(buttonStatus == LOW){
            buttonLastPressed = millis();
        }
    }
    else{
        if(buttonStatus == LOW){
            if((millis() - buttonLastPressed) > 3000){
                return true;
            }
        }
    }
    buttonLastStatus = buttonStatus;
    return false;  
}

