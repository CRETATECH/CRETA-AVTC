/*******************************************************************************
  * @filename   : ticker.c
  * @author     : HgN
  * @last update: December 4th, 2017
  */
/******************************************************************************/

/******************************************************************************/
/* INCLUDE */
/******************************************************************************/
#include "ticker.h"
#include <stdlib.h>
#include <stdbool.h>

/******************************************************************************/
/* LOCAL TYPEDEFS */
/******************************************************************************/

/******************************************************************************/
/* LOCAL DEFINES */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC VARIABLES */
/******************************************************************************/
volatile uint32_t _millis = 0;
/******************************************************************************/
/* LOCAL FUNCTION PROTOTYPES */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTIONS */
/******************************************************************************/
/**
 * @brief       Init ticker
 * @param       None
 * @retval      EXIT_SUCCESS
 *              EXIT_FAILURE
 */
int tickerInit(void) {
    timer2Init(62);
    timer2IntEnable();
    return EXIT_SUCCESS;
}

/**
 * @brief       Ticker millis function
 * @param       None
 * @retval      Miliseconds from tickerInit()
 */
uint32_t tickerMillis(void) {
    return _millis;
}

/**
 * @brief       Delay in us
 * @param       None
 * @retval      None
 */
void tickerDelayMs(uint32_t pMsDelay) {
    uint32_t vStart = tickerMillis();
    while((tickerMillis() - vStart) < pMsDelay);
}

/**
 * @brief       Ticker interrupt handler
 * @param       None
 * @retval      None
 */
#pragma vector = 15
__interrupt void tickerIntHandler(void) {
    timer2IntFlagClear();
    _millis++;
}