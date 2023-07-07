#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "tactile_switch.h"
#include "oled_control.h"
#include "esp_timer.h"

//add
#include "hidd.h"

#define TAG "BUTTON"

// #define BUTTON_1    (32) //down
// #define BUTTON_2    (33) //right
// #define BUTTON_3    (25) //up
// #define BUTTON_4    (26) //click
// #define BUTTON_5    (27) //left

#define BUTTON_1 GPIO_NUM_26    //z
#define BUTTON_2 GPIO_NUM_33    //d
#define BUTTON_3 GPIO_NUM_25    //w
#define BUTTON_4 GPIO_NUM_27    //a
#define BUTTON_5 GPIO_NUM_32    //s
#define BUTTON_L GPIO_NUM_14    
#define BUTTON_R GPIO_NUM_12    

gpio_pull_mode_t pull_mode = GPIO_PULLUP_ONLY;
unsigned long long pin_select = PIN_BIT(BUTTON_1) | PIN_BIT(BUTTON_2) | PIN_BIT(BUTTON_3) | PIN_BIT(BUTTON_4) | PIN_BIT(BUTTON_5) | PIN_BIT(BUTTON_L) | PIN_BIT(BUTTON_R);

button_event_t ev;
QueueHandle_t button_events;

typedef struct {
  uint8_t pin;
  bool inverted;
  uint16_t history;
  uint32_t down_time;
  uint32_t next_long_time;
} debounce_t;

int pin_count = -1;
debounce_t * debounce;
QueueHandle_t queue;

static void update_button(debounce_t *d) {
    d->history = (d->history << 1) | gpio_get_level(d->pin);
}

#define MASK   0b1111000000111111
static bool button_rose(debounce_t *d) {
    if ((d->history & MASK) == 0b0000000000111111) {
        d->history = 0xffff;
        return 1;
    }
    return 0;
}
static bool button_fell(debounce_t *d) {
    if ((d->history & MASK) == 0b1111000000000000) {
        d->history = 0x0000;
        return 1;
    }
    return 0;
}
static bool button_down(debounce_t *d) {
    if (d->inverted) return button_fell(d);
    return button_rose(d);
}
static bool button_up(debounce_t *d) {
    if (d->inverted) return button_rose(d);
    return button_fell(d);
}

static uint32_t millis() {
    return esp_timer_get_time() / 1000;
}

static void send_event(debounce_t db, int ev) {
    button_event_t event = {
        .pin = db.pin,
        .event = ev,
    };
    xQueueSend(queue, &event, portMAX_DELAY);
}

static void oled_control_task(void* pvParameter){
    while (true) {
		if (xQueueReceive(queue, &ev, 1000/portTICK_PERIOD_MS)) {
			if ((ev.pin == BUTTON_1) && (ev.event == BUTTON_DOWN)) {
				console_uart_parser('z');
				ESP_LOGI("TS", "Click (Selected)");
			}
			if ((ev.pin == BUTTON_2) && (ev.event == BUTTON_DOWN)) {
				console_uart_parser('d');
				ESP_LOGI("TS", "Right");
			}
			if ((ev.pin == BUTTON_3) && (ev.event == BUTTON_DOWN)) {
				console_uart_parser('w');
				ESP_LOGI("TS", "Up");
			}
			if ((ev.pin == BUTTON_4) && (ev.event == BUTTON_DOWN)) {
				console_uart_parser('a');
				ESP_LOGI("TS", "Left");
			}
			if ((ev.pin == BUTTON_5) && (ev.event == BUTTON_DOWN)) {
				console_uart_parser('s');
				ESP_LOGI("TS", "Down");
			}
            if ((ev.pin == BUTTON_L) && (ev.event == BUTTON_DOWN)) {
				//console_uart_parser('s');
				ESP_LOGI("TS", "Click Left");
                hid_button_task('L');
			}
            if ((ev.pin == BUTTON_R) && (ev.event == BUTTON_DOWN)) {
				//console_uart_parser('s');
				ESP_LOGI("TS", "Click Right");
                hid_button_task('R');
			}
		}
	}
}

static void button_task(void *pvParameter)
{
    while(true) {
        for (int idx=0; idx<pin_count; idx++) {
            update_button(&debounce[idx]);
            if (button_up(&debounce[idx])) {
                debounce[idx].down_time = 0;
                //ESP_LOGI(TAG, "%d UP", debounce[idx].pin);
                //send_event(debounce[idx], BUTTON_UP);
            } else if (debounce[idx].down_time && millis() >= debounce[idx].next_long_time) {
                //ESP_LOGI(TAG, "%d LONG", debounce[idx].pin);
                debounce[idx].next_long_time = debounce[idx].next_long_time + CONFIG_ESP32_BUTTON_LONG_PRESS_REPEAT_MS;
                //send_event(debounce[idx], BUTTON_HELD);
            } else if (button_down(&debounce[idx]) && debounce[idx].down_time == 0) {
                debounce[idx].down_time = millis();
                //ESP_LOGI(TAG, "%d DOWN", debounce[idx].pin);
                debounce[idx].next_long_time = debounce[idx].down_time + CONFIG_ESP32_BUTTON_LONG_PRESS_DURATION_MS;
                send_event(debounce[idx], BUTTON_DOWN);
            } 
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

esp_err_t tactile_switch_init() {
    esp_err_t ret;

    if (pin_count != -1) {
        ESP_LOGI(TAG, "Already initialized");
        return ESP_FAIL;
    }

    // Configure the pins
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // GPIO_INTR_POSEDGE
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = (pull_mode == GPIO_PULLUP_ONLY || pull_mode == GPIO_PULLUP_PULLDOWN);
    io_conf.pull_down_en = (pull_mode == GPIO_PULLDOWN_ONLY || pull_mode == GPIO_PULLUP_PULLDOWN);;
    io_conf.pin_bit_mask = pin_select;
    ret =  gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GPIO initialize err %04x", ret);
        return ret;
    }

    // Scan the pin map to determine number of pins
    pin_count = 0;
    for (int pin=0; pin<=39; pin++) {
        if ((1ULL<<pin) & pin_select) {
            pin_count++;
        }
    }

    // Initialize global state and queue
    debounce = calloc(pin_count, sizeof(debounce_t));
    queue = xQueueCreate(CONFIG_ESP32_BUTTON_QUEUE_SIZE, sizeof(button_event_t));

    // Scan the pin map to determine each pin number, populate the state
    uint32_t idx = 0;
    for (int pin=0; pin<=39; pin++) {
        if ((1ULL<<pin) & pin_select) {
            ESP_LOGI(TAG, "Registering button input: %d", pin);
            debounce[idx].pin = pin;
            debounce[idx].down_time = 0;
            debounce[idx].inverted = true;
            if (debounce[idx].inverted) debounce[idx].history = 0xffff;
            idx++;
        }
    }

    // Spawn a task to monitor the pins
    xTaskCreate(&button_task, "button_task", CONFIG_ESP32_BUTTON_TASK_STACK_SIZE, NULL, 10, NULL);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    xTaskCreate(&oled_control_task, "oled_control_task", CONFIG_ESP32_BUTTON_TASK_STACK_SIZE, NULL, 10, NULL);

    return ESP_OK;
}