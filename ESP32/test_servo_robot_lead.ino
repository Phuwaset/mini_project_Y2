#include <Servo.h>   // ไลบรารี Servo มาตรฐาน

#define SERVO1_PIN 2    // ฐานหมุน
#define SERVO2_PIN 3    // แขนล่าง
#define SERVO3_PIN 4    // แขนบน

Servo servo1, servo2, servo3;

// ฟังก์ชัน Smooth Move
void smoothMove(Servo &servo, int startAngle, int endAngle, int stepDelay) {
  if (startAngle < endAngle) {
    for (int pos = startAngle; pos <= endAngle; pos++) {
      servo.write(pos);
      delay(stepDelay);
    }
  } else {
    for (int pos = startAngle; pos >= endAngle; pos--) {
      servo.write(pos);
      delay(stepDelay);
    }
  }
}
_PIN 4    // แขนบน

Servo servo1, servo2, servo3;

// ฟังก์ชัน Smooth Move
void smoothMove(Servo &servo, int startAngle, int endAngle, int stepDelay) {
  if (startAngle < endAngle) {
    for (int pos = startAngle; pos <= endAngle; pos++) {
      servo.write(pos);
      delay(stepDelay);
    }
  } else {
    for (int pos = startAngle; pos >= endAngle; pos--) {
      servo.write(pos);
      delay(stepDelay);
    }
  }
}

void setup() {
  Serial.begin(115200);
void setup() {
  Serial.begin(115200);

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);

  Serial.println("เริ่ม Smooth Drive Servo (Pin 2,3,4)");
}

void loop() {
  // -------- Servo1 --------
  Serial.println("Servo1 smooth 0° → 180°");
  smoothMove(servo1, 0, 180, 15);
  delay(1000);
  Serial.println("Servo1 smooth 180° → 0°");
  smoothMove(servo1, 180, 0, 15);
  delay(1000);

  // -------- Servo2 --------
  Serial.println("Servo2 smooth 0° → 180°");
  smoothMove(servo2, 180, 30, 20);
  delay(1000);
  Serial.println("Servo2 smooth 180° → 0°");
  smoothMove(servo2, 30, 180, 20);
  delay(1000);

  // -------- Servo3 --------
  Serial.println("Servo3 smooth 0° → 180°");
  smoothMove(servo3, 0, 180, 10);
  delay(1000);
  Serial.println("Servo3 smooth 180° → 0°");
  smoothMove(servo3, 180, 0, 10);
  delay(1000);
}
