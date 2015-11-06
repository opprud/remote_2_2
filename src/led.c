/*
 * led.c
 *
 *  Created on: Jan 18, 2015
 *      Author: mortenjakobsen
 */

/**
 * Init LED blink module
 *
 * @brief  Clear state and timer, sets up port
 */
#include "led.h"
//#include "lpc8xx_gpio.h"
//#include "LPC8xx.h"
#include "gpio.h"
#include "defs.h"
#include "m0utils.h"

/* array holding led status codes */
/* --------------------------------- LED flash times---------------------------------------------- / timer/ repeat */
static led_status_table_t ledStatus[5] =
{
{
{
{ OFF, 2000 },
{ ON, 100 },
{ OFF, 2000 },
{ ON, 100 },
{ OFF, 2000 },
{ ON, 100 },
{ OFF, 2000 },
{ ON, 100 },
{ OFF, 2000 },
{ ON, 100 }, }, 0, TRUE },
{
{
{ ON, 200 },
{ ON, 200 },
{ ON, 200 },
{ ON, 200 },
{ ON, 200 },
{ ON, 200 },
{ ON, 200 },
{ ON, 200 },
{ ON, 200 },
{ ON, 200 }, }, 0, TRUE },
{
{
{ OFF, 400 },
{ ON, 600 },
{ OFF, 400 },
{ ON, 600 },
{ OFF, 400 },
{ ON, 600 },
{ OFF, 400 },
{ OFF, 400 },
{ OFF, 400 },
{ OFF, 400 }, }, 0, FALSE },
{
{
{ OFF, 120 },
{ ON, 120 },
{ OFF, 120 },
{ ON, 120 },
{ OFF, 120 },
{ ON, 120 },
{ OFF, 120 },
{ ON, 120 },
{ OFF, 120 },
{ ON, 120 }, }, 0, TRUE },
{
{
{ OFF, 200 },
{ ON, 700 },
{ OFF, 200 },
{ ON, 200 },
{ OFF, 200 },
{ ON, 700 },
{ OFF, 400 },
{ OFF, 400 },
{ OFF, 400 },
{ OFF, 400 }, }, 0, TRUE } };

void ledOn(void)
{
	GPIOSetValue(LED_PORT, LED_PIN, 1);
}

void ledOff(void)
{
	GPIOSetValue(LED_PORT, LED_PIN, 0);
}

void initLed()
{
	unsigned long timeNow;

	/* update time */
	timeNow = millis();

	/* Set LED port p1.11 to output */
	GPIOSetDir(LED_PORT, LED_PIN, 1);
	/* LED off */
	GPIOSetValue(LED_PORT, LED_PIN, 0);

	/* set update times accordingly */
	ledStatus[0].index = 0;
	ledStatus[1].index = 0;
	ledStatus[2].index = 0;
	ledStatus[3].index = 0;

	/* somehow fails during init - TODO*/
	ledStatus[0].repeatPattern = 1;
	ledStatus[1].repeatPattern = 1;
	ledStatus[2].repeatPattern = 0;
	ledStatus[3].repeatPattern = 1;
	ledStatus[4].repeatPattern = 1;

	ledStatus[0].nextLedUpdateTime = timeNow + ledStatus[0].timing[ledStatus[0].index].nextLedUpdateTime;
	ledStatus[1].nextLedUpdateTime = timeNow + ledStatus[1].timing[ledStatus[1].index].nextLedUpdateTime;
	ledStatus[2].nextLedUpdateTime = timeNow + ledStatus[2].timing[ledStatus[2].index].nextLedUpdateTime;
	ledStatus[3].nextLedUpdateTime = timeNow + ledStatus[3].timing[ledStatus[3].index].nextLedUpdateTime;
	ledStatus[4].nextLedUpdateTime = timeNow + ledStatus[4].timing[ledStatus[4].index].nextLedUpdateTime;

}

/**
 * updateLed
 * handles led blink from timer and status code
 * iterates through the 10 timing steps, based on the current state
 * @param	status - LED_STATUS_t - status code
 * @brief  	update LED based upon state
 */
int updateLed(LED_STATUS_t status)
{
	static LED_STATUS_t lastState;
	unsigned long timeNow;

	/* update time */
	timeNow = millis();

	/* if LED state has changed, zero indexes*/
	if (status != lastState)
	{
		lastState = status;
		ledStatus[status].index = 0;
		ledStatus[status].nextLedUpdateTime = ledStatus[status].timing[0].nextLedUpdateTime;
	}

	/* if state is the same, just update LED's */
	/* time to update output ? */
	if (timeNow >= ledStatus[status].nextLedUpdateTime)
	{
		/*last pattern ?*/
		if (ledStatus[status].index == 10)
		{
			/* continue pattern ?*/
			if (ledStatus[status].repeatPattern == TRUE)
			{
				ledStatus[status].index = 0;
			}
			else
			{
				return SEQUENCE_END;
			}
		}

		/* LED on / off*/
		GPIOSetValue(LED_PORT, LED_PIN, ledStatus[status].timing[ledStatus[status].index].isOn);

		/* set next update time for the selected state, and increment index */
		ledStatus[status].nextLedUpdateTime = timeNow
				+ ledStatus[status].timing[ledStatus[status].index++].nextLedUpdateTime;

	}
	return SEQUENCE_LOOPS;

}
