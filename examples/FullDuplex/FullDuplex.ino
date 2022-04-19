/*
   Copyright (c) 2016 Boot&Work Corp., S.L. All rights reserved

   This program is free software: you can redistribute it and/or modify
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

// Include Industrial Shields libraries
#include <RS485.h>

//// IMPORTANT: check switches configuration
//// IMPORTANT: Full duplex mode is only available when device supports it

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Begin serial port
	Serial.begin(9600);

  // Begin RS485 port
  RS485.begin(38400); //, FULLDUPLEX);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Wait bytes from the RS-485
  if (RS485.available()) {
    byte tx = RS485.read();

    // In full-duplex mode it is possible to send and receive data
    // at the same time in a secure way
    RS485.write(tx);

    // Echo the byte to the serial port
    Serial.write(tx);
  }
}
