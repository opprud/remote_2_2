/*
 * rfcomm.h
 *
 *  Created on: Jan 3, 2015
 *      Author: mortenjakobsen
 */

#ifndef HW_BRINGUP_SRC_RFCOMM_H_
#define HW_BRINGUP_SRC_RFCOMM_H_

// function return values
#define BEEN_HERE_BEFORE 	-1
#define TOO_MANY_HOPS		-2

#define ROUTER_MAX_HOPS		6

// data values
#define KEY_SHORT_PRESS		1
//#define KEY_RELEASED		2
#define KEY_LONG_PRESS		3
#define KEY_TRIGGER			4
#define LOW_BATT			5


/*telegram type*/
typedef enum
{
	//TLG_NONE=0 ,BROADCAST, KEYDATA_ROUTER, ACK_REMOTE, DATA_REMOTE, DATA_ROUTER, DATA_ROUTER_FOR_ME
	TLG_NONE=0 ,
	BROADCAST,
	REMOTE_KEY_PRESSED,
	REMOTE_KEY_RELEASED,
	REMOTE_ACK,
	TOWER_KEY_PRESSED,
	TOWER_KEY_RELEASED,
	TOWER_ACK,
} tlg_type_t;



/* data payload
 * no CRC is added, since this allready exists in transmitter HW
 * */
typedef struct
{
	unsigned char data[4];
	unsigned char isForward;
	unsigned char destination;
	unsigned char source;
	unsigned char retries;
	unsigned char hops;	//used to limit unlimited forwards
	tlg_type_t type;
} payload_t;

#endif /* HW_BRINGUP_SRC_RFCOMM_H_ */
