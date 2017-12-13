#ifndef  PARAM_H
#define  PARAM_H

/*************************************************/
/*              EXTERN SYMBOL                    */
/*************************************************/
typedef enum {
  LED_STATUS_BLINK = 0,
  LED_STATUS_ON ,
  LED_STATUS_OFF,
}led_status_t;

#define  LED_WIFI_PIN           4
#define  BUTTON_CONFIG_PIN      0
#define  DEBUG

/*
 * "001" : parse json
 * "002" : esp busy
 */

#endif /* PARAM_H */
