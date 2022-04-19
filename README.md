# RS485 for Industrial Shields ESP32 devices (10/IO, PLC, ...)

When trying to implement RS485 on an Industrial Shields 10/IO, ESP based "PLC". I encounter these issues;

- When trying to use Industrial Shield's own Modbus RTU library, nothing was sent over the RS485 interface.
- The CPU crashed whenever the PLC received data over the RS485 interface.

After several workdays, which also required me to rip the PLC into pieces, and with the help of a logic analyzer, I managed to figure out that the RS485 transceiver received the Uart data. But the ESP32 never told the RS485 chip to go into TX mode.

Digging into the RS485.cpp file in `packages/industrialshields/hardware/esp32/2.0.7/libraries/RS485`, and after more time than I am comfortable admitting. I realized that the RS485 implementation only contained `write(uint8_t)` and not the `write(const uint8_t *buffer, size_t size)` method used by the Modbus RTU library. 

The `write` function in the `RS485Class` object is responsible for setting the `enable TX` line (`RS485_DE_RE`) so that the RS485 chip knows it's in transmit mode.

Since this method (`RS485Class::write(const uint8_t *buffer, size_t size)`) was missing, the code in `Uart::write(const uint8_t *buffer, size_t size)` was the code triggered in its stead. The result is that the RS485 transceiver never transmitted any data.

Sigh.

However, this was not the only issue. Whenever I tried to receive data over the RS485 interface, it crashed the CPU;


```text
Guru Meditation Error: Core  1 panic'ed (LoadProhibited). Exception was unhandled.
Core 1 register dump:
PC      : 0x400d5439  PS      : 0x00060330  A0      : 0x800d1195  A1      : 0x3ffb1f30  
A2      : 0x00000002  A3      : 0x00000000  A4      : 0x0800001e  A5      : 0x00000003  
A6      : 0x00000001  A7      : 0x00000000  A8      : 0x800d1108  A9      : 0x3ffb1ef0  
A10     : 0x3ffbfef8  A11     : 0x00004b00  A12     : 0x0800001e  A13     : 0x00000010  
A14     : 0x00000011  A15     : 0x00000000  SAR     : 0x0000000f  EXCCAUSE: 0x0000001c  
EXCVADDR: 0x0000000c  LBEG    : 0x4000c46c  LEND    : 0x4000c477  LCOUNT  : 0x00000000  

Backtrace: 0x400d5439:0x3ffb1f30 0x400d1192:0x3ffb1f60 0x400d0d08:0x3ffb1f80 0x400d1736:0x3ffb1fb0 0x40085a79:0x3ffb1fd0

Rebooting...
```

After even more work, I found that this was due to the following lines in RS485.cpp.

```cpp
  // from RS485Class::begin
  uart_isr_free(RS485_HWSERIAL);
  uart_isr_register(RS485_HWSERIAL, rs485_isr, this, ESP_INTR_FLAG_IRAM, nullptr);
```

Double sigh.

These lines exist as a way to reset the `enable TX` line (`RS485_DE_RE`) once the data transmission has ended. Unfortunately, however, on my hardware, this is broken and crashes the ESP32 CPU. Why? I don't know.

Disabling these lines means that we need to add another way to switch the RS485 transceiver into receive mode.

Well, I solved it the lazy way. Using `Uart::flush()` and executing the `disableTx()` method inside each `RS485::write` operation. Another solution would be to remove the `enableTx()` call from `RS485::write` and move these into the Modbus RTU library since the library knows when it's done writing.

That's four days of my life I will never get back...
