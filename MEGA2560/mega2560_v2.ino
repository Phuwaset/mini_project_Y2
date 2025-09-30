#include <Servo.h>

// ============================================================
// -------------------- Button Pins --------------------
#define sw1_emergency 49  // Emergency Stop
#define sw2_BTN_RED 51    // Stop (Reset)
#define sw3_BTN_GREEN 50  // Start

// -------------------- Stepper Pins --------------------
#define DIR_PIN 46
#define STEP_PIN 47
#define LIMIT_SWITCH_MIN 52
#define LIMIT_SWITCH_MAX 53

// -------------------- Servo robot lead --------------------
#define SERVO1_PIN 36  // ฐานหมุน
#define SERVO2_PIN 38  // แขนล่าง
#define SERVO3_PIN 40  // แขนบน

// --------------------- Servo robot arm ---------------------
#define SERVO1RbotArm_PIN 10
#define SERVO2RbotArm_PIN 11
#define SERVO3RbotArm_PIN 12
#define SERVO4RbotArm_PIN 13

// -------------------- Vacuum --------------------
#define VACUUM_PIN 14
#define VACUUM_ON LOW
#define VACUUM_OFF HIGH

// ---------------- Relay Config TO PLC ----------------
#define RELAY1_PIN 18
#define RELAY2_PIN 19
#define RELAY3_PIN 20
#define RELAY4_PIN 21

// Output Relay Control
#define RELAY_Start 5
#define RELAY_Stop 6
#define RELAY_Emg 7

#define RELAY_ON LOW
#define RELAY_OFF HIGH

// ================== Input Relay Monitor (6 ตัว) : Robot Arm ==================
#define ARM_HOME 22    // จุด home ของ robot arm
#define ARM_POINT1 23  // จุดรอรับพัสดุ ของ robot arm
#define ARM_POINTA 24  // จุดใส่กล่องจุด A ของ robot arm
#define ARM_POINTB 25  // จุดใส่กล่องจุด B ของ robot arm
#define ARM_POINTC 26  // จุดใส่กล่องจุด C ของ robot arm
#define ARM_POINTD 27  // จุดใส่กล่องจุด D ของ robot arm

// ================== Input Relay Monitor (4 ตัว) : Robot Lead ==================
#define LEAD_HOME 28    // จุด home ของ robot lead
#define LEAD_POINTA 29  // จุด 'รอพัสดุจากรถ' ของ robot lead
#define LEAD_POINTB 30  // จุด 'หยิบจากรถ' ของ robot lead
#define LEAD_POINTC 31  // จุด 'วางพัสดุบนแท่น' ของ robot lead

// ===== MEGA2560 ===== (TX2=16, RX2=17)
#define TRIG_PIN 8
#define ECHO_PIN 9

// ---------------- Timer ----------------
unsigned long lastUltra = 0;
unsigned long lastRelayRead = 0;
const unsigned long ultraInterval = 1000;  // Ultrasonic ทุก 1 วิ
const unsigned long relayInterval = 1000;  // Relay ทุก 0.5 วิ

// -------------------- Motion / Mechanics -------------------
#define STEPS_PER_REV 200
#define MICROSTEPPING 1
#define TRAVEL_PER_REV_MM 8.0f

// -------------------- Timing (us) ---------------------------
unsigned long stepIntervalUs = 1600;
unsigned long pulseWidthUs = 20;

// -------------------- Limit Switch Logic --------------------
#define LIMIT_ACTIVE_STATE HIGH
#define LIMIT_INACTIVE_STATE LOW

// -------------------- Servo Objects -------------------
Servo servo1, servo2, servo3;
Servo servoRboArm1, servoRboArm2, servoRboArm3, servoRboArm4;

//========
unsigned long prevTickUs = 0;
long stepCount = 0;
long backOffSteps = 200;
const float mmPerStep = TRAVEL_PER_REV_MM / (STEPS_PER_REV * MICROSTEPPING);

// --- Target Distance (mm) ---
float targetDistanceMM = 100.0;
long targetSteps = targetDistanceMM / mmPerStep;

// timers
unsigned long waitStartMs = 0;
// -------------------- Current Angles -------------------
int currentAngle1 = 180;
int currentAngle2 = 90;
int currentAngle3 = 0;

int currentArm1 = 90;
int currentArm2 = 135;
int currentArm3 = 70;
int currentArm4 = 150;

// ---------------- State Machine ----------------
enum StepperState { IDLE,
                    HOMING,
                    BACKOFF,
                    WAIT,
                    MOVE,
                    Working,
                    Pick,
                    DONE };
StepperState state = IDLE;

// สำหรับจัดการทิศทางตอนเข้า state
bool homingDirSet = false;
bool backoffDirSet = false;

// ============================================================
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
const int inBKK = 32;  // รับสัญญาณจาก UNO → BKK
const int inCHA = 33;  // รับสัญญาณจาก UNO → CHA
const int inCBI = 34;  // รับสัญญาณจาก UNO → CBI
const int inLPG = 35;  // รับสัญญาณจาก UNO → LPG

// เวลาให้ Relay ทำงาน (ms)
const unsigned long relayOnTime = 1000;  // 1 วินาที

// เก็บเวลาที่ Relay ถูกเปิดล่าสุด
unsigned long lastBKK = 0;
unsigned long lastCHA = 0;
unsigned long lastCBI = 0;
unsigned long lastLPG = 0;

// เก็บสถานะก่อนหน้า
bool lastEmg = HIGH;
bool lastStart = HIGH;
bool lastStop = HIGH;

bool activeBKK = false;
bool activeCHA = false;
bool activeCBI = false;
bool activeLPG = false;

// เก็บค่า state ก่อนหน้า (สำหรับ edge detection)
bool prevBKK = HIGH;
bool prevCHA = HIGH;
bool prevCBI = HIGH;
bool prevLPG = HIGH;


static int lastState[10] = { HIGH };    // เก็บค่าเดิม
static unsigned long lastDebounce[10];  // เวลา debounce
const unsigned long debounceDelay = 50;
bool isPressed(uint8_t pin) {
  return digitalRead(pin) == LOW;
}


// ============================================================
// ---------------- Setup ----------------
void setup() {
  Serial.begin(9600);

  pinMode(sw1_emergency, INPUT);
  pinMode(sw2_BTN_RED, INPUT);
  pinMode(sw3_BTN_GREEN, INPUT);

  // Stepper
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(LIMIT_SWITCH_MIN, INPUT);
  pinMode(LIMIT_SWITCH_MAX, INPUT);

  // Relay / Vacuum
  pinMode(VACUUM_PIN, OUTPUT);
  digitalWrite(VACUUM_PIN, VACUUM_OFF);

  pinMode(RELAY1_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, RELAY_OFF);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY2_PIN, RELAY_OFF);
  pinMode(RELAY3_PIN, OUTPUT);
  digitalWrite(RELAY3_PIN, RELAY_OFF);
  pinMode(RELAY4_PIN, OUTPUT);
  digitalWrite(RELAY4_PIN, RELAY_OFF);
  pinMode(RELAY_Start, OUTPUT);
  digitalWrite(RELAY_Start, RELAY_OFF);
  pinMode(RELAY_Stop, OUTPUT);
  digitalWrite(RELAY_Stop, RELAY_OFF);
  pinMode(RELAY_Emg, OUTPUT);
  digitalWrite(RELAY_Emg, RELAY_OFF);

  pinMode(ARM_HOME, INPUT_PULLUP);
  pinMode(ARM_POINT1, INPUT_PULLUP);
  pinMode(ARM_POINTA, INPUT_PULLUP);
  pinMode(ARM_POINTB, INPUT_PULLUP);
  pinMode(ARM_POINTC, INPUT_PULLUP);
  pinMode(ARM_POINTD, INPUT_PULLUP);

  pinMode(LEAD_HOME, INPUT_PULLUP);
  pinMode(LEAD_POINTA, INPUT_PULLUP);
  pinMode(LEAD_POINTB, INPUT_PULLUP);
  pinMode(LEAD_POINTC, INPUT_PULLUP);

  // Servos attach
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);

  pose1_Robot_lead();

  servoRboArm1.attach(SERVO1RbotArm_PIN);
  servoRboArm2.attach(SERVO2RbotArm_PIN);
  servoRboArm3.attach(SERVO3RbotArm_PIN);
  servoRboArm4.attach(SERVO4RbotArm_PIN);

  pinMode(inBKK, INPUT_PULLUP);
  pinMode(inCHA, INPUT_PULLUP);
  pinMode(inCBI, INPUT_PULLUP);
  pinMode(inLPG, INPUT_PULLUP);

  Armhome();

  Serial.println("System start: Lead Scerw homing.");
}

// ============================================================
// ---------------- Loop ----------------
void loop() {
  checkButtons();
  checkSwitches();
  QRScan();
  unsigned long nowUs = micros();
  unsigned long nowMs = millis();
  
    if (digitalRead(sw2_BTN_RED) == 1 || digitalRead(sw1_emergency) == HIGH) {
      Serial.println("▶️ Stop/Emg moving to homing");
      digitalWrite(DIR_PIN,HIGH);  // ทิศทางไปข้างหน้า
      state = HOMING;
      if (state == HOMING){
        homingARM();
        pose1_Robot_lead();  
      }   
    }
  if (state == IDLE) {
    Serial.println("▶️ Start Homing...");
    digitalWrite(DIR_PIN, HIGH);  // เข้าหา MIN
    state = HOMING;
  }
  
  else if (state == HOMING) {
    homingAndBackoff(nowUs);
  } else if (state == WAIT) {
    // รอ Start และ LEAD_POINTA ยังไม่กด
    if (digitalRead(sw3_BTN_GREEN) == 1 && digitalRead(LEAD_POINTA) == HIGH) {
      Serial.println("▶️ Start moving to target distance...");
      digitalWrite(DIR_PIN, LOW);  // ทิศทางไปข้างหน้า
      stepCount = 0;
      state = MOVE;
    }
  } else if (state == MOVE) {
    moveToDistance(nowUs);
    if (stepCount >= targetSteps) {
      Serial.println("🎯 Target distance reached → Go Working");
      pose2_Robot_lead();
      state = Working;
    }
  } else if (state == Working) {
    Serial.println("➡️ Enter Working state");

    // Step B
    if (digitalRead(LEAD_POINTB) == LOW) {
      Serial.println("📍 LEAD_POINTB detected");
      pose2_001_Robot_lead();
      pose3_Robot_lead();
      state = 100;  // flag ชั่วคราว บอกว่าผ่าน B แล้ว
    }
  } else if (state == 100) {  // หลังจากทำ B เสร็จ
    if (digitalRead(LEAD_POINTC) == LOW) {
      Serial.println("📍 LEAD_POINTC detected");
      pose4_Robot_lead();
      pose1_Robot_lead();
      Serial.println("✅ Finished B→C");
      state = Pick;
    }
  } else if (state == Pick) {
    Serial.println("➡️ Enter Pick state");
  
    // ตรวจจับตลอดว่ามี QR ไหน active
    if (activeBKK) {
      runArmSequence();
      runArmpoint_A();
      activeBKK = false;  // รีเซ็ตเพื่อรอรอบใหม่
    }
    if (activeCHA) {
      runArmSequence();
      runArmpoint_B();
      activeCHA = false;
    }
    if (activeCBI) {
      runArmSequence();
      runArmpoint_C();
      activeCBI = false;
    }
    if (activeLPG) {
      runArmSequence();
      runArmpoint_D();
      activeLPG = false;
      
    }
  
    // รอ Arm กลับไปที่จุด Point1
    if (digitalRead(ARM_POINT1) == LOW) {
      Serial.println("📍 ARM_POINT1 detected → Homing ARM");
      homingARM();
      state = DONE;
      }
    }



  else if (state == DONE) {
    digitalWrite(DIR_PIN, HIGH);
    state = HOMING;
    Serial.println("🔄 Reset pressed → Start Homing...");


    if (digitalRead(sw2_BTN_RED) == 1) {
      Serial.println("🔄 Reset pressed → Start Homing...");
    }
  }
}






// ============================================================
// ---------------- Stepper Functions ----------------

// --- HOMING เข้าหา MIN ---
// --- HOMING + BACKOFF ---
void homingAndBackoff(unsigned long nowUs) {
  static bool homingPhase = true;  // true = กำลัง homing, false = กำลัง backoff

  if (homingPhase) {
    // ====== HOMING ======
    if (!isLimitActive(LIMIT_SWITCH_MIN)) {
      doStep(nowUs);  // ยิง step ต่อเนื่องจนกว่าจะชน
    } else {
      Serial.println("✅ Home reached at MIN.");
      stepCount = 0;
      homingDirSet = false;
      // ตั้งทิศทาง backoff
      digitalWrite(DIR_PIN, LOW);  // สมมุติ LOW = ออกจาก MIN
      backOffSteps = 200;          // จำนวนก้าวถอย
      homingPhase = false;         // ไป phase backoff
    }
  } else {
    // ====== BACKOFF ======
    if (backOffSteps > 0) {
      if (doStep(nowUs)) backOffSteps--;
    } else {
      Serial.println("↩️ Back-off complete.");
      stepCount = 0;
      waitStartMs = millis();
      backoffDirSet = false;
      homingPhase = true;  // reset phase สำหรับรอบหน้า
      state = WAIT;        // เสร็จ → เข้าสู่ WAIT
    }
  }
}


// --- MOVE วิ่งไปตำแหน่งเป้าหมาย ---
void moveToDistance(unsigned long nowUs) {
  if (stepCount < targetSteps) {
    if (doStep(nowUs)) stepCount++;
  } else {
    Serial.print("🎯 Target reached: ");
    Serial.print(stepCount);
    Serial.print(" steps (");
    Serial.print(stepCount * mmPerStep, 0);
    Serial.println(" mm)");
    state = DONE;
  }
}

// --- ยิง Step หนึ่งพัลส์ ---
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
      return true;  // นับ step ครบ 1 รอบ
    }
  }
  return false;
}

// --- อ่านสถานะ Limit ---
bool isLimitActive(uint8_t pin) {
  return (digitalRead(pin) == LIMIT_ACTIVE_STATE);
}

// ---------------- ฟังก์ชันท่า Servo Lead ----------------
void pose1_Robot_lead() {  // Home
  moveServoSmooth(servo1, currentAngle1, 180, 15);
  moveServoSmooth(servo2, currentAngle2, 95, 15);
  moveServoSmooth(servo3, currentAngle3, 0, 15);
  digitalWrite(VACUUM_PIN, VACUUM_OFF);  // ปิด Vacuum (ปล่อย)
  Serial.println("Pose 1 (Home): (180,95,0) + OFF VACCUM ");
}
void pose2_Robot_lead() {
  moveServoSmooth(servo1, currentAngle1, 180, 30);
  moveServoSmooth(servo2, currentAngle2, 105, 30);
  moveServoSmooth(servo3, currentAngle3, 0, 30);
  digitalWrite(VACUUM_PIN, VACUUM_OFF);  // ปิด Vacuum (ปล่อย)
  Serial.println("Pose 2: (180,105,0) + ON VACCUM ");
}
void pose3_Robot_lead() {
  moveServoSmooth(servo1, currentAngle1, 0, 30);
  moveServoSmooth(servo2, currentAngle2, 105, 30);
  moveServoSmooth(servo3, currentAngle3, 0, 30);
  digitalWrite(VACUUM_PIN, VACUUM_ON);  // เปิด Vacuum (ดูด)
  Serial.println("Pose 3: (0,105,0) + ON VACCUM ");
}
void pose4_Robot_lead() {
  Serial.println("Pose 4: (0,95,1) + OFF VACCUM");

  moveServoSmooth(servo1, currentAngle1, 0, 30);
  moveServoSmooth(servo2, currentAngle2, 95, 30);
  moveServoSmooth(servo3, currentAngle3, 1, 30);
  digitalWrite(VACUUM_PIN, VACUUM_OFF);  // ปิด Vacuum (ปล่อย)
  delay(1200);
}
void pose2_001_Robot_lead() {
  moveServoSmooth(servo1, currentAngle1, 180, 30);
  moveServoSmooth(servo2, currentAngle2, 80, 30);
  moveServoSmooth(servo3, currentAngle3, 5, 30);
  digitalWrite(VACUUM_PIN, VACUUM_ON);  // เปิด Vacuum (ดูด)
  delay(1000);
  moveServoSmooth(servo1, currentAngle1, 180, 30);
  moveServoSmooth(servo2, currentAngle2, 105, 30);
  moveServoSmooth(servo3, currentAngle3, 4, 30);

  Serial.println("Pose 2_001: (X,X,X) + ON VACCUM ");
}

// ---------------- ฟังก์ชันท่า Home ของ Robot Arm ----------------
bool pose_home_robotArm() {
  // Home angles
  int home1 = 90;
  int home2 = 135;
  int home3 = 70;
  int home4 = 120;

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
  return true;
}

// Robot ARM Pose
void runArmSequence() {
  Serial.println("🤖 Running Robot Arm Sequence...");

  // ===== Pose 1 =====
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 135, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 70, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 120, 15);
  Serial.println("Pose 1: (90,135,70,150)");
  delay(1000);

  // ===== Pose 2 =====
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 150, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 80, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 120, 15);
  Serial.println("Pose 2: (95,150,80,150)");
  delay(1000);

  // ===== Pose 3 =====
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 170, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 90, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 120, 15);
  Serial.println("Pose 3: (95,170,90,150)");
  delay(1000);

  // ===== Pose 4 =====
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 180, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 90, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 70, 15);
  Serial.println("Pose 4: (95,180,90,120)");
  delay(1000);

  // ===== Pose 5 =====
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 180, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 110, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 70, 15);
  Serial.println("Pose 5: (95,180,110,120)");
  delay(1000);

  // ===== Pose 6 =====
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 115, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 95, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 70, 15);
  Serial.println("Pose 1: (95,115,95,120)");
  delay(1000);


  Serial.println(" Robot Arm Sequence finished.");
}
void homingARM() {
  // ===== Pose 9 =====
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 135, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 70, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 120, 15);
  Serial.println("Pose 1: (90,135,70,150)");
  delay(500);
}
void runArmpoint_A() {
  // ===== Pose 7 ===== A
  moveServoSmooth(servoRboArm1, currentArm1, 25, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 180, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 70, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 120, 15);
  Serial.println("Pose A: (30,180,65,150)");
  delay(500);

  // ===== Pose 8 =====
  moveServoSmooth(servoRboArm1, currentArm1, 55, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 145, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 65, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 120, 15);
  Serial.println("Pose A_1: (30,155,65,150)");
  delay(1000);
}
void runArmpoint_B() {
  // ===== Pose B ===== A
  moveServoSmooth(servoRboArm1, currentArm1, 0, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 155, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 65, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 120, 15);
  Serial.println("Pose B: (30,180,65,150)");
  delay(1000);
}
void runArmpoint_C() {
  // ===== Pose B ===== A
  moveServoSmooth(servoRboArm1, currentArm1, 55, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 145, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 65, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 120, 15);
  Serial.println("Pose C: (55,145,65,150)");
  delay(1000);
}

void runArmpoint_D() {
  // ===== Pose B ===== A
  moveServoSmooth(servoRboArm1, currentArm1, 10, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 115, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 55, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 120, 15);
  Serial.println("Pose D: (10,115,55,150)");
  delay(1000);
}

void checkButtons() {
  // 1. Emergency
  if (digitalRead(sw1_emergency) == HIGH) {
    Serial.println("Emergency!");
    digitalWrite(RELAY_Emg, RELAY_ON);
    delay(300);  // กันกดซ้ำ
  }

  // 2. Start
  else if (digitalRead(sw3_BTN_GREEN) == HIGH) {
    Serial.println("Start!");
    digitalWrite(RELAY_Start, RELAY_ON);
    delay(300);  // กันกดซ้ำ
  }

  // 3. Stop/Reset
  else if (digitalRead(sw2_BTN_RED) == HIGH) {
    Serial.println("STOP!");
    digitalWrite(RELAY_Stop, RELAY_ON);
    delay(300);  // กันกดซ้ำ
  } else {
    digitalWrite(RELAY_Start, RELAY_OFF);
    digitalWrite(RELAY_Stop, RELAY_OFF);
    digitalWrite(RELAY_Emg, RELAY_OFF);
  }
}
void Armhome() {
  moveServoSmooth(servoRboArm1, currentArm1, 90, 15);
  moveServoSmooth(servoRboArm2, currentArm2, 135, 15);
  moveServoSmooth(servoRboArm3, currentArm3, 70, 15);
  moveServoSmooth(servoRboArm4, currentArm4, 120, 15);
}

void checkSwitches() {

  const char *names[] = {
    "ARM_HOME", "ARM_POINT1", "ARM_POINTA", "ARM_POINTB", "ARM_POINTC", "ARM_POINTD",
    "LEAD_HOME", "LEAD_POINTA", "LEAD_POINTB", "LEAD_POINTC"
  };

  for (int i = 22; i <= 31; i++) {
    int idx = i - 22;
    int reading = digitalRead(i);

    if (reading != lastState[idx]) {
      if (millis() - lastDebounce[idx] > debounceDelay) {
        if (reading == LOW) Serial.print(names[idx]), Serial.println(" ON");
        else Serial.print(names[idx]), Serial.println(" OFF");
        lastState[idx] = reading;
        lastDebounce[idx] = millis();
      }
    }
  }
}
void QRScan() {
  unsigned long now = millis();

  bool curBKK = digitalRead(inBKK);
  bool curCHA = digitalRead(inCHA);
  bool curCBI = digitalRead(inCBI);
  bool curLPG = digitalRead(inLPG);

  // ตรวจจับสัญญาณเปลี่ยน LOW → HIGH
  if (prevBKK == LOW && curBKK == HIGH) {
    Serial.println("➡️ Detected: BKK2025141902");
    digitalWrite(RELAY1_PIN, LOW);
    lastBKK = now;
    activeBKK = true;
  }
  if (prevCHA == LOW && curCHA == HIGH) {
    Serial.println("➡️ Detected: CHA2025148531");
    digitalWrite(RELAY2_PIN, LOW);
    lastCHA = now;
    activeCHA = true;
  }
  if (prevCBI == LOW && curCBI == HIGH) {
    Serial.println("➡️ Detected: CBI2025892031");
    digitalWrite(RELAY3_PIN, LOW);
    lastCBI = now;
    activeCBI = true;
  }
  if (prevLPG == LOW && curLPG == HIGH) {
    Serial.println("➡️ Detected: LPG2025752031");
    digitalWrite(RELAY4_PIN, LOW);
    lastLPG = now;
    activeLPG = true;
  }

  // ปิด Relay หลังครบเวลา
  if (activeBKK && now - lastBKK >= relayOnTime) {
    digitalWrite(RELAY1_PIN, HIGH);
    activeBKK = false;
  }
  if (activeCHA && now - lastCHA >= relayOnTime) {
    digitalWrite(RELAY2_PIN, HIGH);
    activeCHA = false;
  }
  if (activeCBI && now - lastCBI >= relayOnTime) {
    digitalWrite(RELAY3_PIN, HIGH);
    activeCBI = false;
  }
  if (activeLPG && now - lastLPG >= relayOnTime) {
    digitalWrite(RELAY4_PIN, HIGH);
    activeLPG = false;
  }

  // เก็บสถานะปัจจุบันไว้สำหรับรอบหน้า
  prevBKK = curBKK;
  prevCHA = curCHA;
  prevCBI = curCBI;
  prevLPG = curLPG;
}
