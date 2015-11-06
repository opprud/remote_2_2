/*
 * trigInput.h
 *
 *  Created on: Jan 18, 2015
 *      Author: mortenjakobsen
 */

#ifndef HW_BRINGUP_SRC_TRIGINPUT_H_
#define HW_BRINGUP_SRC_TRIGINPUT_H_


#define NO_TRIGGER		0
#define TRIGGER1_ACTIVE	1
#define TRIGGER2_ACTIVE	2

void trigInputInit(void);
int trigInputRead(void);

#endif /* HW_BRINGUP_SRC_TRIGINPUT_H_ */
