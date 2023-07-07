#ifndef ESP32_HIDD_H
#define ESP32_HIDD_H

#ifdef __cplusplus
extern "C" {
#endif


//void imu_task(void *arg);
//esp_err_t imu_monitor_init();
void hid_init();
void hid_button_task(char button);
void hid_mouse_task(int8_t x, int8_t y);
void hid_uart_task(char character); 
#ifdef __cplusplus
}

#endif

#endif