#include "i2s_sampler.h"

static const char *TAG = "dma_test";

void app_main(void)
{
	// ------------------------------------------------------------
	// SETUP SAMPLER
	// ------------------------------------------------------------

	i2s_sampler_t *sampler = sampler_start(ADC1_CHANNEL_0, 1 << 13);

	sampler_stop(sampler);

	for (;;)
		vTaskDelay(1000 / portTICK_PERIOD_MS);

	// 1: B
	// 2: H
	// 3: A
	// 4: F
	// 5: E
	// 6: I
	// 7: G
}