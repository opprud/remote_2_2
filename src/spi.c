/*
 * spi.c
 * simple polled SPI master for 8 bit xfers
 *
 *  Created on: Oct 27, 2015
 *      Author: mortenopprudjakobsen
 *
 *
 */

#include "driver_config.h"
#include "spi.h"


void SPI_Init(void)
{
	/* SPI0_SCK  -> PIO_6*/
	/* SPI0_MOSI -> PIO_9*/
	/* SPI0_MISO -> PIO_8*/
	LPC_SYSCON->PRESETCTRL |= (0x1 << 0);
	LPC_SYSCON->SYSAHBCLKCTRL |= (0x1 << 11);
	LPC_SYSCON->SSP0CLKDIV = 0x02; /* Divided by 2 */
	LPC_IOCON->PIO0_8 &= ~0x07; /*  SSP I/O config */
	LPC_IOCON->PIO0_8 |= 0x01; /* SSP MISO */
	LPC_IOCON->PIO0_9 &= ~0x07;
	LPC_IOCON->PIO0_9 |= 0x01; /* SSP MOSI */
	LPC_IOCON->SCK_LOC = 0x02;
	LPC_IOCON->PIO0_6 = 0x02; /* P0.6 function 2 is SSP clock, need to

	 /* Set DSS data to 8-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 15 */
	LPC_SSP0->CR0 = 0x0707;

	/* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
	LPC_SSP0->CPSR = 0x2;
	/* Master mode */
	LPC_SSP0->CR1 = SSPCR1_SSE;

}

//void SPI_Send( LPC_SPI0, SLAVE0, tx, 2 );
void SPI_Send(uint8_t *buf, uint32_t Length)
{
	uint32_t i;
	uint8_t Dummy = Dummy;

	for (i = 0; i < Length; i++)
	{
		/* Move on only if NOT busy and TX FIFO not full. */
		while ((LPC_SSP0->SR & (SSPSR_TNF | SSPSR_BSY)) != SSPSR_TNF);
		LPC_SSP0->DR = *buf;
		buf++;
		while ((LPC_SSP0->SR & (SSPSR_BSY | SSPSR_RNE)) != SSPSR_RNE);
		/* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO
		 on MISO. Otherwise, when SSP0Receive() is called, previous data byte
		 is left in the FIFO. */
		Dummy = LPC_SSP0->DR;
	}
	return;

}

void SPI_Receive(uint8_t *buf, uint32_t Length)
{
	uint32_t i;

	for (i = 0; i < Length; i++)
	{
		/* As long as Receive FIFO is not empty, I can always receive. */
		/* If it's a loopback test, clock is shared for both TX and RX,
		 no need to write dummy byte to get clock to get the data */
		/* if it's a peer-to-peer communication, SSPDR needs to be written
		 before a read can take place. */
		LPC_SSP0->DR = 0xFF;
		/* Wait until the Busy bit is cleared */
		while ((LPC_SSP0->SR & (SSPSR_BSY | SSPSR_RNE)) != SSPSR_RNE)
			;
		*buf = LPC_SSP0->DR;
		buf++;
	}
	return;
}

