/**
 * @file main.c
 * @author @s-grundner
 * @brief Main File for Gitcon Project
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "gitcon.h"

#define USER_LOCAL_LEVEL ESP_LOG_ERROR

// when PROTOTYPE is 1, the prototype board 1 is used
#define PROTOTYPE 0

static const char *TAG = "main";

#if PROTOTYPE == 1
#define DIP_POL 8
#define DEBOUNCE_TIME_MS 50

static const char WHOLE_TONE[8] = {0, 2, 4, 5, 7, 9, 11, 12};

static const gpio_num_t DIP_IO[DIP_POL] = {
	GPIO_NUM_32,
	GPIO_NUM_33,
	GPIO_NUM_25,
	GPIO_NUM_26,
	GPIO_NUM_27,
	GPIO_NUM_14,
	GPIO_NUM_12,
	GPIO_NUM_13,
};

typedef struct
{
	QueueHandle_t midi_queue;
	size_t *current_dip;
	size_t *previous_dip;
	TimerHandle_t *timer;
} dip_switch_t;

static void IRAM_ATTR dip_isr(void *args)
{
	dip_switch_t *dip_switch = (dip_switch_t *)args;
	for (size_t i = 0; i < DIP_POL; i++)
	{
		// read DIP switch
		dip_switch->current_dip[i] = !gpio_get_level(DIP_IO[i]);

		// continue if the dip state hasn't changed
		if (dip_switch->current_dip[i] == dip_switch->previous_dip[i])
			continue;

		// send MIDI message
		midi_message_t msg = {
			.status = (midi_status_t)(dip_switch->current_dip[i]) ? MIDI_STATUS_NOTE_ON : MIDI_STATUS_NOTE_OFF,
			.channel = 0,
			.param1 = 0x3C + WHOLE_TONE[i], // C4 + WHOLE_TONE[i]
			.param2 = 127};
		xQueueSendFromISR(dip_switch->midi_queue, &msg, NULL);

		// disable interrupts for gpios to debounce
		ESP_ERROR_CHECK(gpio_intr_disable(DIP_IO[i]));
		dip_switch->previous_dip[i] = dip_switch->current_dip[i];
		xTimerStartFromISR(dip_switch->timer[i], NULL);
	}
}

static void IRAM_ATTR debounce_task(TimerHandle_t debounce_timer)
{
	int i = (int)pvTimerGetTimerID(debounce_timer);
	gpio_intr_enable(DIP_IO[i]);
}
#endif

void app_main(void)
{
	gitcon_handle_t handle;
	
	if (gitcon_init(&handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "gitcon_init failed");
		ESP_ERROR_CHECK(gitcon_exit(handle));
		return;
	}
#if PROTOTYPE == 1
	xTimerHandle debounce_timers[DIP_POL];

	for (size_t i = 0; i < DIP_POL; i++)
		debounce_timers[i] = xTimerCreate("dip_switch", pdMS_TO_TICKS(DEBOUNCE_TIME_MS), pdFALSE, (void *)i, debounce_task);

	size_t current_dip[DIP_POL] = {0};
	size_t previous_dip[DIP_POL] = {0};

	// setup debouncing for DIP Switches
	dip_switch_t dip_switch = {
		.current_dip = current_dip,
		.previous_dip = previous_dip,
		.timer = debounce_timers,
		.midi_queue = handle->midi_queue};

	// setup interrupt for DIP switches
	ESP_ERROR_CHECK(gpio_install_isr_service(0));
	for (size_t i = 0; i < DIP_POL; i++)
	{
		gpio_config_t io_conf = {
			.pin_bit_mask = (1ULL << DIP_IO[i]),
			.mode = GPIO_MODE_INPUT,
			.pull_up_en = GPIO_PULLUP_ENABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_ANYEDGE};

		ESP_ERROR_CHECK(gpio_config(&io_conf));
		ESP_ERROR_CHECK(gpio_isr_handler_add(DIP_IO[i], dip_isr, &dip_switch));
	}
#endif

	while (1)
		vTaskDelay(1000 / portTICK_PERIOD_MS);
}