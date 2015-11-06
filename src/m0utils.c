/*
 * m0utils.c
 *
 *  Created on: Jun 3, 2014
 *      Author: MortenOJ
 */


extern unsigned int MsCount;

// ms elapsed
unsigned long millis()
{
    return (unsigned long) MsCount;	//TODO !!
}

long randomn(long to)
{
    return millis() % to;
}

long random(long from, long to)
{
    return from + (randomn(to) % (to - from));
}

void delay(int ms)
{
	volatile unsigned long start, mSec;

	start = millis();
	mSec = start;

	while((mSec - start) < ms )
	{
		mSec = millis();
	}

}
