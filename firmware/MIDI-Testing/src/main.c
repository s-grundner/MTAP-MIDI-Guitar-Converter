/**
 * @file main.c
 * @author @s-grundner
 * @brief main file for gitcon project
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "gitcon.h"

#define USER_LOCAL_LEVEL ESP_LOG_ERROR
static const char *TAG = "main";

#define DIP_1 (GPIO_NUM)
#define DIP_2 (GPIO_NUM)
#define DIP_3 (GPIO_NUM)
#define DIP_4 (GPIO_NUM)
#define DIP_5 (GPIO_NUM)
#define DIP_6 (GPIO_NUM)
#define DIP_7 (GPIO_NUM)
#define DIP_8 (GPIO_NUM)
#define DIP_9 (GPIO_NUM)

static void adc_reader_task(void *args)
{
	while (1)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

typedef struct
{
	QueueHandle_t i2s_queue;
	size_t *audio_buffer_pos;
	size_t *current_audio_buffer;

} i2s_sampler_t;

void app_main(void)
{
	gitcon_handle_t handle;
	if (gitcon_init(&handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "gitcon_init failed");
		ESP_ERROR_CHECK(gitcon_exit(handle));
		return;
	}

	QueueHandle_t i2s_queue;

	i2s_config_t i2s_cfg = {
		.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
		.sample_rate = 40000,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
		.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
		.communication_format = I2S_COMM_FORMAT_STAND_I2S,
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
		.dma_buf_count = 4,
		.dma_buf_len = 1024,
		.use_apll = false,
		.fixed_mclk = 0};

	// ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM_0, &i2s_cfg, 0, &i2s_queue));
	// ESP_ERROR_CHECK(i2s_set_adc_mode(ADC_UNIT_1, ADC_CHANNEL_1));
	// ESP_ERROR_CHECK(i2s_adc_enable(I2S_NUM_0));

	// TaskHandle_t adc_reader_task_handle;
	// xTaskCreatePinnedToCore(adc_reader_task, "adc_reader_task", 4096, NULL, 5, &adc_reader_task_handle, 0);

	while (1)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}