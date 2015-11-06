/*
 * trigInput.c
 * @brief trigger input, detects AC input, between 10 - 100Hz or DC on optocoupler input
 *
 * @date Jan 18, 2015
 * @author mortenjakobsen
 */

#include "driver_config.h"
#include "trigInput.h"
#include "m0utils.h"
#include "defs.h"
#include "gpio.h"

#define TRIG_IN_PIN_1			10
#define TRIG_IN_PORT_1			1
#define TRIG_IN_PIN_2			2
#define TRIG_IN_PORT_2			2

#define TRIG_ACTIVE_LEVEL	0	//open collector pulled to ground

#define	TRIG_ON_AC		1
#define	TRIG_ON_DC		1

#define TRIG_TIME_MS_MAX	200
#define TRIG_TIME_MS_MIN	5	//5 ms = 100HZ AC = 200IRQ's second

volatile unsigned long timeEdge1, timeLast1;
volatile unsigned long timeEdge2, timeLast2;

/**
 * @brief	interrupt handler, updates timer, when fired
 *
 * */
void PIOINT1_IRQHandler(void)
{
	GPIOSetValue( 1, 11, 0);

	GPIOIntClear( TRIG_IN_PORT_1, TRIG_IN_PIN_1);
	/* detect change time */
	timeLast1 = timeEdge1;
	timeEdge1 = millis();

	GPIOSetValue( 1, 11, 1);
}

/**
 * @brief	interrupt handler, updates timer, when fired
 *
 * */
void PIOINT2_IRQHandler(void)
{
	GPIOIntClear( TRIG_IN_PORT_2, TRIG_IN_PIN_2);
	/* detect change time */
	timeLast2 = timeEdge2;
	timeEdge2 = millis();
}

/**
 * *brief	init trig pin as input, install interrupt handler on event 1
 * IO pins on lpc11xx can be configured for hysteresis ~0.4V, we enable this to avoid
 * multiple interrupts when optocoupler input is fired from a potential noisy and slow input
 * */
void trigInputInit(void)
{
	/* init timers */
	/* avoid triggerning after reset, and allow inputs to settle */
	timeLast1 = 0;
	timeLast2 = 0;
	timeEdge1 = 0;
	timeEdge2 = 0;

	/* allow interrupts on GPIO 1 & 2 */
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);

	/* hard coded pins set pullups and hysteresis TODO*/
	LPC_IOCON->PIO2_2  = 0x00000030;
	LPC_IOCON->PIO1_10 = 0x000000B0;


	GPIOSetDir( TRIG_IN_PORT_1, TRIG_IN_PIN_1, 0);
	GPIOSetDir( TRIG_IN_PORT_2, TRIG_IN_PIN_2, 0);

	/* attach pin to falling edge Interrupt */
	GPIOSetInterrupt(TRIG_IN_PORT_1, TRIG_IN_PIN_1, 0, 0, 0);
	GPIOSetInterrupt(TRIG_IN_PORT_2, TRIG_IN_PIN_2, 0, 0, 0);
	/*enable interrupts*/
	GPIOIntEnable( TRIG_IN_PORT_1, TRIG_IN_PIN_1);
	GPIOIntEnable( TRIG_IN_PORT_2, TRIG_IN_PIN_2);
}

/**
 * @brief	calculate time between sucessive AC trigger pulses, or read a DC signal,
 * depending on configuration, return trigger status if signal is valid
 *
 * @return	TRIGGER_ACTIVE or	NO_TRIGGER
 * */
int trigInputRead(void)
{
	int ret = NO_TRIGGER;

#ifdef TRIG_ON_AC
	/* atomic section, no IRQ's while calculating */
	GPIOIntDisable( TRIG_IN_PORT_1, TRIG_IN_PIN_1);
	GPIOIntDisable( TRIG_IN_PORT_2, TRIG_IN_PIN_2);


	/* any changes  wich is not noise ?*/
	if ((timeEdge1 - timeLast1) > TRIG_TIME_MS_MIN)
		ret =  TRIGGER1_ACTIVE;

	/* any changes  wich is not noise ?*/
	if ((timeEdge2 - timeLast2) > TRIG_TIME_MS_MAX)
		ret |=  TRIGGER2_ACTIVE;

	GPIOIntEnable( TRIG_IN_PORT_1, TRIG_IN_PIN_1);
	GPIOIntEnable( TRIG_IN_PORT_2, TRIG_IN_PIN_2);
	/* atomic section ends here */
#endif

//#ifdef TRIG_ON_DC
#if 0
	/* if trigger is low, we have DC*/
	if (!(LPC_GPIO_PORT->PIN0 & TRIG_IN_PIN))
	{
		return TRIGGER_ACTIVE;
	}
#endif
	return ret;
}

