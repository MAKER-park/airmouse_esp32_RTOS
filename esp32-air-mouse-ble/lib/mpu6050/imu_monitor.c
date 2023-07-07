#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_err.h"

#include "driver/i2c.h"
#include "imu_monitor.h"
#include "inv_mpu.h"
#include "oled_control.h"
#include "hidd.h"

extern int xMailbox_Idx;
extern imu_data_t data;

//-----------------low pass filter
#define SENSITIVITY_X 0.5  // X축 감도 조절
#define SENSITIVITY_Y 0.5  // Y축 감도 조절
#define MAX_MOUSE_DELTA 100  // 최대 마우스 이동량 제한

void calculateMouseMovement(float ax, float ay, int8_t* dx, int8_t* dy) {
    *dx = (int8_t)(ax * SENSITIVITY_X)*10;
    *dy = (int8_t)(ay * SENSITIVITY_Y)*10;

	if (*dx > MAX_MOUSE_DELTA)
	{
		*dx = MAX_MOUSE_DELTA;
	}
	else if (*dx < -MAX_MOUSE_DELTA)
	{
		*dx = -MAX_MOUSE_DELTA;
	}

	if (*dy > MAX_MOUSE_DELTA)
	{
		*dy = MAX_MOUSE_DELTA;
	}
	else if (*dy < -MAX_MOUSE_DELTA)
	{
		*dy = -MAX_MOUSE_DELTA;
	}
	//printf("x : %d , y : %d\n", *dy, *dx);
	hid_mouse_task(*dx,*dy);
}

void imu_task(void *arg)
{
	float att[3] = {0,}; //pitch, roll, yaw;
	float acc[3] = {0,};
	float gyr[3] = {0,};
	int dx, dy;

	for(;;){
		int flag = 0;
		//imu_data_t data={0,0,0};
    	while(mpu_dmp_get_data(att, acc, gyr) != 0){
			//hid_mouse_task((int8_t)att[1], (int8_t)att[0]);
			if (xMailbox_Idx == 0){
				flag = 0;
			}
			else if (xMailbox_Idx == 1){
				flag = 1;
				data.x = att[0]; data.y = att[1]; data.z = att[2];
			}
			else if (xMailbox_Idx == 2){
				flag = 1;
				data.x = acc[0]; data.y = acc[1]; data.z = acc[2];
			}
			else if (xMailbox_Idx == 3){
				flag = 1;
				data.x = gyr[0]; data.y = gyr[1]; data.z = gyr[2];
			}
			vTaskDelay(1 / portTICK_PERIOD_MS);
		}
		//printf("x %1.2f, y %1.2f, z %1.2f\n", acc[0]*10, acc[1]*10, acc[2]*100);
		calculateMouseMovement(-1.0 * acc[2] * 10.0, -1.0 * acc[1] * 10.0, &dx, &dy);
		// fflush(stdout);
    	vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    // Exit
    vTaskDelay(100 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

esp_err_t imu_monitor_init()
{
    uint8_t flag = 1;

	flag = mpu_dmp_init();	
    if (flag!=0){
	    printf("Error Number %d\n", flag); // 0 : No Error
        return ESP_FAIL;
    }
	vTaskDelay(2000 / portTICK_PERIOD_MS); 
    xTaskCreate(imu_task, "imu_task", 2048, NULL, 10, NULL);

    return ESP_OK;
}