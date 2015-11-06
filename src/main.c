#include "driver_config.h"
#include "target_config.h"
#include "timer32.h"
#include "gpio.h"
#include "rf22.h"
#include "spi.h"
#include "defs.h"

//TEMP
unsigned int MsCount;

/* SysTick interrupt happens every 10 ms */
void SysTick_Handler(void)
{
	//GPIOSetValue( LED_PORT, LED_BIT, LED_OFF);

	MsCount +=10;
	//GPIOSetValue( LED_PORT, LED_BIT, LED_ON);

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
	SysTick_Config( SysTick->CALIB + 1);

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
#if 0
	RF22init(6, 56, 1);
//	if (RF22init(6, 56, 1) == 0)
//		initError();

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
