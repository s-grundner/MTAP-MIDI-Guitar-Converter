#include "gitcon.h"

void app_main(void)
{
	gitcon_handle_t handle;
	ESP_ERROR_CHECK(gitcon_init(&handle));
	ESP_ERROR_CHECK(gitcon_exit(handle));
}
