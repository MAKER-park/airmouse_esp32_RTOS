#ifndef OLED_CONTROL_H_
#define OLED_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MENU_NUM (6)

typedef struct{
	float x;
	float y;
	float z;
} imu_data_t;

typedef enum {
	KEY_RIGHT = 1,
	KEY_LEFT = 2,
	KEY_DOWN = 3,
	KEY_UP = 4,
	KEY_SELECTED = 5
} uart_console_key_type_t;

typedef struct {
	signed char sup_menu_num;
	signed char sup_menu_row;
	signed char sub_menu_num;
} menu_direction_t;

char* get_cur_menu_str();
void drawMenus();
void printMenus(uart_console_key_type_t key);
void console_uart_parser(uint8_t command);
void oled_control_init();


#ifdef __cplusplus
}
#endif

#endif /* OLED_CONTROL_H_ */

