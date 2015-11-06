#include "driver_config.h"
#include "target_config.h"
#include "m0utils.h"
#include "timer32.h"
#include "gpio.h"
#include "rf22.h"
#include "rfcomm.h"
#include "trigInput.h"
#include "spi.h"
#include "defs.h"
#include "led.h"

//TEMP
#define DEBUG

/* timer thing */
unsigned int MsCount;
/*read MY_ID from HW jumpers*/
unsigned char MY_ID = 0;

/* tx & rx data package*/
payload_t txData;
payload_t rxData;

/* SysTick interrupt happens every 10 ms */
void SysTick_Handler(void)
{
	//GPIOSetValue( LED_PORT, LED_BIT, LED_OFF);

	MsCount += 1;
	//GPIOSetValue( LED_PORT, LED_BIT, LED_ON);
}

/**
 * @brief	read HW adress from 4 input jumpers
 *
 * @return	HW_ID 4 bits
 * */
void readHwId(void)
{
	unsigned int val;
#if 0
	val = LPC_GPIO_PORT->PIN0;

	MY_ID =1;	//ID1 is the lowest possible
	if(val & (1 << ID_BIT0))
		MY_ID += 1;
	if(val & (1 << ID_BIT1))
		MY_ID += 2;
	if(val & (1 << ID_BIT2))
		MY_ID += 4;
	if(!(val & (1 << ID_BIT3)))
		MY_ID += 8;
#endif
}

/**
 * @brief	initialization error, when communicating with RFM22
 * 			signal to the world using the LED
 * */
void initError(void)
{
	/* display SOS -*- on the LED */
	while (1)
	{
		updateLed(LED_ERROR);
	}
}

/*
 * @brief 	Wait for ACK after transmitting, returns status code accordingly
 *
 * @return	ACK_OK 	GOT_BROADCAST	ACK_TIMEOUT
 * */
signed int waitForAck(void)
{
	unsigned char len;
	if (waitAvailableTimeout(ACK_TIMEOUT_MS))
	{
		len = sizeof(rxData);
		if (recv((unsigned char *) &rxData, &len))
		{
			if (rxData.type == ACK_REMOTE)
			{
				if (rxData.dest == MY_ID)
				{
					return ACK_OK;
				}
                else
                {
                    return OTHER_DATA;
                }
			}
			else if ((rxData.type == DATA_REMOTE) || (rxData.type == DATA_ROUTER))
			{
				return OTHER_DATA;
			}
			else if (rxData.type == BROADCAST)
			{
				return GOT_BROACAST;
			}
		}
	}
	return ACK_TIMEOUT;
}

/*
 * @brief 	Feed the watchdog
 *
 * @return	none
 * */
void WDTFeed(void)
{

}

/**
 * @brief 	setup systems & platform
 *
 * */
void sysInit(void)
{

	/* update core clk */
	SystemCoreClockUpdate();

	/* enable watchdog, we do not use window function, only timeout allow reset of the CPU*/
#ifndef DEBUG
	WDT_CLK_Setup();
	WDTInit();
#endif
	/* enale brown out reset when VCC < 2,4V*/
	//BOD_Init();
	/* Called for system library in core_cmx.h(x=0 or 3). */
	SysTick_Config( (SysTick->CALIB/10) + 1);

	/* Enable clock to MRT and reset the MRT peripheral */
	//LPC_SYSCON->SYSAHBCLKCTRL |= (0x1 << 10);
	/* repetetive timer */
	//init_mrt(12000); //1mS interval
	delay(300);

	/* init periphereals*/
	GPIOInit();
	//initKeys(); - NOT used, we use pins instead, for reading HW ID, since they have pull up/dwn
	initLed();
	trigInputInit();

	/* #NSEL is GPIO controlled*/
	GPIOSetDir(SEL_PORT, SEL_PIN, 1);
	GPIOSetValue(SEL_PORT, SEL_PIN, 1);

	/* SDN - shutdown is GPIO controlled*/
	GPIOSetDir(SDN_PORT, SDN_PIN, 1);
	GPIOSetValue(SDN_PORT, SDN_PIN, 0);	//dont shut down

	/* Config pins for SPI port 0*/

	/* Setup SPI port 0*/
	SPI_Init();

	/* init radio ( chip select - D6/p0.6 ; nIRQ - D56/p2.4 ; 1 == SSP1 */
//	RF22init(6, 56, 1);
	if (RF22init(6, 56, 1) == 0)
		initError();
#if 1

	/* 17dBm TX power - max is 20dBm */
	setTxPower(RF22_TXPOW_17DBM);

	/*  un-select RFM22 */
	//GPIOSetDir(SEL_PORT, SEL_PIN, 1);
	/* inputs for reading HW adress */
	GPIOSetDir(ID_PORT0, ID_BIT0, 0);
	GPIOSetDir(ID_PORT1, ID_BIT1, 0);
	GPIOSetDir(ID_PORT2, ID_BIT2, 0);
	GPIOSetDir(ID_PORT3, ID_BIT3, 0);

	/*set pulldowns - solder jumpers connects to 3v3 */
	LPC_IOCON->PIO2_10 &= ~0x18;
	LPC_IOCON->PIO2_10 |= 0x8;
	LPC_IOCON->PIO2_11 &= ~0x18;
	LPC_IOCON->PIO2_11 |= 0x8;
	LPC_IOCON->PIO3_0 &= ~0x18;
	LPC_IOCON->PIO3_0 |= 0x8;
	LPC_IOCON->PIO3_1 &= ~0x18;
	LPC_IOCON->PIO3_1 |= 0x8;
#endif
}

/* Main Program */

int main(void)
{
	volatile int timeouts, triggerAction, lastTrigger; //todo remove volatile
	signed char gotAck = 0;
	unsigned char txRetries = 0;
	volatile unsigned char len = 0;

	LED_STATUS_t state = LED_IDLE;

	/* setup GPIO and RFM22 */
	sysInit();

	/* allow system to settle ?WTF? before reading HW ID*/
	delay(300);

#if 0
	/* blink led to signal HW adress*/
	readHwId();
	if (MY_ID < 16)
	{
		len = MY_ID;
		while (len--)
		{
			ledOn();
			delay(150);
			ledOff();
			delay(300);
			WDTFeed();
		}
		/**additional delay, before led handler takes over
		 * also allows sufficient time to avoid detecting a trig input
		 */
		delay(300);
	}

	//rx all
	setPromiscuous(1);
#endif
	while (1)
	{
		WDTFeed();
		/* and AC trigger input */
		while(1)
		triggerAction = trigInputRead();
		//if ( triggerAction != NO_TRIGGER)
		/* any change since last...?*/
		if (triggerAction != lastTrigger)
		{
			/* update delay line */
			lastTrigger = triggerAction;
			/* so if we are triggered...*/
			if ((triggerAction == TRIGGER1_ACTIVE) || (triggerAction == TRIGGER2_ACTIVE))
			{
				/* compose data pkg */
				txData.data[0] = triggerAction;
				txData.data[1] = MY_ID;
				txData.dest = DEST_ANY_ROUTER;
				txData.source = MY_ID;
				txData.type = DATA_REMOTE;
			}
			/* nothing usefull, then don't send*/
			else
			{
				continue;
			}
			/* send message, if unsucessfull, retry */
			gotAck = 0;
			txRetries = 0;
			do
			{
				WDTFeed();
				/* if we have been here before due to non ACK_OK data*/
				if ((gotAck == OTHER_DATA) || (gotAck == GOT_BROACAST))
				{
					gotAck = waitForAck();
				}
				/* if we are timed out...*/
				else
				{
					len = sizeof(txData);
					send(&txData.data[0], len);
					waitPacketSent();
					gotAck = waitForAck();
				}
			} while ((gotAck != ACK_OK) && (txRetries++ < NO_OF_RETRIIES));

			if (gotAck == ACK_TIMEOUT)
			{
				timeouts++;
				state = LED_ACK_TIMEOUT;
				/* set IDLE to lower consumption*/
				setModeIdle();
			}
			if (gotAck == ACK_OK)
			{
				/* set IDLE to lower consumption*/
				state = LED_ACK_OK;
				setModeIdle();
			}

		}
		/* update LED's*/
		if (updateLed(state) == SEQUENCE_END)
			state = LED_IDLE;
	}
}

#if 0

int main(void)
{
	int i = 0, on = 0;
	/* Basic chip initialization is taken care of in SystemInit() called
	 * from the startup code. SystemInit() and chip settings are defined
	 * in the CMSIS system_<part family>.c file.
	 */

	/* Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS */
	/* You may also want to use the Cortex SysTick timer to do this */
	//1 ms
	//init_timer32(0, TIME_INTERVAL);
	/* Enable timer 0. Our interrupt handler will begin incrementing
	 * the TimeTick global each time timer 0 matches and resets.
	 */
	//enable_timer32(0);
	/* Initialize GPIO (sets up clock) */
	GPIOInit();
	/* Set LED port pin to output */
	GPIOSetDir( LED_PORT, LED_BIT, 1);

	sysInit();
	while (1) /* Loop forever */
	{
		/* Each time we wake up... */
		/* Check TimeTick to see whether to set or clear the LED I/O pin */
		if ((timer32_0_counter % (LED_TOGGLE_TICKS / COUNT_MAX))
				< ((LED_TOGGLE_TICKS / COUNT_MAX) / 2))
		{
			GPIOSetValue( LED_PORT, LED_BIT, LED_OFF);
			on = 0;
		}
		else
		{
			GPIOSetValue( LED_PORT, LED_BIT, LED_ON);
			if (!on)
			{
				i++;
			}
			on = 1;
		}
		/* Go to sleep to save power between timer interrupts */
		__WFI();
	}
}
#endif
