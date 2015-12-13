/*
 * RHGeneric.h
 *
 *  Created on: Nov 27, 2015
 *      Author: mortenjakobsen
 */

#ifndef RHGENERIC_H_
#define RHGENERIC_H_

#include "defs.h"

// Defines bits of the FLAGS header reserved for use by the RadioHead library and
// the flags available for use by applications
#define RH_FLAGS_RESERVED                 0xf0
#define RH_FLAGS_APPLICATION_SPECIFIC     0x0f
#define RH_FLAGS_NONE                     0
// This is the address that indicates a broadcast
#define RH_BROADCAST_ADDRESS 0xff
#define YIELD


/// The current transport operating mode
//volatile RHMode _mode;

/// This node id
uint8_t _thisAddress;

/// Whether the transport is in promiscuous mode
bool _promiscuous;

/// TO header in the last received mesasge
volatile uint8_t _rxHeaderTo;

/// FROM header in the last received mesasge
volatile uint8_t _rxHeaderFrom;

/// ID header in the last received mesasge
volatile uint8_t _rxHeaderId;

/// FLAGS header in the last received mesasge
volatile uint8_t _rxHeaderFlags;

/// TO header to send in all messages
uint8_t _txHeaderTo;

/// FROM header to send in all messages
uint8_t _txHeaderFrom;

/// ID header to send in all messages
uint8_t _txHeaderId;

/// FLAGS header to send in all messages
uint8_t _txHeaderFlags;

/// The value of the last received RSSI value, in some transport specific units
volatile int8_t _lastRssi;

/// Count of the number of bad messages (eg bad checksum etc) received
volatile uint16_t _rxBad;

/// Count of the number of successfully transmitted messaged
volatile uint16_t _rxGood;

/// Count of the number of bad messages (correct checksum etc) received
volatile uint16_t _txGood;

#endif /* RHGENERIC_H_ */
