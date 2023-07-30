/**
 * @file midi.c
 * @author @s-grundner
 * @brief MIDI UART Driver Source for ESP32
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "midi.h"

#define MIDI_BYTE_SIZE_DEFAULT 3
#define MIDI_BYTE_SIZE_SHORT 2

static const char *TAG = "MIDI";
static const char *MIDI_MON = "MIDI MONITOR";

/**
 * @brief midi data
 * @struct midi_data_t
 * @param uart MIDI Config
 * @param queue MIDI Queue
 */
typedef struct
{
	struct
	{
		uart_port_t port;
		uint baudrate;
		gpio_num_t rx_io;
		gpio_num_t tx_io;
	} uart;
} midi_data_t;

// ------------------------------------------------------------
// MIDI CONFIG
// ------------------------------------------------------------

esp_err_t midi_init(midi_handle_t *out_data, uart_port_t uart_port_num, uint baudrate, gpio_num_t rx_io, gpio_num_t tx_io)
{
	// Allocate memory for context
	midi_data_t *data = (midi_data_t *)malloc(sizeof(midi_data_t));
	if (!data)
		return ESP_ERR_NO_MEM;

	*data = (midi_data_t){
		.uart = {
			.port = uart_port_num,
			.baudrate = baudrate,
			.rx_io = rx_io,
			.tx_io = tx_io},
	};

	ESP_LOGI(TAG, "Initializing MIDI on %d (rx:%d, tx:%d) with %d baud...", data->uart.port, data->uart.rx_io, data->uart.tx_io, data->uart.baudrate);

	// Configure UART
	gpio_config_t rx_pin_config = {
		.pin_bit_mask = (1ULL << data->uart.rx_io),
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE};

	gpio_config_t tx_pin_config = {
		.pin_bit_mask = (1ULL << data->uart.tx_io),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE};

	ESP_ERROR_CHECK(gpio_config(&rx_pin_config));
	ESP_ERROR_CHECK(gpio_config(&tx_pin_config));

	uart_config_t uart_config = {
		.baud_rate = data->uart.baudrate,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB};

	ESP_ERROR_CHECK(uart_param_config(data->uart.port, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(data->uart.port, data->uart.tx_io, data->uart.rx_io, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK(uart_driver_install(data->uart.port, 1024 * 2, 1024 * 2, 0, NULL, 0));

	// Pass configured context to outer parameters
	*out_data = data;
	return ESP_OK;
}

esp_err_t midi_exit(midi_handle_t midi_handle)
{
	esp_err_t err = ESP_OK;
	err = uart_driver_delete(midi_handle->uart.uart_num);
	free(midi_handle);
	return err;
}

// ------------------------------------------------------------
// MIDI TRANSMISSIONS
// ------------------------------------------------------------

esp_err_t midi_write(midi_handle_t handle, midi_message_t *msg)
{
	int len = 0;
	const char data[] = {msg->status | msg->channel, msg->param1, msg->param2};

	// switch status to determine message length
	switch (msg->status)
	{
	case MIDI_STATUS_NOTE_OFF:
	case MIDI_STATUS_NOTE_ON:
	case MIDI_STATUS_CONTROL_CHANGE:
	case MIDI_STATUS_PITCH_BEND:
	case MIDI_STATUS_POLYPHONIC_KEY_PRESSURE:
		len = uart_write_bytes(handle->uart.uart_num, data, MIDI_BYTE_SIZE_DEFAULT);
		break;
	case MIDI_STATUS_PROGRAM_CHANGE:
	case MIDI_STATUS_CHANNEL_PRESSURE:
		len = uart_write_bytes(handle->uart.uart_num, data, MIDI_BYTE_SIZE_SHORT);
		break;
	default:
		ESP_LOGE(TAG, "midi_send: invalid status: %02X", msg->status);
		return ESP_ERR_INVALID_ARG;
		break;
	}
	switch (len)
	{
	case -1:
		ESP_LOGE(TAG, "uart_write_bytes failed");
		return ESP_FAIL;
		break;
	case 0:
		ESP_LOGE(TAG, "uart_write_bytes timed out");
		return ESP_ERR_TIMEOUT;
		break;
	default:
		// ESP_LOGI(MIDI_MON, "Status: %02X\tChannel: %02X\t Data: %02X %02X\t Length:%d", msg->status, msg->channel, msg->param1, msg->param2, len);
		break;
	}
	return ESP_OK;
}

esp_err_t midi_read(midi_handle_t midi_handle, midi_message_t *msg, TickType_t timeout)
{
	char data[3];
	int len = uart_read_bytes(midi_handle->uart.uart_num, (uint8_t *)data, 3, timeout);
	switch (len)
	{
	case -1:
		ESP_LOGE(TAG, "uart_read_bytes failed");
		return ESP_FAIL;
		break;
	case 0:
		ESP_LOGE(TAG, "uart_read_bytes timeout");
		return ESP_ERR_TIMEOUT;
		break;
	case 2:
		msg->status = data[0] & 0xF0;
		msg->channel = data[0] & 0x0F;
		msg->param1 = data[1];
		msg->param2 = 0;
		break;
	case 3:
		msg->status = data[0] & 0xF0;
		msg->channel = data[0] & 0x0F;
		msg->param1 = data[1];
		msg->param2 = data[2];
		break;
	default:
		ESP_LOGE(TAG, "uart_read_bytes invalid message");
		return ESP_ERR_INVALID_ARG;
		break;
	}
	return ESP_OK;
}
