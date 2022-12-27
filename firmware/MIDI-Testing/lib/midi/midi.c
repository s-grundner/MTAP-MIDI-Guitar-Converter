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

esp_err_t midi_init(const uart_port_t uart_port, int baudrate, gpio_num_t rx_pin, gpio_num_t tx_pin)
{
	// ------------------------------------------------------------
	// INIT USART1 DRIVER
	// ------------------------------------------------------------

	gpio_config_t rx_pin_config = {
		.pin_bit_mask = (1ULL << rx_pin),
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE};

	gpio_config_t tx_pin_config = {
		.pin_bit_mask = (1ULL << tx_pin),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE};

	ESP_ERROR_CHECK(gpio_config(&rx_pin_config));
	ESP_ERROR_CHECK(gpio_config(&tx_pin_config));

	uart_config_t uart_config = {
		.baud_rate = baudrate,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB,
	}; // Configure UART parameters

	ESP_ERROR_CHECK(uart_param_config(uart_port, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(uart_port, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK(uart_driver_install(uart_port, 1024 * 2, 1024 * 2, 0, NULL, 0));

	return ESP_OK;
}

esp_err_t midi_send(const uart_port_t uart_port, midi_message_t *message)
{
	// ------------------------------------------------------------
	// SEND MIDI MESSAGE
	// ------------------------------------------------------------

	ESP_LOGD(TAG, "midi_send: status: %02X, channel: %02X, note_num: %02X, velocity: %02X", message->status, message->channel, message->note_num, message->velocity);

	uint8_t data[MIDI_UART_DATA_SIZE] = {
		(message->status & 0xF0) | (message->channel & 0x0F),
		message->note_num & 0x7F,
		message->velocity & 0x7F,
	};
	if (uart_write_bytes(uart_port, (const char *)data, MIDI_UART_DATA_SIZE) == -1)
	{
		ESP_LOGE(TAG, "uart_write_bytes failed");
		return ESP_FAIL;
	}

	return ESP_OK;
}

esp_err_t midi_exit(const uart_port_t uart_port)
{
	return uart_driver_delete(uart_port);
}