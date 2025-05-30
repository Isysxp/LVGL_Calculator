#ifndef _DISPLAY_STUFF_H
#define _DISPLAY_STUFF_H

#include "src/ui/ui.h"
#define LGFX_USE_V1       // set to use new version of library
#include <LovyanGFX.hpp>  // main library
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include "Wire.h"
#include "driver/i2c.h"

#define TFT_HOR_RES   800
#define TFT_VER_RES   480

#define TOUCH_SDA 8
#define TOUCH_SCL 9
#define TOUCH_INT 4
#define TOUCH_RST -1

class LGFX : public lgfx::LGFX_Device {
public:
  lgfx::Bus_RGB _bus_instance;
  lgfx::Panel_RGB _panel_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_GT911 _touch_instance;

  LGFX(void) {
    {
      auto cfg = _panel_instance.config();
      cfg.memory_width = TFT_HOR_RES;
      cfg.memory_height = TFT_VER_RES;
      cfg.panel_width = TFT_HOR_RES;
      cfg.panel_height = TFT_VER_RES;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      _panel_instance.config(cfg);
    }

    {
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;

      cfg.pin_d0 = 14;   // B0
      cfg.pin_d1 = 38;   // B1
      cfg.pin_d2 = 18;   // B2
      cfg.pin_d3 = 17;   // B3
      cfg.pin_d4 = 10;   // B4

      cfg.pin_d5 = 39;   // G0
      cfg.pin_d6 = 0;    // G1
      cfg.pin_d7 = 45;   // G2
      cfg.pin_d8 = 48;   // G3
      cfg.pin_d9 = 47;   // G4
      cfg.pin_d10 = 21;  // G5

      cfg.pin_d11 = 1;  // R0
      cfg.pin_d12 = 2;  // R1
      cfg.pin_d13 = 42;  // R2
      cfg.pin_d14 = 41;  // R3
      cfg.pin_d15 = 40;  // R4

      cfg.pin_henable = 5;
      cfg.pin_vsync = 3;
      cfg.pin_hsync = 46;
      cfg.pin_pclk = 7;
      cfg.freq_write = 14000000;

      cfg.hsync_polarity = 0;
      cfg.hsync_front_porch = 20;
      cfg.hsync_pulse_width = 10;
      cfg.hsync_back_porch = 10;

      cfg.vsync_polarity = 0;
      cfg.vsync_front_porch = 10;
      cfg.vsync_pulse_width = 10;
      cfg.vsync_back_porch = 10;

      cfg.pclk_active_neg = 0;
      cfg.de_idle_high = 0;
      cfg.pclk_idle_high = 0;

      _bus_instance.config(cfg);
    }
    _panel_instance.setBus(&_bus_instance);

    // {
    //   auto cfg = _light_instance.config();
    //   cfg.pin_bl = GPIO_NUM_2;
    //   _light_instance.config(cfg);
    // }
    // _panel_instance.light(&_light_instance);

    {
      auto cfg = _touch_instance.config();
      cfg.x_min = 0;
      cfg.x_max = TFT_HOR_RES-1;
      cfg.y_min = 0;
      cfg.y_max = TFT_VER_RES-1;
      cfg.pin_int = TOUCH_INT;
      cfg.pin_rst = TOUCH_RST;
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;
      cfg.i2c_port = I2C_NUM_1;
      cfg.pin_sda = TOUCH_SDA;
      cfg.pin_scl = TOUCH_SCL;
      cfg.freq = 400000;
      cfg.i2c_addr = 0x14;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }
    setPanel(&_panel_instance);
  }
};


#include <lvgl.h>


LGFX tft;

#define screenWidth 800
#define screenHeight 480

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  tft.endWrite();
  lv_disp_flush_ready(disp);
}

void my_touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  uint16_t touchX, touchY;
  bool touched = tft.getTouch(&touchX, &touchY);
  if (!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;
  }
}


#endif	// _DISPLAY_STUFF_H

