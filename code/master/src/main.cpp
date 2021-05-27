// #include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <config.h>

#define RS485_BAUD_CALC (F_CPU/(RS485_BAUD*16L)-1)
#define RS232_BAUD_CALC (F_CPU/(RS232_BAUD*16L)-1)
#define BUF_SIZE 16
#define BUF_MASK (BUF_SIZE-1)

#define RX_PIN PD2
#define EN_PIN PD1

uint8_t out_buffer[BUF_SIZE][2];
uint8_t out_buffer_start = 0;
uint8_t out_buffer_end = 0;

bool is_waiting_data = false;

volatile char data_from_slave = -1;
volatile char data_from_app = -1;
uint8_t packages_iterator = 0;
bool send_packages = false;

enum RSMode {
	RECEIVING,
	TRANSMISSION
};

enum MessageType {
	DATA = 0,
	ADDRESS = 1
};

void sendByteToUSART(uint8_t u8Data);
void setRSMode(RSMode mode);
void writeOutBuffer(MessageType message_type, uint8_t data);
bool isOutBufferEmpty();

ISR(USART0_RX_vect) {
	data_from_app = UDR0;
	if (data_from_app != AUTOMODE_ADDRESS) {
		writeOutBuffer(ADDRESS, data_from_app);
		writeOutBuffer(DATA, 0);
	}
	else {
		send_packages = true;
		packages_iterator = 0;
		writeOutBuffer(ADDRESS, SLAVE_A_ADDRESS);
		writeOutBuffer(DATA, packages_iterator);
		writeOutBuffer(ADDRESS, SLAVE_B_ADDRESS);
		writeOutBuffer(DATA, packages_iterator);
		packages_iterator++;
	}
}

ISR(USART1_RX_vect) {
	data_from_slave = UDR1;
	sendByteToUSART(data_from_slave);
	if (data_from_slave == EOT) {
		is_waiting_data = false;
		if (send_packages) {
			writeOutBuffer(ADDRESS, SLAVE_A_ADDRESS);
			writeOutBuffer(DATA, packages_iterator);
			writeOutBuffer(ADDRESS, SLAVE_B_ADDRESS);
			writeOutBuffer(DATA, packages_iterator);
			packages_iterator++;
			if (packages_iterator == 5) {
				send_packages = false;
			}
		}
		if (isOutBufferEmpty() == false) {
			UCSR1B |= 1 << UDRIE1;
		}
	}
}

ISR(USART1_TX_vect) {
	setRSMode(RECEIVING);
}

ISR(USART1_UDRE_vect) {
	setRSMode(TRANSMISSION);
	if (out_buffer[out_buffer_start][1] == 1) {
		UCSR1B |= 1 << TXB81;
	}
	else {
		UCSR1B &= ~(1 << TXB81);
		is_waiting_data = 1;
	}

	UDR1 = out_buffer[out_buffer_start][0];
	out_buffer_start++;
	out_buffer_start &= BUF_MASK;

	if (isOutBufferEmpty() || is_waiting_data) {
		UCSR1B &= ~(1 << UDRIE1);
	}
}

void init() {
	// RS-485
	UBRR1 = RS485_BAUD_CALC;
	UCSR1A = 0;
	UCSR1B = (1 << RXCIE1) | (1 << TXCIE1) | (1 << RXEN1) | (1 << TXEN1) | (1 << UCSZ12);
	UCSR1C = (1 << USBS1) | (1 << UCSZ10) | (1 << UCSZ11);
	// 9n2

	// RS-232
	UBRR0 = RS232_BAUD_CALC;
	UCSR0A = 0;
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	// 8n1

	sei();

	DDRD = ~(1 << RX_PIN) | (1 << EN_PIN);
	PORTD = (1 << RX_PIN) & ~(1 << EN_PIN);

	DDRA = 0xFF;
}

int main() {
	init();
	_delay_ms(100);
	while (true) {
	}
}


void sendByteToUSART(uint8_t u8Data) {
	while ((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = u8Data;
}

void setRSMode(RSMode mode) {
	switch (mode) {
	case RECEIVING:
		PORTD &= ~(1 << EN_PIN);
		break;
	case TRANSMISSION:
		PORTD |= (1 << EN_PIN);
		break;
	}
}

void writeOutBuffer(MessageType message_type, uint8_t data) {
	out_buffer[out_buffer_end][0] = data;
	out_buffer[out_buffer_end][1] = message_type;
	out_buffer_end++;
	out_buffer_end &= BUF_MASK;

	if (is_waiting_data == false) {
		UCSR1B |= 1 << UDRIE1;
	}
}

bool isOutBufferEmpty() {
	return out_buffer_start == out_buffer_end;
}