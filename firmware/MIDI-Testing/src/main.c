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

void app_main(void)
{
	gitcon_handle_t handle;
	if (gitcon_init(&handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "gitcon_init failed");
		ESP_ERROR_CHECK(gitcon_exit(handle));
		return;
	}
	while (1)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
