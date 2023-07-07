#ifndef ESP32_IMU_MONITOR_H
#define ESP32_IMU_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif


void imu_task(void *arg);
esp_err_t imu_monitor_init();

#ifdef __cplusplus
}

#endif

#endif