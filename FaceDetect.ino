#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "esp_camera.h"
#include "fd_forward.h"
#include "fb_gfx.h"
#include "fd_forward.h"
#include "fr_forward.h"
#include "BoardDefine.h"
#include "face_detect.h"

#include <TFT_eSPI.h>
#include <TFT_eFEX.h>
#include <SD.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eFEX fex = TFT_eFEX(&tft);

camera_fb_t *fb = NULL;

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    // setup TFT LCD
    tft.init();
    tft.setRotation(0);

    // config camera
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
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    //init with high specs to pre-allocate larger buffers
    if (psramFound())
    {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 13;
        config.fb_count = 1;
    }
    else
    {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    // init SD
    // SPI.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
    // if (!SD.begin(SD_CS))
    // {
    //     Serial.println("Card Mount Failed");
    //     return;
    // }
}

FaceDetect fd = FaceDetect();
void loop()
{
    uint8_t *buf = NULL;
    size_t len;

    fb = esp_camera_fb_get();
    buf = fb->buf;
    len = fb->len;
    if (fd.draw_face_boxes(fb))
    {
        buf = fb->buf;
        len = fb->len;
    }

    esp_camera_fb_return(fb);
    fex.drawJpg((const uint8_t *)buf, len, 0, 0);
    free(buf);
}