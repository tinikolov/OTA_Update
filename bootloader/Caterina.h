/*
             LUFA Library
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for BootloaderCDC.c.
 */

#ifndef _CDC_H_
#define _CDC_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/boot.h>
		#include <avr/eeprom.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <avr/pgmspace.h>

		#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
		#define LED_SETUP()		DDRC |= (1<<7); DDRB |= (1<<0); DDRD |= (1<<5); DDRC |= (1<<6); DDRD |= (1<<7);
		#define L_LED_OFF()		PORTC &= ~(1<<7)
		#define L_LED_ON()		PORTC |= (1<<7)
		#define L_LED_TOGGLE()	PORTC ^= (1<<7)
		#if DEVICE_PID == 0x0037	// polarity of the RX and TX LEDs is reversed on the Micro
			#define TX_LED_OFF()	PORTD &= ~(1<<5)
			#define TX_LED_ON()		PORTD |= (1<<5)
			#define RX_LED_OFF()	PORTB &= ~(1<<0)
			#define RX_LED_ON()		PORTB |= (1<<0)			
		#else 
			#define TX_LED_OFF()	PORTD |= (1<<5)
			#define TX_LED_ON()		PORTD &= ~(1<<5)
			#define RX_LED_OFF()	PORTB |= (1<<0)
			#define RX_LED_ON()		PORTB &= ~(1<<0)
		#endif

  
#define FIRMWARE_LINE_SIZE 32
#define CRC_CHECK_SIZE     4
#define FILE_LENGTH        2

typedef union{
  uint16_t length16;
  uint8_t  length8[2];
}firmware_data_size;

firmware_data_size firmware_length;

uint16_t imagesize;

char buffer[FIRMWARE_LINE_SIZE];
uint16_t current_byte;
uint16_t nextAddress;
uint8_t read_data;

uint32_t strAddr;

#endif

