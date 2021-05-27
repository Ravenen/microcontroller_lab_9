// #include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <config.h>
#include <crc16.h>

#define BAUD_CALC (F_CPU/(RS485_BAUD*16L)-1)

#define RX_PIN PD0
#define EN_PIN PD2

#define ADDRESS SLAVE_A_ADDRESS
#define data_distortion slave_a_data_distortion
uint8_t data[] = SLAVE_A_DATA;

uint8_t packet_number = 0;

uint8_t to_check_buffer[BYTES_TO_CHECK];
uint8_t crc_buffer[2];

enum RSMode {
	RECEIVING,
	TRANSMISSION
};

void sendData();
void setRSMode(RSMode mode);
void sendToRS(uint8_t _data);

ISR(USART_RX_vect) {
	if (UCSR0A & (1 << MPCM0)) {
		if (UDR0 == ADDRESS) {
			UCSR0A &= ~(1 << MPCM0);
		}
	}
	else {
		packet_number = UDR0;

		setRSMode(TRANSMISSION);
		sendData();
		sendToRS(EOT);

		UCSR0A = (1 << MPCM0);
	}
}

ISR(USART_TX_vect) {
	setRSMode(RECEIVING);
}

void init() {
	DDRD = ~(1 << RX_PIN) | (1 << EN_PIN);
	PORTD = (1 << RX_PIN);

	UBRR0 = (uint8_t)BAUD_CALC;
	UCSR0A = (1 << MPCM0);
	UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0) | (1 << UCSZ02);
	UCSR0C = (1 << USBS0) | (1 << UCSZ00) | (1 << UCSZ01);
	sei();
}

int main() {
	init();
	while (true) {
	}
}

void sendData() {
	uint8_t byte_to_distore = data_distortion[packet_number][0];
	uint8_t distortion_mask = data_distortion[packet_number][1];

	uint8_t iterator = 0;
	while (data[iterator] != '\0') {
		uint8_t original_byte = data[iterator];
		uint8_t byte_to_send = data[iterator];
		if (iterator == byte_to_distore) {
			byte_to_send ^= distortion_mask;
		}
		sendToRS(byte_to_send);
		to_check_buffer[iterator % BYTES_TO_CHECK] = original_byte;

		iterator++;

		if (iterator % BYTES_TO_CHECK == 0) {
			generateCrc(to_check_buffer, crc_buffer);
			sendToRS(crc_buffer[0]);
			sendToRS(crc_buffer[1]);
		}
	}

	// if data length is not multiple of BYTES_TO_CHECK
	for (int i = iterator % BYTES_TO_CHECK; i < BYTES_TO_CHECK; i++) {
		to_check_buffer[i] = '\0';
		sendToRS('\0');
	}
	generateCrc(to_check_buffer, crc_buffer);
	sendToRS(crc_buffer[0]);
	sendToRS(crc_buffer[1]);
}

void sendToRS(uint8_t _data) {
	while ((UCSR0A & (1 << UDRE0)) == 0) {};
	UDR0 = _data;
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