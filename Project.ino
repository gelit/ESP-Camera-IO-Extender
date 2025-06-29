// 29 juin : correct L23 & L95

#include "esp_camera.h"
#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
#include "camera_pins.h"

#include "tft_setup.h"  from https://www.makerguides.com/interface-tft-st7735-display-with-esp32/
#include"TFT_eSPI.h"
TFT_eSPI tft = TFT_eSPI();

#include <Wire.h>           // I2C library
#include <SparkFunSX1509.h> // http://librarymanager/All#SparkFun_SX1509
#define I2C_SDA 5
#define I2C_SCL 6
SX1509 io;

int N = 0;           // Counter for LED
unsigned long Time;
bool Switch = 0;

void setup() {

  Serial.begin(115200);
  Serial.println("Begin of Setup");

  Wire.begin(I2C_SDA, I2C_SCL);
  if (io.begin(0x3E) == false)
  {
    Serial.println("Failed to communicate. Check wiring and address of SX1509.");
    while (1);
  }
  io.pinMode(1,OUTPUT);

  pinMode(TFT_BL,OUTPUT); digitalWrite(TFT_BL,1); 

  pinMode(LED_BUILTIN,OUTPUT);

//--
tft.init();
  tft.fillScreen(TFT_BLACK);  
  tft.setRotation(1); 
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.setSwapBytes(false);  // 17 juin : IMPORTANT !!!

  tft.invertDisplay(true);  // 20 juin from https://rntlab.com/question/colors-inverted-on-cyb-esp32-display/

//--

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000; // 20000000;  https://github.com/espressif/esp32-camera/issues/488

  config.pixel_format = PIXFORMAT_RGB565; 
  config.grab_mode = CAMERA_GRAB_LATEST;
  
  config.frame_size = FRAMESIZE_QVGA;  // 320 x 240   ---  FRAMESIZE_96X96;
  config.fb_count = 2;

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  
  Serial.println("End of Setup");
}

void loop() {

  N++;
  if (N==10) { N=0; bool x=digitalRead(LED_BUILTIN); x=!x; digitalWrite(LED_BUILTIN,x); }
  
  camera_fb_t *fb = esp_camera_fb_get();  
  if(!fb) { Serial.println("Camera capture failed"); delay(1000); return; } 
  else {
    tft.pushImage(0, 0, 320, 240, (uint16_t*)fb->buf);
    esp_camera_fb_return(fb);
  }

  if (millis() > Time + 2000) {  // to avoid blocking Camera stream with delay(2000)
    Serial.print('.');
    Time = millis();
    if (Switch) { io.digitalWrite(1,0); Serial.print('0'); } else { io.digitalWrite(1,1); Serial.print('1'); }
    Switch = !Switch;
  }
  
}