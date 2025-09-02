#include <WiFi.h>
#include <PubSubClient.h>

// กำหนดขา
const int in1Pin = 18;
const int in2Pin = 19;
const int in3Pin = 22;
const int in4Pin = 23;

// Wi-Fi config
const char* ssid = "Phupha&Game_2.4G";
const char* password = "0982607298";

// MQTT config
const char* mqtt_server = "192.168.1.48";  // IP ของ MQTT Broker (Node-RED)
const int mqtt_port = 1883;
const char* mqtt_topic = "car/control";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void callback(char* topic, byte* message, unsigned int length) {
  String msgStr;
  for (int i = 0; i < length; i++) {
    msgStr += (char)message[i];
  }

  int command = msgStr.toInt();  // แปลงเป็นตัวเลข

  Serial.println("====================");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload (number): ");
  Serial.println(command);

  switch (command) {
    case 1:  // Forward
      Serial.println("Direction: Forward");
      digitalWrite(in1Pin, LOW);
      digitalWrite(in2Pin, HIGH);
      digitalWrite(in3Pin, LOW);
      digitalWrite(in4Pin, HIGH);
      break;

    case 2:  // Backward
      Serial.println("Direction: Backward");
      digitalWrite(in1Pin, HIGH);
      digitalWrite(in2Pin, LOW);
      digitalWrite(in3Pin, HIGH);
      digitalWrite(in4Pin, LOW);
      break;

    case 3:  // Left
      Serial.println("Direction: Left");
      digitalWrite(in1Pin, LOW);
      digitalWrite(in2Pin, HIGH);
      digitalWrite(in3Pin, LOW);
      digitalWrite(in4Pin, LOW);
      break;

    case 4:  // Right
      Serial.println("Direction: Right");
      digitalWrite(in1Pin, LOW);
      digitalWrite(in2Pin, LOW);
      digitalWrite(in3Pin, LOW);
      digitalWrite(in4Pin, HIGH);
      break;

    case 0:  // Stop
      Serial.println("Direction: Stop");
      digitalWrite(in1Pin, LOW);
      digitalWrite(in2Pin, LOW);
      digitalWrite(in3Pin, LOW);
      digitalWrite(in4Pin, LOW);
      break;

    default:
      Serial.println("Unknown Command");
      break;
  }

  Serial.println("====================\n");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32CarClient")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);

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