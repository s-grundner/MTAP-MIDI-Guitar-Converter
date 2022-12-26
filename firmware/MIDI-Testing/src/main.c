/**
 * @file main.c
 * @author @s-grundner
 * @brief
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "gitcon.h"

static const char *TAG = "main";

void app_main(void)
{
	gitcon_handle_t handle;
	if (gitcon_init(&handle) != ESP_OK)
	{
		ESP_ERROR_CHECK(gitcon_exit(handle));
		ESP_LOGE(TAG, "gitcon_init failed");
		return;
	}

	while (1)
		vTaskDelay(10 / portTICK_PERIOD_MS);
}
