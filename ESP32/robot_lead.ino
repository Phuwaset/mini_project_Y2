#include <ESP32Servo.h>   // ไลบรารีสำหรับควบคุม Servo บน ESP32

// ---------------- กำหนดขา GPIO ของ Servo ----------------
#define SERVO1_PIN 25   // ฐานหมุน
#define SERVO2_PIN 33   // แขนล่าง
#define SERVO3_PIN 32   // แขนบน

// ---------------- สร้างออบเจ็กต์ Servo ----------------
Servo servo1;
Servo servo2;
Servo servo3;

// ---------------- ฟังก์ชันตั้งท่า ----------------
void pose1() {
  servo1.write(180);   // ฐานไปที่ 180
  servo2.write(90);    // แขนล่างไปที่ 90
  servo3.write(0);     // แขนบนไปที่ 0
  Serial.println("Pose 1: (180,90,0)");
}

void pose2() {
  servo1.write(0);     // ฐานไปที่ 0
  servo2.write(90);   // แขนล่างไปที่ 180
  servo3.write(0);     // แขนบนไปที่ 0
  Serial.println("Pose 2: (0,180,0)");
}

// ---------------- Setup ----------------
void setup() {
  Serial.begin(9600);

  servo1.attach(SERVO1_PIN, 500, 2400);
  servo2.attach(SERVO2_PIN, 500, 2400);
  servo3.attach(SERVO3_PIN, 500, 2400);

  Serial.println("ESP32 Servo Arm Ready!");

  // เริ่มต้นที่ท่าแรก
  pose1();
  delay(2000);
}

// ---------------- Loop ----------------
void loop() {

}
