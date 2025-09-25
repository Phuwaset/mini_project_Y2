#include <ESP32Servo.h>   // ไลบรารีสำหรับควบคุม Servo บน ESP32

// ---------------- กำหนดขา GPIO ของ Servo ----------------
// ชุดที่ 1
#define SERVO1_PIN 25   // ฐานหมุน
#define SERVO2_PIN 33   // แขนล่าง
#define SERVO3_PIN 32   // แขนบน

// ชุด Robot Arm
#define SERVO1RbotArm_PIN 12
#define SERVO2RbotArm_PIN 14
#define SERVO3RbotArm_PIN 27
#define SERVO4RbotArm_PIN 26



// ---------------- สร้างออบเจ็กต์ Servo ----------------
Servo servo1;
Servo servo2;
Servo servo3;

// Object servo robot ARM
Servo servoRboArm1;  // ฐานหมุน
Servo servoRboArm2;  // แขนล่าง
Servo servoRboArm3;  // แขนบน
Servo servoRboArm4;  // Gripper

// เก็บมุมปัจจุบัน (เพื่อ smooth move) ของชุดแรก
int currentAngle1 = 180;
int currentAngle2 = 90;
int currentAngle3 = 0;

// เก็บมุมปัจจุบันของ robot arm
int currentArm1 = 95;
int currentArm2 = 60;
int currentArm3 = 90;
int currentArm4 = 150;

// ---------------- Smooth Move ----------------
void moveServoSmooth(Servo &servo, int &currentAngle, int targetAngle, int stepDelay) {
  if (currentAngle < targetAngle) {
    for (int angle = currentAngle; angle <= targetAngle; angle++) {
      servo.write(angle);
      delay(stepDelay);
    }
  } else {
    for (int angle = currentAngle; angle >= targetAngle; angle--) {
      servo.write(angle);
      delay(stepDelay);
    }
  }
  currentAngle = targetAngle; 
}

// ---------------- ฟังก์ชันตั้งท่าของชุดแรก ----------------
void pose1() {  // Home
  moveServoSmooth(servo1, currentAngle1, 180, 15);
  moveServoSmooth(servo2, currentAngle2, 95, 15);
  moveServoSmooth(servo3, currentAngle3, 0, 15);
  digitalWrite(Vuccum, 1);   

  Serial.println("Pose 1 (Home): (180,95,0) + OFF VACCUM ");
}

void pose2() {  // ท่าถัดไป
  moveServoSmooth(servo1, currentAngle1, 180, 30);
  moveServoSmooth(servo2, currentAngle2, 105, 30);
  moveServoSmooth(servo3, currentAngle3, 0, 30);
  digitalWrite(Vuccum, 0);
  Serial.println("Pose 2: (180,105,0) + ON VACCUM ");
}

void pose3() {  // ท่าถัดไป + เปิด vacuum
  moveServoSmooth(servo1, currentAngle1, 0, 30);
  moveServoSmooth(servo2, currentAngle2, 105, 30);
  moveServoSmooth(servo3, currentAngle3, 0, 30);

  digitalWrite(Vuccum, 0);   
  Serial.println("Pose 3: (0,105,0) + ON VACCUM ");
}

void pose4() {  // ท่าถัดไป + เปิด vacuum
  moveServoSmooth(servo1, currentAngle1, 0, 30);
  moveServoSmooth(servo2, currentAngle2, 95, 30);
  moveServoSmooth(servo3, currentAngle3, 0, 30);

  digitalWrite(Vuccum, 1);   // high logic 1 == OFF
  Serial.println("Pose 3: (0,95,0) + OFF VACCUM");
}

// ---------------- ฟังก์ชันควบคุม Robot Arm ----------------

void setArmPosition(int a1, int a2, int a3, int a4, int stepDelay = 15) {
  int start1 = currentArm1;
  int start2 = currentArm2;
  int start3 = currentArm3;
  int start4 = currentArm4;

  int diff1 = a1 - start1;
  int diff2 = a2 - start2;
  int diff3 = a3 - start3;
  int diff4 = a4 - start4;

  // หาค่าสูงสุดของ step
  int maxStep = max(max(abs(diff1), abs(diff2)), max(abs(diff3), abs(diff4)));

  for (int i = 1; i <= maxStep; i++) {
    int angle1 = start1 + (diff1 * i) / maxStep;
    int angle2 = start2 + (diff2 * i) / maxStep;
    int angle3 = start3 + (diff3 * i) / maxStep;
    int angle4 = start4 + (diff4 * i) / maxStep;

    servoRboArm1.write(angle1);
    servoRboArm2.write(angle2);
    servoRboArm3.write(angle3);
    servoRboArm4.write(angle4);

    delay(stepDelay);
  }

  // อัพเดตตำแหน่งสุดท้าย
  currentArm1 = a1;
  currentArm2 = a2;
  currentArm3 = a3;
  currentArm4 = a4;

  Serial.printf("RobotArm Smooth Position: (%d, %d, %d, %d)\n", a1, a2, a3, a4);
}


void pose_home_robotArm() {
  setArmPosition(95, 60, 90, 150,20);   // ค่า Home ของหุ่นยนต์แขน
  Serial.println("Robot Arm Home Pose");
}

// ---------------- Setup ----------------
void setup() {
  Serial.begin(9600);

  // attach ชุดแรก
  servo1.attach(SERVO1_PIN, 500, 2400);
  servo2.attach(SERVO2_PIN, 500, 2400);
  servo3.attach(SERVO3_PIN, 500, 2400);

  // attach Robot Arm
  servoRboArm1.attach(SERVO1RbotArm_PIN, 500, 2400);
  servoRboArm2.attach(SERVO2RbotArm_PIN, 500, 2400);
  servoRboArm3.attach(SERVO3RbotArm_PIN, 500, 2400);
  servoRboArm4.attach(SERVO4RbotArm_PIN, 500, 2400);

  // ตั้งค่า vacuum
  pinMode(Vuccum, OUTPUT);
  digitalWrite(Vuccum, 1); // ปิด vacuum เริ่มต้น

  Serial.println("ESP32 Servo Arm Ready!");

  pose1();
  pose_home_robotArm();
  delay(2000);
}

// ---------------- Loop ----------------
void loop() {
//  pose1();
//  delay(1000);
//  pose2();
//  delay(1000);
//  pose3();
//  delay(1000);
//  pose4();
//  delay(1000);
  
//  setArmPosition(95, 60, 85, 150, 20);
//  delay(2000);
//  setArmPosition(105, 60, 80, 150, 20);
//  delay(1000);
//  setArmPosition(105, 155, 70, 150, 20);
//  delay(2000);
//  setArmPosition(105, 155, 70, 95, 20);
//  delay(3000);
//  setArmPosition(105, 100, 65, 95, 20);
//  delay(2000);
//  setArmPosition(30, 100, 65, 95, 20);
//  delay(2000);
//  setArmPosition(30, 80, 45, 95, 20);
//  delay(2000);
//  setArmPosition(30, 60, 80, 150, 20);
//  delay(1000);
//  setArmPosition(30, 130, 5, 150, 20);
//  delay(1000);
  

}
