/*
 * trigInput.h
 *
 *  Created on: Jan 18, 2015
 *      Author: mortenjakobsen
 */

#ifndef TRIGINPUT_H_
#define TRIGINPUT_H_


#define NO_TRIGGER		0
#define TRIGGER1_ACTIVE	1
#define TRIGGER2_ACTIVE	2

void trigInputInit(void);
int trigInputRead(void);

#endif /* TRIGINPUT_H_ */
