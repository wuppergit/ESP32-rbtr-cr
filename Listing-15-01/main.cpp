#include "esp_camera.h"
#include "FS.h"
#include "SD_MMC.h"
// Pin-Definition von CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
int imageNumber = 1;
char fileNumber[6];
void initCamera() {
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
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;
  }
  Serial.println("initialisiere Kamera");
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("error 0x%x", err);
    while (true) {}
  }  
}
void initSDKarte() {
  Serial.println("initialisiere SD-Card");
  if (!SD_MMC.begin()) {
    Serial.println("SD-Card-Init fehlgeschlagen!");
    while (true) {}
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_MMC) {
    Serial.println("Typ: MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("Typ: SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("Typ: SDHC");
  } else if (cardType == CARD_NONE) {
    Serial.println("No SD-Card attached!");
    while (true) {}
  } else {
    Serial.println("Typ: UNKNOWN");
    while (true) {}
  }
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD-Card Größe: %lluMB\n", cardSize);
  Serial.printf("Speicherplatz gesamt : %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
  Serial.printf("Speicherplatz genutzt: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));  
}
void fotografieren () {
  camera_fb_t *frameBuffer = NULL;
  // Bild aufnehmen
  frameBuffer = esp_camera_fb_get();
  if (!frameBuffer) {
    Serial.println("fotografieren fehlgeschlagen");
    return;
  }
  // Bild speichern
  sprintf(fileNumber, "%05d", imageNumber++);
  String imageFilename = "/image"+String(fileNumber)+".jpg";
  fs::FS &fs = SD_MMC;
  File imgFile = fs.open(imageFilename.c_str(), FILE_WRITE);
  if (!imgFile) {
    Serial.println("Dateiöffnen im Schreibmodus fehlgeschlagen");
  } else {
    imgFile.write(frameBuffer->buf, frameBuffer->len);
    Serial.println("Bild gespeichert in Datei " + imageFilename);
  }
  imgFile.close();
  esp_camera_fb_return(frameBuffer);  
}
void setup(){
  Serial.begin(115200);
  initCamera();
  initSDKarte ();  
}
void loop() {
  fotografieren();
  delay(10000);
}