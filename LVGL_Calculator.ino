
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
extern float addsub();
extern char input[101];

void action_button_clicked(lv_event_t *e) {
  const char *label_text;
  float rslt;
  char strslt[32];

  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  if (code == LV_EVENT_PRESSED) {
    uint32_t id = lv_btnmatrix_get_selected_btn(obj);
    label_text = lv_btnmatrix_get_btn_text(obj, id);
    if (strstr(input,"="))
      input[0]=0;
    if (*label_text == 'C') {
      lv_textarea_set_text(objects.textarea1, "");
      input[0]=0;
      return;
    }
    strcat(input,label_text);
    if (*label_text == '=') {
      rslt = addsub();
      sprintf(strslt, "%.7g", rslt);
      strcat(input, strslt);
    }
    lv_textarea_set_text(objects.textarea1, (const char *)&input);
  }
}


lv_disp_t *disp;
int count = 0;

void setup() {
  Serial.begin(115200);

  tft.begin();

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
  input[0] = 0;
  ui_init();
}

void loop() {
  lv_timer_handler();
  ui_tick();
  delay(5);
}
