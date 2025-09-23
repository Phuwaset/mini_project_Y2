// ============================================================
// Stepper One-Time Homing + Travel Test + Servo Pose1
// ============================================================

#include <ESP32Servo.h>

// -------------------- Pin Configuration --------------------
#define DIR_PIN           19
#define STEP_PIN          18
#define LIMIT_SWITCH_MIN  35
#define LIMIT_SWITCH_MAX  34

// Servo pins
#define SERVO1_PIN 25   // ฐานหมุน
#define SERVO2_PIN 33   // แขนล่าง
#define SERVO3_PIN 32   // แขนบน

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

// -------------------- Servo Configuration -------------------
Servo servo1, servo2, servo3;

// -------------------- Global Variables ----------------------
unsigned long prevTickUs = 0;
const float mmPerStep = TRAVEL_PER_REV_MM / (STEPS_PER_REV * MICROSTEPPING);

// ============================================================
// Setup
// ============================================================
void setup() {
  Serial.begin(9600);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN,  OUTPUT);
  pinMode(LIMIT_SWITCH_MIN, INPUT);
  pinMode(LIMIT_SWITCH_MAX, INPUT);

  // Attach servo pins
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);

  // =============== STEP 1: Homing to MIN ===================
  Serial.println("System start: homing to MIN...");
  digitalWrite(DIR_PIN, HIGH); // direction toward MIN
  while (!isLimitActive(LIMIT_SWITCH_MIN)) {
    doStep(micros());
  }
  Serial.println("Home reached at MIN.");

  // back off a bit (initial homing)
  long backOffSteps = 200;
  digitalWrite(DIR_PIN, LOW); // move away from MIN
  while (backOffSteps > 0) {
    if (doStep(micros())) backOffSteps--;
  }
  Serial.println("Back-off complete.");

  // =============== STEP 2: Move to MAX =====================
  long stepCount = 0;
  digitalWrite(DIR_PIN, LOW); // direction toward MAX
  while (!isLimitActive(LIMIT_SWITCH_MAX)) {
    if (doStep(micros())) stepCount++;
  }
  float distanceMM = stepCount * mmPerStep;
  Serial.print("MAX reached. Steps = ");
  Serial.print(stepCount);
  Serial.print(", Distance = ");
  Serial.print(distanceMM, 2);
  Serial.println(" mm");

  // =============== STEP 3: Move back to MIN ================
  digitalWrite(DIR_PIN, HIGH); // direction toward MIN
  while (!isLimitActive(LIMIT_SWITCH_MIN)) {
    doStep(micros());
  }
  Serial.println("Returned to MIN. Test cycle complete.");

  // =============== STEP 4: Back off from MIN (final) ========
  long backOffAfterCycleSteps = 200;  // กำหนดจำนวน step ที่ต้องการถอยออก (เช่น 100 step)
  digitalWrite(DIR_PIN, LOW); // move toward MAX (ออกจาก MIN)
  while (backOffAfterCycleSteps > 0) {
    if (doStep(micros())) backOffAfterCycleSteps--;
  }
  Serial.println("Final back-off from MIN complete.");

  // =============== STEP 5: Servo Pose1 =====================
  pose1();
}

// ============================================================
// Loop (empty)
// ============================================================
void loop() {
  // Nothing, test done in setup
}

// ============================================================
// Servo Pose1 (Home position)
// ============================================================
void pose1() {
  servo1.write(180);   // base to 180
  servo2.write(90);    // lower arm to 90
  servo3.write(1);     // upper arm to 0
  Serial.println("Pose1 set: (180, 90, 0)");
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
      return true; // one complete step done
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
