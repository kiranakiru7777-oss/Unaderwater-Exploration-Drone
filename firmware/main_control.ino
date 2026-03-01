/*
=========================================================
Underwater Exploration Drone (ROV)
Controller: ESP32
Motor Driver: L298N
Communication: WiFi (Access Point Mode)
Author: Kirana Ap
=========================================================
*/

#include <WiFi.h>
#include <WebServer.h>

// ================= WIFI CONFIG =================

const char* ssid = "Underwater_ROV";
const char* password = "12345678";

WebServer server(80);

// ================= PIN DEFINITIONS =================

// Horizontal Thrusters (via L298N)
#define FRONT_THRUSTER   26
#define BACK_THRUSTER    27
#define LEFT_THRUSTER    14
#define RIGHT_THRUSTER   12

// Vertical Thruster
#define VERTICAL_THRUSTER 25

// Ballast System
#define BALLAST_FILL     33
#define BALLAST_RELEASE  32

// LED
#define LED_LIGHT        4

// ================= FUNCTION DECLARATIONS =================

void handleRoot();
void stopAllThrusters();

void moveForward();
void moveReverse();
void turnLeft();
void turnRight();
void moveUp();
void ballastFill();
void ballastRelease();
void stopAll();
void lightOn();
void lightOff();

// ================= SETUP =================

void setup() {

  Serial.begin(115200);

  // Set all pins as OUTPUT
  pinMode(FRONT_THRUSTER, OUTPUT);
  pinMode(BACK_THRUSTER, OUTPUT);
  pinMode(LEFT_THRUSTER, OUTPUT);
  pinMode(RIGHT_THRUSTER, OUTPUT);
  pinMode(VERTICAL_THRUSTER, OUTPUT);
  pinMode(BALLAST_FILL, OUTPUT);
  pinMode(BALLAST_RELEASE, OUTPUT);
  pinMode(LED_LIGHT, OUTPUT);

  stopAllThrusters();

  // Start WiFi Access Point
  WiFi.softAP(ssid, password);
  Serial.println("================================");
  Serial.println("Underwater ROV Control System");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("================================");

  // Web routes
  server.on("/", handleRoot);
  server.on("/forward", moveForward);
  server.on("/reverse", moveReverse);
  server.on("/left", turnLeft);
  server.on("/right", turnRight);
  server.on("/up", moveUp);
  server.on("/sink", ballastFill);
  server.on("/surface", ballastRelease);
  server.on("/stop", stopAll);
  server.on("/light_on", lightOn);
  server.on("/light_off", lightOff);

  server.begin();
}

// ================= LOOP =================

void loop() {
  server.handleClient();
}

// ================= WEB INTERFACE =================

void handleRoot() {

  String page = "<html><head><title>Underwater ROV</title></head><body>";
  page += "<h2>Underwater Exploration Drone Control</h2>";
  page += "<a href='/forward'><button>Forward</button></a><br><br>";
  page += "<a href='/reverse'><button>Reverse</button></a><br><br>";
  page += "<a href='/left'><button>Left</button></a><br><br>";
  page += "<a href='/right'><button>Right</button></a><br><br>";
  page += "<a href='/up'><button>Move Up</button></a><br><br>";
  page += "<a href='/sink'><button>Sink (Ballast Fill)</button></a><br><br>";
  page += "<a href='/surface'><button>Surface</button></a><br><br>";
  page += "<a href='/stop'><button>STOP</button></a><br><br>";
  page += "<a href='/light_on'><button>Light ON</button></a><br><br>";
  page += "<a href='/light_off'><button>Light OFF</button></a>";
  page += "</body></html>";

  server.send(200, "text/html", page);
}

// ================= CONTROL LOGIC =================

void stopAllThrusters() {
  digitalWrite(FRONT_THRUSTER, LOW);
  digitalWrite(BACK_THRUSTER, LOW);
  digitalWrite(LEFT_THRUSTER, LOW);
  digitalWrite(RIGHT_THRUSTER, LOW);
  digitalWrite(VERTICAL_THRUSTER, LOW);
  digitalWrite(BALLAST_FILL, LOW);
  digitalWrite(BALLAST_RELEASE, LOW);
}

// ----- Horizontal Movement -----

void moveForward() {
  stopAllThrusters();
  digitalWrite(FRONT_THRUSTER, HIGH);
  digitalWrite(BACK_THRUSTER, HIGH);
  server.send(200, "text/plain", "Moving Forward");
}

void moveReverse() {
  stopAllThrusters();
  digitalWrite(FRONT_THRUSTER, LOW);
  digitalWrite(BACK_THRUSTER, LOW);
  server.send(200, "text/plain", "Moving Reverse");
}

void turnLeft() {
  stopAllThrusters();
  digitalWrite(LEFT_THRUSTER, HIGH);
  server.send(200, "text/plain", "Turning Left");
}

void turnRight() {
  stopAllThrusters();
  digitalWrite(RIGHT_THRUSTER, HIGH);
  server.send(200, "text/plain", "Turning Right");
}

// ----- Vertical Movement -----

void moveUp() {
  stopAllThrusters();
  digitalWrite(VERTICAL_THRUSTER, HIGH);
  server.send(200, "text/plain", "Moving Up");
}

// ----- Ballast Control -----

void ballastFill() {
  stopAllThrusters();
  digitalWrite(BALLAST_FILL, HIGH);
  server.send(200, "text/plain", "Ballast Filling - Sinking");
}

void ballastRelease() {
  stopAllThrusters();
  digitalWrite(BALLAST_RELEASE, HIGH);
  server.send(200, "text/plain", "Ballast Releasing - Surfacing");
}

// ----- Stop -----

void stopAll() {
  stopAllThrusters();
  server.send(200, "text/plain", "All Systems Stopped");
}

// ----- LED -----

void lightOn() {
  digitalWrite(LED_LIGHT, HIGH);
  server.send(200, "text/plain", "Light ON");
}

void lightOff() {
  digitalWrite(LED_LIGHT, LOW);
  server.send(200, "text/plain", "Light OFF");
}
