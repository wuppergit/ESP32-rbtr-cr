#include "esp_camera.h"
#include <WiFi.h>
#include "soc/soc.h"             // disable brownout problems
#include "soc/rtc_cntl_reg.h"    // disable brownout problems
#include "esp_http_server.h"
#define ENA             12    // GPIO12 = Pin D12 -  Motor-Treiber A EnableA      
#define IN1             13    // GPIO13 = Pin D13 -  Motor-Treiber A IN1
#define IN2             15    // GPIO15 = Pin D15 -  Motor-Treiber A IN2
#define ENB             14    // GPIO14 = Pin D14 -  Motor-Treiber A EnableB
#define IN3             2     // GPIO02 = Pin D02 -  Motor-Treiber B IN3
#define IN4             16    // GPIO16 = Pin D16 -  Motor-Treiber B IN4
#define LEDC_KANAL_3    3     // Kanal 3; einer der 16-LEDC-Kanäle
#define LEDC_KANAL_4    4     // Kanal 4; einer der 16-LEDC-Kanäle
#define LEDC_AUFLOESUNG 10    // 10-Bit Tastverhältnisauflöung (0-1023)
#define LEDC_FREQ       30000 // 5.000 Hz Frequenz
// Funktions-Prototypen
void motorenStop ();          // Motorsteuerung
void rechts();
void links();
void vorwaerts();
void rueckwaerts();
// Definition von Router-Zugangsdaten (credentials)
const char* ssid     = "IhreSSID";
const char* password = "RouterPW";
//  fuer die Motorsteuerung
#define MINIMUMSPEED    723   // "Minimal-Geschindigkeit" damit der Mototr läuft
int motorSpeed = 1023;        // fuer Geschwindigkeit (1023 = Maximum)
int lenkSpeed  = 850;         // fuer Lenkbewegungen / Richtungswechsel
bool motorRun = 0;            // 1 = true = Motor an
// Motoren Fahrt und Richtung schalten
void motorenStop () {
  //Serial.println("stop");  
  digitalWrite(IN1, LOW);  
  digitalWrite(IN2, LOW);       
  digitalWrite(IN3, LOW);  
  digitalWrite(IN4, LOW);     
  motorRun = false;   
}
void vorwaerts () {
  //Serial.println("vor");  
  ledcWrite(LEDC_KANAL_3, motorSpeed);     
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);    
  ledcWrite(LEDC_KANAL_4, motorSpeed);    
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);       
  //Serial.print  ("motorSpeed ");        
  //Serial.println(motorSpeed);     
  motorRun = true;    
}
void rueckwaerts () {
  //Serial.println("rueck"); 
  ledcWrite(LEDC_KANAL_3, motorSpeed);     
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);  
  ledcWrite(LEDC_KANAL_4, motorSpeed);     
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);    
  //Serial.print  ("motorSpeed ");        
  //Serial.println(motorSpeed);    
  motorRun = true;      
}
void links () {
  //Serial.println("links"); 
  ledcWrite(LEDC_KANAL_3, lenkSpeed);   
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);     
  ledcWrite(LEDC_KANAL_4, lenkSpeed);   
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);  
  //Serial.print  ("lenkSpeed ");        
  //Serial.println(lenkSpeed);   
}
void rechts () {
  //Serial.println("rechts");       
  ledcWrite(LEDC_KANAL_3, lenkSpeed);     
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);     
  ledcWrite(LEDC_KANAL_4, lenkSpeed);     
  digitalWrite(IN3, LOW);
  //Serial.print  ("lenkSpeed ");        
  //Serial.println(lenkSpeed);   
}
//  fuer die Kamera                             
#define PART_BOUNDARY "123456789000000000000987654321"
#define CAMERA_MODEL_AI_THINKER
#if defined(CAMERA_MODEL_WROVER_KIT)
  #define PWDN_GPIO_NUM    -1
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    21
  #define SIOD_GPIO_NUM    26
  #define SIOC_GPIO_NUM    27  
  #define Y9_GPIO_NUM      35
  #define Y8_GPIO_NUM      34
  #define Y7_GPIO_NUM      39
  #define Y6_GPIO_NUM      36
  #define Y5_GPIO_NUM      19
  #define Y4_GPIO_NUM      18
  #define Y3_GPIO_NUM       5
  #define Y2_GPIO_NUM       4
  #define VSYNC_GPIO_NUM   25
  #define HREF_GPIO_NUM    23
  #define PCLK_GPIO_NUM    22
#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21
#elif defined(CAMERA_MODEL_M5STACK_WITHOUT_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       17
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21
#elif defined(CAMERA_MODEL_AI_THINKER)
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
#elif defined(CAMERA_MODEL_M5STACK_PSRAM_B)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     22
  #define SIOC_GPIO_NUM     23  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21
#else
  #error "Camera model not selected"
#endif
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";
httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<html>
  <head>
    <title>ESP32-CAM Robot</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body {font-family: Arial; text-align: center; 
            margin:0px auto; padding-top: 30px;}
      table {margin-left: auto; margin-right: auto;}
      td {padding: 8 px;}
      .button {
        background-color: #2f4468;
        border: none;
        color: white;
        width: 80px;                   /* Elementbreite 80px */        
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 18px;
        margin: 6px 3px;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }
      img {width: auto ;
        max-width: 100% ;
        height: auto ; 
      }
    </style>
  </head>
  <body>
    <h1>ESP32CAM Motorsteuerung</h1>
    <img src="" id="photo" >
    <table>
      <tr><td colspan="3" align="center">
            <button class="button" onmousedown="toggleCheckbox('vor');" 
              ontouchstart="toggleCheckbox('vor');">vor</button>
          </td>
      </tr>
      <tr><td align="center">
            <button class="button" onmousedown="toggleCheckbox('li');" 
              ontouchstart="toggleCheckbox('li');">links</button>
          </td>
          <td align="center">
             <button class="button" onmousedown="toggleCheckbox('stp');" 
              ontouchstart="toggleCheckbox('stp');">stop</button>         
          </td>
          <td align="center">
            <button class="button" onmousedown="toggleCheckbox('re');"
              ontouchstart="toggleCheckbox('re');">rechts</button>
          </td>
      </tr>
      <tr><td colspan="3" align="center">
            <button class="button" onmousedown="toggleCheckbox('rck');"
              ontouchstart="toggleCheckbox('rck');">rueck</button>
            </td>
      </tr>                   
    </table>
   <script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/action?go=" + x, true);
     xhr.send();
   }
   window.onload = document.getElementById("photo").src = window.location.href.slice(0, -1) + ":81/stream";
  </script>
  </body>
</html>
)rawliteral";
static esp_err_t index_handler(httpd_req_t *req){
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}
static esp_err_t stream_handler(httpd_req_t *req){
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];
  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK){
    return res;
  }
  while(true){
    fb = esp_camera_fb_get();
    if (!fb) {
      //Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if(fb->width > 400){
        if(fb->format != PIXFORMAT_JPEG){
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if(!jpeg_converted){
            //Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }
    if(res == ESP_OK){
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if(_jpg_buf){
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if(res != ESP_OK){
      break;
    }
    //Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
  } 
  return res;
}
static esp_err_t cmd_handler(httpd_req_t *req){
  char*  buf;
  size_t buf_len;
  char variable[32] = {0,};
    buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if(!buf){
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      if (httpd_query_key_value(buf, "go", variable, sizeof(variable)) == ESP_OK) {
      } else {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
      }
    } else {
      free(buf);
      httpd_resp_send_404(req);
      return ESP_FAIL;
    }
    free(buf);
  } else {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }
  sensor_t * s = esp_camera_sensor_get();
  int res = 0;
  if(!strcmp(variable, "vor")) {
    //vorwaerts();
    Serial.println("vor");
  }
  else if(!strcmp(variable, "li")) {
    //links();
    Serial.println("links");
  }
  else if(!strcmp(variable, "stp")) {
    //motorenStop ();
    Serial.println("stop");
  }
  else if(!strcmp(variable, "re")) {
    //rechts();
    Serial.println("rechts");
  }
  else if(!strcmp(variable, "rck")) {
    //rueckwaerts();
    Serial.println("rueck");
  }
  else {
    res = -1;
  }
  if(res){
    return httpd_resp_send_500(req);
  }
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
}
void startCameraServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };
  httpd_uri_t cmd_uri = {
    .uri       = "/action",
    .method    = HTTP_GET,
    .handler   = cmd_handler,
    .user_ctx  = NULL
  };
  httpd_uri_t stream_uri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &cmd_uri);
  }
  config.server_port += 1;
  config.ctrl_port += 1;
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  // Motor-Control-Pins als Output öffnen
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT); 
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);   
  pinMode(ENA, OUTPUT);
  pinMode(ENA, OUTPUT);
  // Motor-Control-Pins auf LOW setzen  
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);  
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);  
  ledcSetup(LEDC_KANAL_3, LEDC_FREQ, LEDC_AUFLOESUNG); 
  ledcAttachPin(ENA, LEDC_KANAL_3);
  ledcSetup(LEDC_KANAL_4, LEDC_FREQ, LEDC_AUFLOESUNG); 
  ledcAttachPin(ENB, LEDC_KANAL_4);
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
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    //Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }  
  // Wi-Fi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("Camera Stream Ready! Go to: http://");
  Serial.println(WiFi.localIP());  
  // Start streaming web server
  startCameraServer();
}
void loop() {  
}
