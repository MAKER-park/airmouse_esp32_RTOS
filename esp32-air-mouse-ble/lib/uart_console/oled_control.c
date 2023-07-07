#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#include "ssd1306.h"
#include "font8x8_basic.h"

#include "oled_control.h"

#define tag "SSD1306"

int xMailbox_Idx = 0;
imu_data_t data;
//QueueHandle_t xMailbox;

//super cur sub disp_num 정의된 배열
//char ***menu_array= NULL;
// [][][2] = {
// {{-1, 1}, {-1, 2}, {-1, 3}, {-1, -1}},						//-1, 1, 2, 3, -1
// {{0, 4}, {0, -1}, {0, -1}, {0, -1}}, 						// 0, 처음만 4 나머지 -1
// {{0, 5}, {0, -1}, {0, -1}, {0, -1}, {0, -1}}, 				// 0, 처음만 5 나머지 -1
// {{0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}}, 		// 0, -1
// {{1, -1}, {1, -1}, {1, -1}}, 								// 1, -1
// {{2, -1}, {2, -1}, {2, -1}, {2, -1}}							// 2, -1
// };

int menu_num[]= {4, 3, 3, 3, 3, 4};
int cur_menu_num = 0;
int cur_menu_row = 0;

char* menu0_str[4] = {"ATT", "ACC", "GYR", "CTL"};
char* menu1_str[3] = {"ATT_X d", "ATT_Y d", "ATT_Z d"};
char* menu2_str[3] = {"Acc_X g", "Acc_Y g", "Acc_Z g"};
char* menu3_str[3] = {"Gyr_X d", "Gyr_Y d", "Gyr_Z d"};
char* menu4_str[3] = {"CNTRL_1", "CNTRL_2", "CNTRL_3"};
char* menu5_str[4] = {"211", "212", "213", "214"};

menu_direction_t **menus;
SSD1306_t dev;

char* get_cur_menu_str(){
	char* str = 0;
	if (cur_menu_num == 0)
		str = menu0_str[cur_menu_row];
	else if (cur_menu_num == 1)
		str = menu1_str[cur_menu_row];
	else if (cur_menu_num == 2)
		str = menu2_str[cur_menu_row];
	else if (cur_menu_num == 3)
		str = menu3_str[cur_menu_row];
	else if (cur_menu_num == 4)
		str = menu4_str[cur_menu_row];
	else if (cur_menu_num == 5)
		str = menu5_str[cur_menu_row];
	return str;
}

void modifiedMenu(char menu_str[17], int row){
	char* str = NULL;
	char mstr[14]={0,};

	if (cur_menu_num == 0)
		str = menu0_str[row];
	else if (cur_menu_num == 1)
		str = menu1_str[row];
	else if (cur_menu_num == 2)
		str = menu2_str[row];
	else if (cur_menu_num == 3)
		str = menu3_str[row];
	else if (cur_menu_num == 4)
		str = menu4_str[row];
	else if (cur_menu_num == 5)
		str = menu5_str[row];

	if (menus[cur_menu_num][row].sup_menu_num != -1)
		sprintf(mstr, "< %s", str);
	else
		strcpy(mstr, str);
	if (menus[cur_menu_num][row].sub_menu_num != -1)
		sprintf(menu_str, "%s >", mstr);
	else
		strcpy(menu_str, mstr);
}

void modifiedMenu_from_string(char menu_str[17], char* str){
	char mstr[10]={0,};

	if (menus[cur_menu_num][cur_menu_row].sup_menu_num != -1)
		sprintf(mstr, "< %s", str);
	else
		strcpy(mstr, str);
	if (menus[cur_menu_num][cur_menu_row].sub_menu_num != -1)
		sprintf(menu_str, "%s >", mstr);
	else
		strcpy(menu_str, mstr);
}

void drawMenus(){
	int i;
	for(i=0; i<menu_num[cur_menu_num]; i++){
		char menu_str[17]={0,};
		if (i== cur_menu_row){
			ssd1306_clear_line(&dev, i+1, true);			
			modifiedMenu(menu_str, i);
			ssd1306_display_text(&dev, i+1, menu_str, strlen(menu_str), true);
			
		}
		else {
			modifiedMenu(menu_str, i);
			ssd1306_display_text(&dev, i+1, menu_str, strlen(menu_str), false);
		}
	}
}

void drawMenusfromString(char x[10], char y[10], char z[10] ){
	int i;
	for(i=0; i<menu_num[cur_menu_num]; i++){
		char menu_str[17]={0,};
		if (i== cur_menu_row){
			ssd1306_clear_line(&dev, i+1, true);
			if (i==0) strcpy(menu_str, x);
			else if (i==1) strcpy(menu_str, y);
			else if (i==2) strcpy(menu_str, z);
			ssd1306_display_text(&dev, i+1, menu_str, strlen(menu_str), true);
			
		}
		else {
			modifiedMenu(menu_str, i);
			if (i==0) strcpy(menu_str, x);
			else if (i==1) strcpy(menu_str, y);
			else if (i==2) strcpy(menu_str, z);
			ssd1306_display_text(&dev, i+1, menu_str, strlen(menu_str), false);
		}
	}
}

void printMenus(uart_console_key_type_t key){
	char* str;
	int temp;

	if ( (cur_menu_row == 0) && (key == KEY_UP) )
		ESP_LOGI("CONTROL", "[STATUS : %d, %d] INVALID KEY (UP)",cur_menu_num, cur_menu_row);
	else if ((cur_menu_row == (menu_num[cur_menu_num]-1)) && (key == KEY_DOWN) )
		ESP_LOGI("CONTROL", "[STATUS : %d, %d] INVALID KEY (DOWN)",cur_menu_num, cur_menu_row);
	else if ( (menus[cur_menu_num][cur_menu_row].sup_menu_num == -1) && (key == KEY_LEFT) )
		ESP_LOGI("CONTROL", "[STATUS : %d, %d] INVALID KEY (LEFT)",cur_menu_num, cur_menu_row);
	else if ( (menus[cur_menu_num][cur_menu_row].sub_menu_num == -1) && (key == KEY_RIGHT) )
		ESP_LOGI("CONTROL", "[STATUS : %d, %d] INVALID KEY (RIGHT)",cur_menu_num, cur_menu_row);
	else {		
		if (key == KEY_LEFT){
			xMailbox_Idx = 0;
			temp = menus[cur_menu_num][cur_menu_row].sup_menu_num;
			cur_menu_row = menus[cur_menu_num][cur_menu_row].sup_menu_row;
			cur_menu_num = temp;
			
			// 전체 clear screen 후 다시 메뉴그리기
			ssd1306_clear_screen(&dev, false);
			ssd1306_contrast(&dev, 0xff);
			ssd1306_display_text(&dev, 0, "[<]", 3, false);
			drawMenus();
		}
		else if (key == KEY_RIGHT){
			cur_menu_num = menus[cur_menu_num][cur_menu_row].sub_menu_num;
			cur_menu_row = 0;
			xMailbox_Idx = cur_menu_num;			
		}
		else if (key == KEY_UP){
			if (cur_menu_num==0){
				ssd1306_clear_line(&dev, 0, false);
				ssd1306_display_text(&dev, 0, "[^]", 3, false);
				// 현재 라인만 clear하고 normal text
				ssd1306_clear_line(&dev, cur_menu_row+1, false);
				str = get_cur_menu_str();
				char menu_str[17]={0,};
				modifiedMenu_from_string(menu_str, str);
				ssd1306_display_text(&dev, cur_menu_row+1, menu_str, strlen(menu_str), false);
				cur_menu_row--;
				// 변경된 라인 반전 clear & text
				ssd1306_clear_line(&dev, cur_menu_row+1, true);
				str = get_cur_menu_str();
				char menu_str2[17]={0,};
				modifiedMenu_from_string(menu_str2, str);
				ssd1306_display_text(&dev, cur_menu_row+1, menu_str2, strlen(menu_str2), true);
			}
			else
				cur_menu_row--;
		}
		else if (key == KEY_DOWN){
			if (cur_menu_num==0){
				ssd1306_clear_line(&dev, 0, false);
				ssd1306_display_text(&dev, 0, "[v]", 3, false);
				// 현재 라인만 clear하고 normal text
				ssd1306_clear_line(&dev, cur_menu_row+1, false);
				str = get_cur_menu_str();
				char menu_str[17]={0,};
				modifiedMenu_from_string(menu_str, str);
				ssd1306_display_text(&dev, cur_menu_row+1, menu_str, strlen(menu_str), false);
				cur_menu_row++;
				// 변경된 라인 반전 clear & text
				ssd1306_clear_line(&dev, cur_menu_row+1, true);
				str = get_cur_menu_str();
				char menu_str2[17]={0,};
				modifiedMenu_from_string(menu_str2, str);
				ssd1306_display_text(&dev, cur_menu_row+1, menu_str2, strlen(menu_str2), true);
			}
			else
				cur_menu_row++;
		}
		else if (key == KEY_SELECTED){
			str = get_cur_menu_str();
			char mstr[10]={0,};
			sprintf(mstr, "[%s S]", str);
			ssd1306_clear_line(&dev, 0, false);
			ssd1306_display_text(&dev, 0, mstr, strlen(mstr), false);
		}
	}
}

void console_uart_parser(uint8_t command){
    // ASCII code와 비교해서 처리
	uart_console_key_type_t key_t;
	switch (command){
		case 119:	//w
		case 87:	//W
			key_t = KEY_UP;
			break;
		case 97:	//a
		case 65:	//A
			key_t = KEY_LEFT;
			break;
		case 100:	//d
		case 68:	//D
			key_t = KEY_RIGHT;
			break;
		case 115:	//s
		case 83:	//S
			key_t = KEY_DOWN;
			break;
		case 122:	//z
		case 90:	//Z
			key_t = KEY_SELECTED;
			break;
		default:
			return;
			// do nothing
	}
	printMenus(key_t);
}

static void oled_display_task(void *pvParameters)
{
	TickType_t xLastWakeUpTime = xTaskGetTickCount();
	//imu_data_t data;
	char mstr_x[10]={0,};
	char mstr_y[10]={0,};
	char mstr_z[10]={0,};

    while(1) {
		if ((xMailbox_Idx>=1)&&(xMailbox_Idx<=3)){
			// 전체 clear screen 후 다시 메뉴그리기
			ssd1306_clear_screen(&dev, false);
			ssd1306_contrast(&dev, 0xff);
			ssd1306_display_text(&dev, 0, "[>]", 3, false);
			sprintf(mstr_x, "%+3.2f", data.x);
			sprintf(mstr_y, "%+3.2f", data.y);
			sprintf(mstr_z, "%+3.2f", data.z);
			drawMenusfromString(mstr_x, mstr_y, mstr_z);
		}
		vTaskDelayUntil(&xLastWakeUpTime, 500/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void oled_control_init(){
	int i, j;

	menus = (menu_direction_t**) malloc ( sizeof(menu_direction_t*) * MENU_NUM );
	for(i=0; i<MENU_NUM; i++){
		menus[i] = (menu_direction_t*) malloc ( sizeof(menu_direction_t) * menu_num[i] );
	}
	for(i=0; i<MENU_NUM; i++){
		for(j=0; j<menu_num[i]; j++){
			if (i==0) {
				menus[i][j].sup_menu_num = -1;
				menus[i][j].sup_menu_row = -1;
				if ((j>=0) && (j<=3)){					
					menus[i][j].sub_menu_num = j+1;
				}
				else
					menus[i][j].sub_menu_num = -1;
			}
			else if (i==1){
				menus[i][j].sup_menu_num = 0;
				menus[i][j].sup_menu_row = 0;
				// if (j==0){					
				// 	menus[i][j].sub_menu_num = 4;
				// }
				// else
					menus[i][j].sub_menu_num = -1;
			}
			else if (i==2){
				menus[i][j].sup_menu_num = 0;
				menus[i][j].sup_menu_row = 1;
				// if (j==0){					
				// 	menus[i][j].sub_menu_num = 5;
				// }
				// else
					menus[i][j].sub_menu_num = -1;
			}
			else if (i==3){
				menus[i][j].sup_menu_num = 0;
				menus[i][j].sup_menu_row = 2;
				menus[i][j].sub_menu_num = -1;
			}
			else if (i==4){
				menus[i][j].sup_menu_num = 0;
				menus[i][j].sup_menu_row = 3;
				menus[i][j].sub_menu_num = -1;
			}
			else if (i==5){
				menus[i][j].sup_menu_num = 0;
				menus[i][j].sup_menu_row = 4;
				menus[i][j].sub_menu_num = -1;
			}
			else
				ESP_LOGI("KEY_INIT", "Error Occurred!");
		}
	}

	//xMailbox = xQueueCreate(1, sizeof(imu_data_t));


#if CONFIG_SPI_INTERFACE
	ESP_LOGI(tag, "INTERFACE is SPI");
	ESP_LOGI(tag, "CONFIG_MOSI_GPIO=%d",CONFIG_MOSI_GPIO);
	ESP_LOGI(tag, "CONFIG_SCLK_GPIO=%d",CONFIG_SCLK_GPIO);
	ESP_LOGI(tag, "CONFIG_CS_GPIO=%d",CONFIG_CS_GPIO);
	ESP_LOGI(tag, "CONFIG_DC_GPIO=%d",CONFIG_DC_GPIO);
	ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_SPI_INTERFACE

#if CONFIG_SSD1306_128x64
	ESP_LOGI(tag, "Panel is 128x64");
	ssd1306_init(&dev, 128, 64);
    ESP_LOGI(tag, "Panel is initialized");
#endif // CONFIG_SSD1306_128x64

	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
    ESP_LOGI(tag, "clear screen & Contrast");

#if CONFIG_SSD1306_128x64
	ssd1306_display_text(&dev, 0, "[Initialized]", 13, false);
	drawMenus();
#endif // CONFIG_SSD1306_128x64

	xTaskCreate(oled_display_task, "oled_display_task", 2048, NULL, 8, NULL);
}