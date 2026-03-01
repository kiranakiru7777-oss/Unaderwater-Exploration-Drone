/*
=========================================================
Underwater Exploration Drone - Video System
Controller: ESP32-CAM (AI Thinker)
Function: Live MJPEG Streaming + Camera Control Panel
Author: Kirana Ap
=========================================================
*/

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// ================= WIFI CONFIG =================

const char* ssid = "Underwater_CAM";
const char* password = "12345678";

WebServer server(80);

// ================= CAMERA PIN CONFIG =================
// AI Thinker ESP32-CAM

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

// ================= STREAM HANDLER =================

void handleStream() {

  WiFiClient client = server.client();

  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (client.connected()) {

    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) continue;

    server.sendContent("--frame\r\n");
    server.sendContent("Content-Type: image/jpeg\r\n\r\n");
    client.write(fb->buf, fb->len);
    server.sendContent("\r\n");

    esp_camera_fb_return(fb);
  }
}

// ================= CONTROL PANEL =================

void handleRoot() {

  String page = "<html><head><title>Underwater ROV Camera</title></head><body>";
  page += "<h2>ESP32-CAM Live Stream</h2>";
  page += "<img src='/stream' width='640'><br><br>";

  page += "<h3>Camera Settings</h3>";
  page += "<a href='/res_low'><button>Low Resolution</button></a><br><br>";
  page += "<a href='/res_high'><button>High Resolution</button></a><br><br>";
  page += "<a href='/quality_low'><button>Low Quality</button></a><br><br>";
  page += "<a href='/quality_high'><button>High Quality</button></a><br><br>";
  page += "<a href='/restart'><button>Restart Camera</button></a>";

  page += "</body></html>";

  server.send(200, "text/html", page);
}

// ================= CAMERA SETTINGS =================

void setLowResolution() {
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);
  server.send(200, "text/plain", "Resolution set to QVGA");
}

void setHighResolution() {
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_VGA);
  server.send(200, "text/plain", "Resolution set to VGA");
}

void setLowQuality() {
  sensor_t * s = esp_camera_sensor_get();
  s->set_quality(s, 20);
  server.send(200, "text/plain", "Quality Reduced");
}

void setHighQuality() {
  sensor_t * s = esp_camera_sensor_get();
  s->set_quality(s, 10);
  server.send(200, "text/plain", "Quality Increased");
}

void restartCamera() {
  ESP.restart();
}

// ================= SETUP =================

void setup() {

  Serial.begin(115200);

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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 12;
  config.fb_count = 2;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return;
  }

  WiFi.softAP(ssid, password);

  Serial.println("================================");
  Serial.println("ESP32-CAM Ready");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("================================");

  server.on("/", handleRoot);
  server.on("/stream", HTTP_GET, handleStream);
  server.on("/res_low", setLowResolution);
  server.on("/res_high", setHighResolution);
  server.on("/quality_low", setLowQuality);
  server.on("/quality_high", setHighQuality);
  server.on("/restart", restartCamera);

  server.begin();
}

void loop() {
  server.handleClient();
}
