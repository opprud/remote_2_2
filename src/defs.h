/*
 * defs.h
 *
 *  Created on: Jun 6, 2014
 *      Author: MortenOJ
 */

#ifndef HW_BRINGUP_SRC_DEFS_H_
#define HW_BRINGUP_SRC_DEFS_H_

#define TRUE	1
#define true	1
#define FALSE	0
#define false	0
#define NULL	0

typedef unsigned char bool;

/** Misc defines */
#define GOT_BROACAST	1
#define ACK_OK			2
#define OTHER_DATA		3
#define ACK_TIMEOUT		-1

#define ACK_TIMEOUT_MS		1000
#define NO_OF_RETRIIES		3//5

/** Hardware / board defines */
#define RFMIRQ_PORT	0
#define RFMIRQ_PIN	11
#define SDN_PORT	0
#define SDN_PIN		3
#define SEL_PORT	2
#define SEL_PIN		9

#define ID_PORT0	2
#define ID_BIT0		10
#define ID_PORT1	2
#define ID_BIT1		11
#define ID_PORT2	3
#define ID_BIT2		0
#define ID_PORT3	3
#define ID_BIT3		1

#endif /* HW_BRINGUP_SRC_DEFS_H_ */
