#include <Arduino.h>

// ===== Motor Pin Config =====
const int ENA = 21; // PWM left
const int IN1 = 18;
const int IN2 = 19;

const int ENB = 27; // PWM right
const int IN3 = 26;
const int IN4 = 25;

// ===== Motor Functions =====
void forward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 255);  
  analogWrite(ENB, 255);
}

void backward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 255);  
  analogWrite(ENB, 255);
}

void leftTurn() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 255);  
  analogWrite(ENB, 255);
}

void rightTurn() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 255);  
  analogWrite(ENB, 255);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void setup() {
  Serial.begin(115200);

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  stopMotors();
  Serial.println("✅ ESP32 + L298N พร้อมทดสอบความเร็วเต็ม (255)");
}

void loop() {
  Serial.println("Forward");
  forward();
  delay(2000);

  Serial.println("Stop");
  stopMotors();
  delay(1000);

  Serial.println("Backward");
  backward();
  delay(2000);

  Serial.println("Stop");
  stopMotors();
  delay(1000);

  Serial.println("Left Turn");
  leftTurn();
  delay(2000);

  Serial.println("Stop");
  stopMotors();
  delay(1000);

  Serial.println("Right Turn");
  rightTurn();
  delay(2000);

  Serial.println("Stop");
  stopMotors();
  delay(2000);
}
