/*
 * led.h
 *
 *  Created on: Jan 18, 2015
 *      Author: mortenjakobsen
 */

#ifndef LED_H_
#define LED_H_

#include "defs.h"

#define ON	TRUE
#define OFF	FALSE

#define LED_PORT	1
#define LED_PIN		11

#define SEQUENCE_LOOPS	1
#define SEQUENCE_END	2

typedef struct
{
	unsigned char 	isOn;
	unsigned long	nextLedUpdateTime;
}led_status_t;

typedef struct
{
	const led_status_t	timing[10];
	unsigned long	nextLedUpdateTime;
	unsigned char	index;
	unsigned char	repeatPattern;
}led_status_table_t;




typedef enum
{
	LED_IDLE = 0,
	LED_TRIG = 1,
	LED_ACK_OK = 2,
	LED_ACK_TIMEOUT = 3,
	LED_ERROR = 4
}LED_STATUS_t;

void ledOn(void);
void ledOff(void);
void initLed();
int updateLed(LED_STATUS_t status);

#endif /* LED_H_ */
