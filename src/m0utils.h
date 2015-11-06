/*
 * m0utils.h
 *
 *  Created on: Jun 3, 2014
 *      Author: MortenOJ
 */

#ifndef M0UTILS_H_
#define M0UTILS_H_


#include <math.h>
//#include "lpc8xx_mrt.h"

#define	HIGH	1
#define	LOW	0

unsigned long millis();
long random(long from, long to);
//long random(long to);
void delay(int ms);


#endif /* M0UTILS_H_ */
