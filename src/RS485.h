/*
   Copyright (c) 2016 Boot&Work Corp., S.L.

   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __INDUSTRIALSHIELDS_RS485_H__
#define __INDUSTRIALSHIELDS_RS485_H__

#include <Arduino.h>

#if defined(HAVE_RS485)

#if defined(HAVE_RS485_HARD)
#include <Uart.h>

class RS485Class : public Uart {
	public:
		RS485Class(int numSerial);

	public:
		virtual void begin(unsigned long baud, uint8_t duplex = HALFDUPLEX, uint32_t config = SERIAL_8N1);

	public:
		virtual size_t write(uint8_t value);
		virtual size_t write(const uint8_t *buffer, size_t size);
		using Uart::write;

	public:
		bool isTxMode() const;
		void enableTx();
		void disableTx();

	private:
		bool txMode;
};

#endif // HAVE_RS485_HARD

extern RS485Class RS485;

#endif // HAVE_RS485

#endif // __INDUSTRIALSHIELDS_RS485_H__
