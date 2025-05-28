
/*******************************************
	Board https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3
	Flash size 8Mb
	OPI PSRAM

	board manager : esp32 by ESpressif 3.1.1
	Libraries :
	- lvgl by kisvegador 8.4.0
	- LovyanGFX by lovyan03 1.2.7
*******************************************/

#include "displayStuff.h"
#include <string.h>
#include "src/ui/vars.h"
#include "src/ui/actions.h"
#include "src/ui/screens.h"
#include "src/ui/ui.h"
#include <stdio.h>
#include <math.h>

static float rslt = 0.0;
static int inptr = 0;
static char strslt[32];
extern float addsub();
extern char input[101];

void action_button_clicked(lv_event_t* e) {
	const char* label_text;
	float rslt;


	lv_event_code_t code = lv_event_get_code(e);    // Get the event code eg LV_EVENT_PRESSED
	lv_obj_t* obj = lv_event_get_target(e);         // Get a reference to the widget generating this event
	if (code == LV_EVENT_PRESSED) {
		uint32_t id = lv_btnmatrix_get_selected_btn(obj);   // Get the widget (keypad) id number 
		label_text = lv_btnmatrix_get_btn_text(obj, id);    // Get the text label inside the specific widget.
		if (strstr(input, "="))
			input[0] = 0;
		strcat(input, label_text);
		switch (*label_text) {
		case 'C':
			lv_textarea_set_text(objects.textarea1, "");      // Directly reference a specific widget (textarea1) see screens.h
			input[0] = strslt[0] = 0;                         // Clear input and result text
			return;
		case '+':
		case '-':
		case '/':
		case '*':
			if (*strslt) {                                    // If there is a result, set it as first operand
				strcpy(input, strslt);
				strcat(input, label_text);
			}
			break;
		case '=':
			rslt = addsub();
			sprintf(strslt, "%.7g", rslt);
			strcat(input, strslt);
			break;
		}
		lv_textarea_set_text(objects.textarea1, (const char*)&input);
	}
}


lv_disp_t* disp;
int count = 0;

void setup() {
	Serial.begin(115200);

	tft.begin();                  // Graphics init code for the ST7262+CHG422 touch screen. (see displaystuff.h as well)

	lv_init();
	lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.hor_res = screenWidth;
	disp_drv.ver_res = screenHeight;
	disp_drv.flush_cb = my_disp_flush;
	disp_drv.draw_buf = &draw_buf;
	disp = lv_disp_drv_register(&disp_drv);
	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_touch_read;
	lv_indev_drv_register(&indev_drv);

	// Set display rotation to any of
	// LV_DISP_ROT_NONE, LV_DISP_ROT_90, LV_DISP_ROT_180, LV_DISP_ROT_270
	//disp_drv.sw_rotate = 1;
	//disp_drv.rotated = LV_DISP_ROT_NONE;
	input[0] = strslt[0] = 0;                 // Clear input and result text
	ui_init();
}

void loop() {
	lv_timer_handler();
	ui_tick();
	delay(5);
}
