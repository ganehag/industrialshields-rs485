#include "RS485.h"

#if defined(HAVE_RS485)

#if defined(HAVE_RS485_HARD)

#include <driver/uart.h>

static void IRAM_ATTR rs485_isr(void* arg) {
	RS485Class* rs485 = (RS485Class*) arg;
	if (rs485->isTxMode() && (rs485->availableForWrite() == UART_FIFO_LEN)) {
		rs485->disableTx();
	}
}

RS485Class::RS485Class(int numSerial): Uart(numSerial), txMode(false) {
}

void RS485Class::begin(unsigned long baud, uint8_t duplex, uint32_t config) {
	pinMode(RS485_DE_RE, OUTPUT);
	disableTx();
	Uart::begin(baud, config);

	/*
	Disabled until someone finds a fix for this;

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
	*/

	//uart_isr_free(RS485_HWSERIAL);
	//uart_isr_register(RS485_HWSERIAL, rs485_isr, this, ESP_INTR_FLAG_IRAM, nullptr);
}

/*
 * Write a single byte
*/
size_t RS485Class::write(uint8_t value) {
	enableTx();
	size_t ret = Uart::write(value);
	Uart::flush();  // Flush
	disableTx();
	return ret;
}

/*
 * Write multiple bytes at once
*/
size_t RS485Class::write(const uint8_t *buffer, size_t size) {
	enableTx();
	size_t ret = Uart::write(buffer, size);
	Uart::flush();
	disableTx();
	return ret;
}

/*
 * Set the RS485 chip in TX mode
*/
void RS485Class::enableTx() {
	txMode = true;
	digitalWrite(RS485_DE_RE, HIGH);
}

/*
 * Set the RS485 chip in RX mode
*/
void RS485Class::disableTx() {
	digitalWrite(RS485_DE_RE, LOW);
	txMode = false;
}

/*
 * Check the current transmission mode
*/
bool RS485Class::isTxMode() const {
	return txMode;
}

RS485Class RS485(RS485_HWSERIAL);

#endif // HAVE_RS485_HARD

void RS485Event() __attribute__((weak));

void RS485EventRun() {
	if (RS485Event && RS485.available()) {
		RS485Event();
	}
}

#endif
