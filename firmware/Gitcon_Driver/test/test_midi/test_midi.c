/**
 * @file test_midi.c
 * @author @s-grundner
 * @brief Unit Tests for MIDI Driver
 * @version 0.1
 * @date 2023-03-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <midi.h>
#include <unity.h>

static midi_handle_t midi_handle;

#define MIDI_UART (UART_NUM_1)
#define MIDI_BAUD (115200)
#define MIDI_TX (GPIO_NUM_26)
#define MIDI_RX (GPIO_NUM_27)

void setUp(void)
{
	static midi_config_t midi_cfg = {
		.uart_num = MIDI_UART,
		.baudrate = MIDI_BAUD,
		.rx_io = MIDI_RX,
		.tx_io = MIDI_TX};
	ESP_ERROR_CHECK(midi_init(&midi_handle, &midi_cfg));
}

void tearDown(void)
{
	midi_exit(midi_handle);
}

void test_midi_blink(void)
{
	midi_message_t msg = note_on(0, 0x3C, 0x7F);
	TEST_ASSERT_EQUAL(ESP_OK, midi_write(midi_handle, &msg));
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	msg.status = MIDI_STATUS_NOTE_OFF;
	TEST_ASSERT_EQUAL(ESP_OK, midi_write(midi_handle, &msg));
}

void test_midi_bend(void)
{
	midi_message_t msg = pitch_bend(0, MIDI_PITCH_BEND_MAX);
	TEST_ASSERT_EQUAL(ESP_OK, midi_write(midi_handle, &msg));
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	msg = pitch_bend(0, MIDI_PITCH_BEND_MIN);
	TEST_ASSERT_EQUAL(ESP_OK, midi_write(midi_handle, &msg));
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	msg = pitch_bend(0, MIDI_PITCH_BEND_CENTER);
	TEST_ASSERT_EQUAL(ESP_OK, midi_write(midi_handle, &msg));
}

void test_midi_bent_note(void)
{
	midi_message_t msg = note_on(0, 0x3C, 0x7F);
	TEST_ASSERT_EQUAL(ESP_OK, midi_write(midi_handle, &msg));

	// ramp up and down pitch bend in 1 second
	midi_message_t msg_bend = pitch_bend(0, MIDI_PITCH_BEND_CENTER);
	for (int i = MIDI_PITCH_BEND_CENTER; i <= MIDI_PITCH_BEND_MAX; i += 128)
	{
		msg_bend = pitch_bend(0, i);
		TEST_ASSERT_EQUAL(ESP_OK, midi_write(midi_handle, &msg_bend));
		vTaskDelay(4 / portTICK_PERIOD_MS);
	}
	for (int i = MIDI_PITCH_BEND_MAX; i >= MIDI_PITCH_BEND_MIN; i -= 128)
	{
		msg_bend = pitch_bend(0, i);
		TEST_ASSERT_EQUAL(ESP_OK, midi_write(midi_handle, &msg_bend));
		vTaskDelay(2 / portTICK_PERIOD_MS);
	}
	for (int i = MIDI_PITCH_BEND_MIN; i <= MIDI_PITCH_BEND_CENTER; i += 128)
	{
		msg_bend = pitch_bend(0, i);
		TEST_ASSERT_EQUAL(ESP_OK, midi_write(midi_handle, &msg_bend));
		vTaskDelay(4 / portTICK_PERIOD_MS);
	}
	msg.status = MIDI_STATUS_NOTE_OFF;
	TEST_ASSERT_EQUAL(ESP_OK, midi_write(midi_handle, &msg));
}

void app_main()
{
	UNITY_BEGIN();

	RUN_TEST(test_midi_blink);
	RUN_TEST(test_midi_bend);
	RUN_TEST(test_midi_bent_note);

	UNITY_END();
}