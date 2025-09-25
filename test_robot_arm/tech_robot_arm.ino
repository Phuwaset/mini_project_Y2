#include <ESP32Servo.h>

// ---------------- Robot Arm Pins ----------------
#define SERVO1RbotArm_PIN 12
#define SERVO2RbotArm_PIN 14
#define SERVO3RbotArm_PIN 27
#define SERVO4RbotArm_PIN 26

// ---------------- Servo Objects ----------------
Servo servoRboArm1;  // ฐานหมุน
Servo servoRboArm2;  // แขนล่าง
Servo servoRboArm3;  // แขนบน
Servo servoRboArm4;  // Gripper

// ---------------- Current Angles ----------------
int currentArm1 = 90;
int currentArm2 = 135;
int currentArm3 = 70;
int currentArm4 = 150;

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);

  servoRboArm1.attach(SERVO1RbotArm_PIN, 500, 2400);
  servoRboArm2.attach(SERVO2RbotArm_PIN, 500, 2400);
  servoRboArm3.attach(SERVO3RbotArm_PIN, 500, 2400);
  servoRboArm4.attach(SERVO4RbotArm_PIN, 500, 2400);  
  // ตั้งค่าเริ่มต้น
  servoRboArm1.write(currentArm1);
  servoRboArm2.write(currentArm2);
  servoRboArm3.write(currentArm3);
  servoRboArm4.write(currentArm4);  // ✅ ให้ Servo4 ไปที่ 150°

  Serial.println("Robot Arm Keyboard Control Ready!");
  Serial.println("q/a = Servo1 +5/-5");
  Serial.println("w/s = Servo2 +5/-5");
  Serial.println("e/d = Servo3 +5/-5");
  Serial.println("r/f = Servo4 +5/-5");
}

// ---------------- Loop ----------------
void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    switch (cmd) {
      case 'q': currentArm1 += 5; break;
      case 'a': currentArm1 -= 5; break;

      case 'w': currentArm2 += 5; break;
      case 's': currentArm2 -= 5; break;

      case 'e': currentArm3 += 5; break;
      case 'd': currentArm3 -= 5; break;

      case 'r': currentArm4 += 5; break;
      case 'f': currentArm4 -= 5; break;
    }

    // จำกัดองศาให้อยู่ในช่วง 0–180
    currentArm1 = constrain(currentArm1, 0, 160);
    currentArm2 = constrain(currentArm2, 30, 180);
    currentArm3 = constrain(currentArm3, 30, 160);
    currentArm4 = constrain(currentArm4, 30, 150);

    // ส่งค่าไปที่ servo
    servoRboArm1.write(currentArm1);
    servoRboArm2.write(currentArm2);
    servoRboArm3.write(currentArm3);
    servoRboArm4.write(currentArm4);

    Serial.printf("Servo1=%d  Servo2=%d  Servo3=%d  Servo4=%d\n",
                  currentArm1, currentArm2, currentArm3, currentArm4);
  }
}
