#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_event.h"
//-----add-------------
#include "hidd.h"
//----------------------

#include <console_uart.h>
#include <oled_control.h>
#include <tactile_switch.h>
#include <imu_monitor.h>

#define tag "SSD1306"

extern "C" void app_main() {
    console_uart_init();
	vTaskDelay(1000/portTICK_PERIOD_MS);
	hid_init();
	oled_control_init();
	tactile_switch_init();
	imu_monitor_init();
}