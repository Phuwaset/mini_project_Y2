// ตัวอย่างการควบคุม L298N ด้วย ESP32 + MQTT
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ===== WiFi Config =====
const char *ssid = "Phupha&Game_2.4G";
const char *password = "0982607298";

// ===== MQTT Config =====
const char *mqtt_server = "192.168.1.54";
const int mqtt_port = 1883;
const char *mqtt_topic = "car/control";

WiFiClient espClient;
PubSubClient client(espClient);

// ===== Motor Pin Config =====
const int ENA = 21; // PWM left
const int IN1 = 18;
const int IN2 = 19;

const int ENB = 27; // PWM right
const int IN3 = 26;
const int IN4 = 25;

// ===== PWM Settings =====
const int freq = 5000;
const int resolution = 8;

// ===== Motor Functions =====
void forward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  ledcWrite(0, speed);
  ledcWrite(1, speed);
}

void backward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(0, speed);
  ledcWrite(1, speed);
}

void leftTurn(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  ledcWrite(0, speed);
  ledcWrite(1, speed);
}

void rightTurn(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(0, speed);
  ledcWrite(1, speed);
}

void stopMotors() {
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ===== MQTT Callback =====
void callback(char *topic, byte *message, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)message[i];
  }
  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  // ตัวอย่าง: รับข้อความแล้วควบคุม
  if (msg == "forward") {
    forward(220);
  } else if (msg == "backward") {
    backward(220);
  } else if (msg == "left") {
    leftTurn(200);
  } else if (msg == "right") {
    rightTurn(200);
  } else if (msg == "stop") {
    stopMotors();
  }
}

// ===== WiFi Connect =====
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// ===== Reconnect MQTT =====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Car")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  ledcSetup(0, freq, resolution);
  ledcAttachPin(ENA, 0);
  ledcSetup(1, freq, resolution);
  ledcAttachPin(ENB, 1);

  stopMotors();

  // WiFi & MQTT
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
