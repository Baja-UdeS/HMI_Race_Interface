#include "pins_config.h"
#include "LovyanGFX_Driver.h"

#include <Arduino.h>
#include <lvgl.h>
#include <SPI.h>

#include <stdbool.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#include <thread.hpp>

#include "ui.h"

/* Multi Threading */
using namespace freertos;

thread speed_update;
lv_timer_t * label_updates_timer;

/* Expand IO */
#include <TCA9534.h>
TCA9534 ioex;


/*Speed Calculation*/

#define RATIO  (11/125)
//rpm_wheel = rpm_cvt * ratio;
#define WHEEL_RADIUS_IN 11
#define WHEEL_RADIUS_M (WHEEL_RADIUS_IN * 0.0254)
#define PI 3.1415926535897932384626433832795
#define WHEEL_CIRCUMFERENCE_M (2 * PI * WHEEL_RADIUS_M)
//vitesse_ms = (rpm_wheel / 60) * circ;
//vitesse_kmh = vitesse_ms * 3.6;

uint8_t last_instruction = 0;

float RPM1 = 0.0;
float RPM2 = 0.0;
float speed = 0.0;
int buttons_state = 0;

char * speed_str = (char *)malloc(3 * sizeof(char));

LGFX gfx;

/* Change to your screen resolution */
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;
static lv_color_t *buf1;

//uint16_t touch_x, touch_y;

//  Display refresh
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  if (gfx.getStartCount() > 0) {
    gfx.endWrite();
  }
  gfx.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::rgb565_t *)&color_p->full);

  lv_disp_flush_ready(disp);  //	Tell lvgl that the refresh is complete
}

void label_callback(lv_timer_t * timer) {
  lv_label_set_text(ui_SPEED, speed_str);
}

//  Read touch
/*void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
  data->state = LV_INDEV_STATE_REL;// The state of data existence when releasing the finger
  bool touched = gfx.getTouch( &touch_x, &touch_y );
  if (touched)
  {
    data->state = LV_INDEV_STATE_PR;

    //  Set coordinates
    data->point.x = touch_x;
    data->point.y = touch_y;
  }
}*/

void setup()
{
  Serial.begin(115200); 

  //pinMode(19, OUTPUT);//uart1

  Wire.begin(15, 16);
  delay(50);

  ioex.attach(Wire);
  ioex.setDeviceAddress(0x18);
  ioex.config(1, TCA9534::Config::OUT);
  ioex.config(2, TCA9534::Config::OUT);

  /* Turn on backlight*/
  ioex.output(1, TCA9534::Level::H);

  delay(20);
  ioex.output(2, TCA9534::Level::H);
  delay(100);
  pinMode(1, INPUT);
  /*end*/
  pinMode(2, INPUT);

  // Init Display
  gfx.init();
  gfx.initDMA();
  gfx.startWrite();
  gfx.fillScreen(TFT_BLACK);

  lv_init();
  size_t buffer_size = sizeof(lv_color_t) * LCD_H_RES * LCD_V_RES;
  buf = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  buf1 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);

  lv_disp_draw_buf_init(&draw_buf, buf, buf1, LCD_H_RES * LCD_V_RES);

  // Initialize display
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  // Change the following lines to your display resolution
  disp_drv.hor_res = LCD_H_RES;
  disp_drv.ver_res = LCD_V_RES;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Initialize input device driver program
  /*static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
  */
  delay(100);
  gfx.fillScreen(TFT_BLACK);

  ui_init();

  label_updates_timer = lv_timer_create(label_callback, 500, NULL);

  speed_update = thread::create_affinity(1-thread::current().affinity(),[](void*){
    while(true) {
      
      speed = speed + 1;
      if (speed > 100) {
        speed = 0;
      }
      strcpy(speed_str, String((int)speed).c_str());
      delay(250);

    }
  },nullptr,1,2000);
  
  speed_update.start();
  //Serial.println( "Setup done" );
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay(1);
}
