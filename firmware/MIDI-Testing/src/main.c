#include "gitcon.h"

void app_main(void)
{
	gitcon_handle_t gitcon_handle;
	if (gitcon_init(&gitcon_handle) != ESP_OK)
	{
		ESP_LOGE("GITCON", "gitcon_init failed");
		return;
	}

	for (;;)
		vTaskDelay(1000 / portTICK_PERIOD_MS);
}