/**
 * @file midi.c
 * @brief
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "midi.h"

static const char *TAG = "midi";

esp_err_t midi_init(uart_port_t uart_port, int baudrate, gpio_num_t rx_pin, gpio_num_t tx_pin)
{
	// ------------------------------------------------------------
	// INIT USART1 DRIVER
	// ------------------------------------------------------------

	uart_config_t uart_config = {
		.baud_rate = baudrate,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB,
	}; // Configure UART parameters

	ESP_ERROR_CHECK(uart_driver_install(uart_port, 1024 * 2, 0, 0, NULL, 0));
	ESP_ERROR_CHECK(uart_param_config(uart_port, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(uart_port, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	return ESP_OK;
}

esp_err_t midi_send(uart_port_t uart_port, midi_message_t *message)
{
	// ------------------------------------------------------------
	// SEND MIDI MESSAGE
	// ------------------------------------------------------------
	uint8_t data[MIDI_UART_DATA_SIZE] = {
		(message->status & 0xF0) | (message->channel & 0x0F),
		message->note_num & 0x7F,
		message->velocity & 0x7F,
	};

	ESP_LOG_BUFFER_HEXDUMP(TAG, data, MIDI_UART_DATA_SIZE, ESP_LOG_INFO);
	ESP_ERROR_CHECK(uart_write_bytes(uart_port, (const char *)data, MIDI_UART_DATA_SIZE));

	return ESP_OK;
}

esp_err_t midi_exit(uart_port_t uart_port)
{
	return uart_driver_delete(uart_port);
}