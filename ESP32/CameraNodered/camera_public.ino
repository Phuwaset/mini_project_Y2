#include <WiFi.h>
#include <PubSubClient.h>

// ================= WiFi =================
const char* ssid = "Phupha&Game_2.4G";
const char* password = "0982607298";

// ================= MQTT =================
const char* mqtt_server = "192.168.1.54"; // IP Adress localhost.
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  if (String(topic) == "qr/LPG") {
     Serial.println("OUT/LPG");
  } 
  else if (String(topic) == "qr/CBI") {
     Serial.println("OUT/CBI");
  } 
  else if (String(topic) == "qr/BKK") {
     Serial.println("OUT/BKK");

  } 
  else if (String(topic) == "qr/CHA") {
     Serial.println("OUT/CHA");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");

      // Subscribe ทุก topic ที่เราต้องการ
      client.subscribe("qr/LPG");
      client.subscribe("qr/CBI");
      client.subscribe("qr/BKK");
      client.subscribe("qr/CHA");

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

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
