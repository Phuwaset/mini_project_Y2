// ================================== ESP32 ============================== 
#include <ESP32Servo.h>

HardwareSerial U2(2); // UART2 RX=16, TX=17

// -------------------- Button Pins --------------------
#define sw1_emergency 21   // Emergency Stop
#define sw2_BTN_RED   23   // Stop (ใช้เป็น Reset ด้วย)
#define sw3_BTN_GREEN 22   // Start

// -------------------- Stepper Pins --------------------
#define DIR_PIN           19
#define STEP_PIN          18
#define LIMIT_SWITCH_MIN  35
#define LIMIT_SWITCH_MAX  34

// -------------------- Servo robot lead --------------------
#define SERVO1_PIN 25   // ฐานหมุน
#define SERVO2_PIN 33   // แขนล่าง
#define SERVO3_PIN 32   // แขนบน

// ---------------------Servo robot arm ---------------------
#define SERVO1RbotArm_PIN 12
#define SERVO2RbotArm_PIN 14
#define SERVO3RbotArm_PIN 27
#define SERVO4RbotArm_PIN 26

// Vacuum
#define Vuccum 13

// -------------------- Motion / Mechanics -------------------
#define STEPS_PER_REV     200
#define MICROSTEPPING     1
#define TRAVEL_PER_REV_MM 8.0f

// -------------------- Timing (us) ---------------------------
unsigned long stepIntervalUs = 1600;  
unsigned long pulseWidthUs   = 20;    

// -------------------- Limit Switch Logic --------------------
#define LIMIT_ACTIVE_STATE   HIGH
#define LIMIT_INACTIVE_STATE LOW

// -------------------- Servo Objects -------------------
Servo servo1, servo2, servo3;
Servo servoRboArm1, servoRboArm2, servoRboArm3, servoRboArm4;

// เก็บมุมปัจจุบัน (เพื่อ smooth move) ของชุดแรก
int currentAngle1 = 180;
int currentAngle2 = 90;
int currentAngle3 = 0;

// เก็บมุมปัจจุบันของ robot arm
int currentArm1 = 90;
int currentArm2 = 135;
int currentArm3 = 70;
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

// ---------------- ฟังก์ชันท่า Servo Lead ----------------
void pose1_Robot_lead() {  // Home
  moveServoSmooth(servo1, currentAngle1, 180, 15);
  moveServoSmooth(servo2, currentAngle2, 95, 15);
  moveServoSmooth(servo3, currentAngle3, 0, 15);
  digitalWrite(Vuccum, 1);   
  Serial.println("Pose 1 (Home): (180,95,0) + OFF VACCUM ");
}

void pose2_Robot_lead() {
  moveServoSmooth(servo1, currentAngle1, 180, 30);
  moveServoSmooth(servo2, currentAngle2, 105, 30);
  moveServoSmooth(servo3, currentAngle3, 0, 30);
  digitalWrite(Vuccum, 1);
  Serial.println("Pose 2: (180,105,0) + ON VACCUM ");
}

void pose3_Robot_lead() {
  moveServoSmooth(servo1, currentAngle1, 0, 30);
  moveServoSmooth(servo2, currentAngle2, 105, 30);
  moveServoSmooth(servo3, currentAngle3, 0, 30);
  digitalWrite(Vuccum, 0);   
  Serial.println("Pose 3: (0,105,0) + ON VACCUM ");
}

void pose4_Robot_lead() {
  moveServoSmooth(servo1, currentAngle1, 0, 30);
  moveServoSmooth(servo2, currentAngle2, 95, 30);
  moveServoSmooth(servo3, currentAngle3, 1, 30);
  digitalWrite(Vuccum, 1);   // high logic 1 == OFF
  Serial.println("Pose 4: (0,95,1) + OFF VACCUM");
}
void pose2_001_Robot_lead() {
  moveServoSmooth(servo1, currentAngle1, 180, 30);
  moveServoSmooth(servo2, currentAngle2, 88, 30);
  moveServoSmooth(servo3, currentAngle3, 2, 30);
  digitalWrite(Vuccum, 0);
  delay(1000);
  moveServoSmooth(servo1, currentAngle1, 180, 30);
  moveServoSmooth(servo2, currentAngle2, 105, 30);
  moveServoSmooth(servo3, currentAngle3, 5, 30);

  Serial.println("Pose 2_001: (X,X,X) + ON VACCUM ");
}
// ---------------- ฟังก์ชันท่า Home ของ Robot Arm ----------------
bool pose_home_robotArm() {
  // Home angles
  int home1 = 90;
  int home2 = 135;
  int home3 = 70;
  int home4 = 150;

  // Smooth Move ทีละตัว
  moveServoSmooth(servoRboArm1, currentArm1, home1, 15);
  moveServoSmooth(servoRboArm2, currentArm2, home2, 15);
  moveServoSmooth(servoRboArm3, currentArm3, home3, 15);
  moveServoSmooth(servoRboArm4, currentArm4, home4, 15);

  // update current
  currentArm1 = home1;
  currentArm2 = home2;
  currentArm3 = home3;
  currentArm4 = home4;

  Serial.println("🤖 Robot Arm Home Pose: (95,60,90,150)");
  return true;   // ✅ คืนค่า true เสมอ
}

// Robot ARM Pose

// ---------------- Robot Arm ตาม Sequence ----------------
void runArmSequence() {
  Serial.println("🤖 Running Robot Arm Sequence...");

  // ===== Pose 1 =====
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 135, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 70, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 150, 15);
  Serial.println("Pose 1: (90,135,70,150)");
  delay(2000);

  // ===== Pose 2 =====
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 180, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 90, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 150, 15);
  Serial.println("Pose 2: (90,180,90,150)");
  delay(2000);

  // ===== Pose 3 =====
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 180, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 90, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 110, 15);
  Serial.println("Pose 3: (90,180,90,110)");
  delay(2000);

  // ===== Pose 4 =====
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 120, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 90, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 110, 15);
  Serial.println("Pose 4: (90,120,90,110)");
  delay(2000);

  // ===== Pose 5 =====
  moveServoSmooth(servoRboArm1, currentArm1, 30, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 180, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 65, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 150, 15);
  Serial.println("Pose 5: (30,180,65,150)");
  delay(2000);

  // ===== Pose 6 =====
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 135, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 70, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 150, 15);
  Serial.println("Pose 1: (90,135,70,150)");
  delay(2000);
  
  Serial.println("✅ Robot Arm Sequence finished.");
}


// -------------------- Global Variables ----------------------
unsigned long prevTickUs = 0;
const float mmPerStep = TRAVEL_PER_REV_MM / (STEPS_PER_REV * MICROSTEPPING);

char inputBuffer[32];  
int bufferIndex = 0;
float latestDistance = 0.0;      
unsigned long lastReadTime = 0;  

// -------------------- System Flags ----------------------
bool emergencyActive = false;    
bool systemRunning   = false;    
int last_emg   = LOW;
int last_red   = LOW;
int last_green = LOW;

// -------------------- State Machine ----------------------
enum SystemState { IDLE, RUNNING, EMERGENCY };
SystemState sysState = IDLE;

//------------------- State Relay form Mega -----------------
bool R_EMG,R_RED,R_GRE = false;

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(9600);
  U2.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17

  // Button setup
  pinMode(sw1_emergency, INPUT);
  pinMode(sw2_BTN_RED,   INPUT);
  pinMode(sw3_BTN_GREEN, INPUT);

  // Stepper setup
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN,  OUTPUT);
  pinMode(LIMIT_SWITCH_MIN, INPUT);
  pinMode(LIMIT_SWITCH_MAX, INPUT);

  // Servo setup
  servo1.attach(SERVO1_PIN, 500, 2400);
  servo2.attach(SERVO2_PIN, 500, 2400);
  servo3.attach(SERVO3_PIN, 500, 2400);

  // attach Robot Arm
  servoRboArm1.attach(SERVO1RbotArm_PIN, 500, 2400);
  servoRboArm2.attach(SERVO2RbotArm_PIN, 500, 2400);
  servoRboArm3.attach(SERVO3RbotArm_PIN, 500, 2400);
  servoRboArm4.attach(SERVO4RbotArm_PIN, 500, 2400);

  // ตั้งค่า vacuum = OFF
  pinMode(Vuccum, OUTPUT);
  digitalWrite(Vuccum, HIGH); // OFF เริ่มต้น

  // กลับ Home เริ่มต้น
  pose_home_lead_screw(); 
  pose_home_robotArm(); 

  sysState = IDLE;   // เข้าสถานะว่าง
  Serial.println("System Ready. Waiting for START...");
  delay(50);
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  checkButtons();
  readSerialFromMega();

  if (sysState == EMERGENCY) {
    delay(20);
    return;
  }

  if (sysState == RUNNING) {
    // ตรวจระยะทุก 1 วิ
    if (millis() - lastReadTime >= 1000) {
      lastReadTime = millis();
      Serial.print("📏 Distance: ");
      Serial.print(latestDistance);
      Serial.println(" cm");

      if (latestDistance > 20.0 && latestDistance < 35) {
        Serial.println("   Object detected < 20 cm → Start sequence");

      // ===== SEQUENCE =====
        pose1_Robot_lead();
        delay(2000);
        pose2_Robot_lead();
        delay(2000);
        pose2_mm_robot_lead_screw(100.0);
        delay(2000);
        pose2_001_Robot_lead();
        delay(2000);
        pose3_Robot_lead();
        delay(2000);
        pose4_Robot_lead();
        delay(2000);
        digitalWrite(Vuccum, 1);

        //------
        pose1_Robot_lead();
        delay(2000);
        pose_home_lead_screw();
        delay(2000);
        runArmSequence();
        delay(1000);

        // จบงาน → ค้างรอ Reset
        sysState = IDLE;
        systemRunning = false;
        Serial.println("✅ Sequence finished. Waiting for RESET...");
      }
    }
  }
}

// ============================================================
// Check Buttons with Edge Detection + Debounce
// ============================================================
void checkButtons() {
  int emg = digitalRead(sw1_emergency);
  int red = digitalRead(sw2_BTN_RED);
  int green = digitalRead(sw3_BTN_GREEN);

  // Emergency Stop
  if (last_emg == LOW && emg == HIGH) {
    sysState = EMERGENCY;
    emergencyActive = true;
    systemRunning = false;
    U2.println("STATE:EMG");
    Serial.println("🚨 Emergency STOP! System halted.");
  }
  last_emg = emg;

  // Red Button = STOP or Reset Emergency
  if (last_red == LOW && red == HIGH) {
    Serial.println("⏹ STOP/RESET pressed...");
    U2.println("STATE:RED");

    // เคลียร์ Emergency ก่อน
    emergencyActive = false;
    sysState = IDLE;
    systemRunning = false;

    digitalWrite(Vuccum, HIGH); // ปิด Vacuum

    bool ok1 = pose_home_lead_screw();
    bool ok2 = pose_home_robotArm();

    if (ok1 && ok2) {
      Serial.println("✅ Reset done. Back to IDLE.");
    } else {
      Serial.println("❌ Reset failed! Check limit switch/servo.");
      sysState = EMERGENCY;
    }
  }
  last_red = red;

  // Green Button = START
  if (last_green == LOW && green == HIGH) {
        U2.println("STATE:GRE");

    if (!emergencyActive) {
      sysState = RUNNING;
      systemRunning = true;
      Serial.println("▶️ START pressed. System running.");
    } else {
      Serial.println("⚠️ Cannot START. Emergency Active!");
    }
  }
  last_green = green;
}


// ============================================================
// Read Serial from MEGA
// ============================================================
void readSerialFromMega() {
  static unsigned long lastReceiveTime = 0;
  const unsigned long timeout = 50;  // หน่วงเวลา 50 ms ถ้าไม่มี data เพิ่ม = จบข้อความ

  while (U2.available()) {
    char c = U2.read();

    // ข้าม '\r' ถ้ามี
    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      // กรณีใช้ println()
      inputBuffer[bufferIndex] = '\0';
      bufferIndex = 0;

      // ตรวจข้อความที่ได้รับ
      if (strncmp(inputBuffer, "DIST:", 5) == 0) {
        latestDistance = atof(inputBuffer + 5);
      }
      else if (strcmp(inputBuffer, "R_EMG") == 0) {
        Serial.println("Emergency command received!");
        R_EMG = true;
      }
      else if (strcmp(inputBuffer, "R_RED") == 0) {
        Serial.println("Start command received!");
        R_RED = true;
      }
      else if (strcmp(inputBuffer, "R_GRE") == 0) {
        Serial.println("Stop command received!");
        R_GRE = true;
      }
    }
    else {
      // เก็บตัวอักษร
      if (bufferIndex < sizeof(inputBuffer) - 1) {
        inputBuffer[bufferIndex++] = c;
      }
    }

    lastReceiveTime = millis();  // update เวลาที่อ่านล่าสุด
  }
}


// ============================================================
// Pose Home → Stepper กลับไป MIN + Servo กลับไป Pose1
// ============================================================
bool pose_home_lead_screw() {
  Serial.println("🏠 Returning Home...");

  unsigned long startTime = millis();
  digitalWrite(DIR_PIN, HIGH); 

  while (!isLimitActive(LIMIT_SWITCH_MIN)) {
    doStep(micros());

    // Timeout 5 วินาที ป้องกันค้าง
    if (millis() - startTime > 5000) {
      Serial.println("❌ Homing timeout! Limit switch MIN not triggered.");
      return false;
    }
  }

  Serial.println("Stepper homed at MIN.");
  pose1_Robot_lead();  
  Serial.println("  Home position reached (Stepper MIN + Servo Pose1).");

  return true;
}

// ============================================================
// Servo Pose2 + Stepper move by distance in mm
// ============================================================
void pose2_mm_robot_lead_screw(float distance_pose2) {
  static bool firstRun = true;       // flag สำหรับครั้งแรกเท่านั้น
  static long totalTravelSteps = 0;  // เก็บค่ารอบเต็ม

  if (firstRun) {
    Serial.println("🔄 First run: calibrating travel distance...");

    // เดินไป MAX เพื่อนับจำนวน step
    digitalWrite(DIR_PIN, LOW); 
    while (!isLimitActive(LIMIT_SWITCH_MAX)) {
      if (doStep(micros())) totalTravelSteps++;
    }
    Serial.print("Total travel steps to MAX: ");
    Serial.println(totalTravelSteps);

    // กลับ MIN เพื่อรีเซ็ต
    digitalWrite(DIR_PIN, HIGH);
    while (!isLimitActive(LIMIT_SWITCH_MIN)) {
      doStep(micros());
    }
    Serial.println("Returned to MIN after calibration.");

    firstRun = false; // ✅ ทำครั้งเดียว
  }

  // ---- วิ่งไปตามระยะที่กำหนด ----
  long targetSteps = (long)(distance_pose2 / mmPerStep);
  if (totalTravelSteps > 0 && targetSteps > totalTravelSteps) {
    targetSteps = totalTravelSteps;
  }

  Serial.print("Target steps for distance ");
  Serial.print(distance_pose2);
  Serial.print(" mm = ");
  Serial.println(targetSteps);

  digitalWrite(DIR_PIN, LOW); 
  while (targetSteps > 0) {
    if (doStep(micros())) targetSteps--;
  }
  Serial.println("Stepper reached target distance.");

}

// ============================================================
// Step Pulse Generator (non-blocking)
// ============================================================
bool doStep(unsigned long nowUs) {
  static bool pulseHigh = false;

  if (!pulseHigh) {
    if (nowUs - prevTickUs >= stepIntervalUs) {
      digitalWrite(STEP_PIN, HIGH);
      prevTickUs = nowUs;
      pulseHigh = true;
    }
  } else {
    if (nowUs - prevTickUs >= pulseWidthUs) {
      digitalWrite(STEP_PIN, LOW);
      pulseHigh = false;
      return true; 
    }
  }
  return false;
}

// ============================================================
// Helper: check limit switch
// ============================================================
bool isLimitActive(uint8_t pin) {
  return (digitalRead(pin) == LIMIT_ACTIVE_STATE);
}
