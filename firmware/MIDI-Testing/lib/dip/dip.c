#include "dip.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_log.h"

#define DIP_POL 8
#define DEBOUNCE_TIME_MS 50

typedef struct
{
	QueueHandle_t midi_queue;
	int *current_dip;
	int *previous_dip;
	TimerHandle_t *timer;
} debounce_dip_t;

static const char whole_tone[8] = {0, 2, 4, 5, 7, 9, 11, 12};

static const gpio_num_t dip_io[DIP_POL] = {
	GPIO_NUM_32,
	GPIO_NUM_33,
	GPIO_NUM_25,
	GPIO_NUM_26,
	GPIO_NUM_27,
	GPIO_NUM_14,
	GPIO_NUM_12,
	GPIO_NUM_13,
};

static void IRAM_ATTR dip_isr(void *args)
{
	debounce_dip_t *debounce = (debounce_dip_t *)args;
	// send MIDI message

	for (int i = 0; i < DIP_POL; i++)
	{
		// read DIP switch
		debounce->current_dip[i] = !gpio_get_level(dip_io[i]);

		// continue if the dip state hasn't changed
		if (debounce->current_dip[i] == debounce->previous_dip[i])
			continue;

		// send MIDI message
		midi_message_t msg = {
			.status = (midi_status_t)(debounce->current_dip[i]) ? MIDI_STATUS_NOTE_ON : MIDI_STATUS_NOTE_OFF,
			.channel = 0,
			.param1 = 0x3C + whole_tone[i], // C4
			.param2 = 127};
		xQueueSendFromISR(debounce->midi_queue, &msg, NULL);

		// disable interrupts for gpios
		gpio_intr_disable(dip_io[i]);
		debounce->previous_dip[i] = debounce->current_dip[i];
		xTimerStart(debounce->timer[i], 0);
	}
}

static void IRAM_ATTR debounce_task(TimerHandle_t debounce_timer)
{
	int i = (int)pvTimerGetTimerID(debounce_timer);
	gpio_intr_enable(dip_io[i]);
}

// ------------------------------------------------------------

void init_dip(QueueHandle_t midi_queue)
{
	// setup debouncing for DIP Switches
	xTimerHandle debounce_timers[DIP_POL];

	for (int i = 0; i < DIP_POL; i++)
		debounce_timers[i] = xTimerCreate("debounce", pdMS_TO_TICKS(DEBOUNCE_TIME_MS), pdFALSE, (void *)i, debounce_task);

	int current_dip[DIP_POL] = {0};
	int previous_dip[DIP_POL] = {0};

	debounce_dip_t debounce = {
		.current_dip = current_dip,
		.previous_dip = previous_dip,
		.midi_queue = midi_queue,
		.timer = debounce_timers};

	// setup interrupt for DIP switches
	for (int i = 0; i < DIP_POL; i++)
	{
		gpio_config_t io_conf = {
			.pin_bit_mask = (1ULL << dip_io[i]),
			.mode = GPIO_MODE_INPUT,
			.pull_up_en = GPIO_PULLUP_ENABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_ANYEDGE};
		gpio_config(&io_conf);
		gpio_install_isr_service(0);
		gpio_isr_handler_add(dip_io[i], dip_isr, &debounce);
	}
}