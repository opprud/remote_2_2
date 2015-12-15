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
#include "RHReliableDatagram.h"

volatile unsigned int MsCount;

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

/* tx & rx data package*/
payload_t txData;
payload_t rxData;

/*read MY_ID from HW jumpers*/
unsigned char MY_ID = 0;

/* SysTick interrupt happens every 10 ms */
void SysTick_Handler(void)
{
	MsCount += 1;
}

/**
 * @brief	read HW adress from 4 input jumpers
 *
 * @return	HW_ID 4 bits
 * */
void readHwId(void)
{
	unsigned int val;

	MY_ID = 1;	//ID1 is the lowest possible

	/* ID pins are hardcoded for now, missing a gpio read func...*/
	if ((LPC_GPIO2->DATA) & (1 << ID_BIT0))
		MY_ID += 1;
	if ((LPC_GPIO2->DATA) & (1 << ID_BIT1))
		MY_ID += 2;
	if ((LPC_GPIO3->DATA) & (1 << ID_BIT2))
		MY_ID += 4;
	if ((LPC_GPIO3->DATA) & (1 << ID_BIT3))
		MY_ID += 8;

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
/**
 * @brief 	setup systems & platform
 *
 * */
void sysInit(void)
{

	//__disable_irq();
	/* update core clk */
	SystemCoreClockUpdate();

	/* enale brown out reset when VCC < 2,4V*/
	//BOD_Init();
	/* Called for system library in core_cmx.h(x=0 or 3). */
	SysTick_Config((SysTick->CALIB / 10) + 1);
	__enable_irq();

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
	if (RF22init(6, 56, 1) == 0)
		initError();

	/* 17dBm TX power - max is 20dBm */
	setTxPower(RF22_TXPOW_17DBM);

#if 1
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

	//__enable_irq();
}

/* Main Program */

int main(void)
{

	volatile int triggerAction, lastTrigger; //todo remove volatile

	uint8_t to;
	uint8_t from;
	uint8_t id;
	uint8_t flags;

	/* 0 = ignore */
	from = to = id = flags = 0;

	LED_STATUS_t state = LED_IDLE;

	/* setup GPIO and RFM22 */
	sysInit();

	/* allow system to settle ?WTF? before reading HW ID*/
	delay(300);

	/*read HW ID fro solder jumpers */
	readHwId();

	setThisAddress(MY_ID);
	initRHReliableDatagram(MY_ID);

	//setPromiscuous(1);//catch all

	while (1)
	{

		/* and AC trigger input */
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
				/* we have a keypress */
				txData.type = REMOTE_KEY_PRESSED;
				/* send key1 or key2 */
				txData.data[0] = triggerAction;

				/* Send a message to manager_server */
				if (sendtoWait((uint8_t*) &txData, sizeof(txData), SERVER_ADDRESS))
				{
					// Now wait for a reply from the server
					uint8_t len = sizeof(rxData);
					if (recvfromAckTimeout((uint8_t*) &rxData, &len, 2000, &from, &to, &id, &flags))
					{
						if ((rxData.type == REMOTE_ACK) && (from))
						{
							/* signal ack on led */
							state = LED_ACK_OK;
							/* set IDLE to lower consumption*/
							setModeIdle();
						}
					}
				}
				/* ack failed */
				else
				{
					/* NO-ONE is ACK'ing, shout out as broadcast, and expect no ack'ed telegram..*/
					sendtoWait((uint8_t*) &txData, sizeof(txData), RH_BROADCAST_ADDRESS);

					/* timeout set IDLE to lower consumption */
					setModeIdle();

					/* signal timeout */
					state = LED_ACK_TIMEOUT;
					//printf("No reply, is rf22_reliable_datagram_server running?");
				}
			}
		}
		/* update LED's */
		if (updateLed(state) == SEQUENCE_END)
			state = LED_IDLE;

	}
}

