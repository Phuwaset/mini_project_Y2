// ============================================================
// Stepper Homing + Travel Measurement (MIN -> MAX)
// - Professional, student-friendly version (no emojis)
// - Prints steps and distance in mm
// Hardware assumptions:
//   * STEP/DIR driver
//   * Two limit switches: MIN and MAX
//   * Limit switches return LOW when NOT triggered, HIGH when triggered
//     (change LIMIT_ACTIVE_STATE if your wiring is different)
// ============================================================

// -------------------- Pin Configuration --------------------
#define DIR_PIN           19
#define STEP_PIN          18
#define LIMIT_SWITCH_MIN  35
#define LIMIT_SWITCH_MAX  34

// -------------------- Motion / Mechanics -------------------
#define STEPS_PER_REV     200      // steps/rev of the motor (1.8° => 200)
#define MICROSTEPPING     1        // microstep setting on the driver (1, 2, 4, 8, 16, ...)
#define TRAVEL_PER_REV_MM 8.0f     // mm of linear travel per 1 motor revolution

// -------------------- Timing (us) ---------------------------
unsigned long stepIntervalUs = 1600;  // time between steps (lower = faster)
unsigned long pulseWidthUs   = 20;    // HIGH pulse width on STEP pin

// -------------------- Limit Switch Logic --------------------
// If your switch is wired differently, adjust these two lines.
// Example: if your switch reads LOW when triggered -> set LIMIT_ACTIVE_STATE = LOW.
#define LIMIT_ACTIVE_STATE   HIGH
#define LIMIT_INACTIVE_STATE LOW

// -------------------- State Machine -------------------------
enum State { HOMING_TO_MIN, BACK_OFF_MIN, MOVE_TO_MAX, STOPPED };
State state = HOMING_TO_MIN;

// -------------------- Runtime Variables ---------------------
unsigned long prevTickUs = 0;     // for step timing
long  stepCount = 0;              // count steps from MIN toward MAX
long  backOffSteps = 200;         // steps to back off from MIN after homing

// Distance conversion
const float mmPerStep = TRAVEL_PER_REV_MM / (STEPS_PER_REV * MICROSTEPPING);

// ============================================================
// Setup
// ============================================================
void setup() {
  Serial.begin(9600);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN,  OUTPUT);

  // Note: change to INPUT_PULLUP if your wiring needs it
  pinMode(LIMIT_SWITCH_MIN, INPUT);
  pinMode(LIMIT_SWITCH_MAX, INPUT);

  // Start by moving toward MIN
  digitalWrite(DIR_PIN, HIGH);

  Serial.println("System start: homing to MIN...");
}

// ============================================================
// Main Loop
// ============================================================
void loop() {
  const unsigned long nowUs = micros();

  switch (state) {
    case HOMING_TO_MIN: homingToMin(nowUs);  break;
    case BACK_OFF_MIN:  backOffFromMin(nowUs); break;
    case MOVE_TO_MAX:   moveToMax(nowUs);    break;
    case STOPPED:       // Idle
      break;
  }
}

// ============================================================
// State: HOMING_TO_MIN
// Move toward MIN until MIN switch is triggered.
// ============================================================
void homingToMin(unsigned long nowUs) {
  if (!isLimitActive(LIMIT_SWITCH_MIN)) {
    doStep(nowUs);  // keep stepping toward MIN
  } else {
    Serial.println("Home reached at MIN.");
    // Reverse direction to move away from the MIN switch
    digitalWrite(DIR_PIN, LOW);
    state = BACK_OFF_MIN;
  }
}

// ============================================================
// State: BACK_OFF_MIN
// Back off a bit from the MIN switch so it is clearly released.
// ============================================================
void backOffFromMin(unsigned long nowUs) {
  if (backOffSteps > 0) {
    if (doStep(nowUs)) backOffSteps--;
  } else {
    Serial.println("Back-off complete.");
    stepCount = 0;                    // zero position at this point
    state = MOVE_TO_MAX;
    Serial.println("Moving toward MAX...");
  }
}

// ============================================================
// State: MOVE_TO_MAX
// Move toward MAX until MAX switch is triggered.
// Count steps to measure total travel in steps and mm.
// ============================================================
void moveToMax(unsigned long nowUs) {
  if (!isLimitActive(LIMIT_SWITCH_MAX)) {
    if (doStep(nowUs)) {
      stepCount++;
    }
  } else {
    const float distanceMM = stepCount * mmPerStep;
    Serial.print("MAX reached. Total steps = ");
    Serial.print(stepCount);
    Serial.print(", distance = ");
    Serial.print(distanceMM, 2);
    Serial.println(" mm");
    state = STOPPED;
  }
}

// ============================================================
// Step Pulse Generator (non-blocking)
// Returns true when one full step (HIGH->LOW) is completed.
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
// Helpers
// ============================================================
bool isLimitActive(uint8_t pin) {
  return (digitalRead(pin) == LIMIT_ACTIVE_STATE);
}
