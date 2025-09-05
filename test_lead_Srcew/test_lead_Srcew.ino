// กำหนดขา Digital สำหรับ Stepper Motor Driver (TB6600)
#define DIR_PIN 25  // ขาสำหรับกำหนดทิศทางการหมุน (Direction) - เปลี่ยนเป็นขา 25
#define STEP_PIN 24 // ขาสำหรับส่งสัญญาณ Step (Pulse) - เปลี่ยนเป็นขา 24

// กำหนดขา Digital สำหรับ Limit Switch ทั้งสองตัว
// IMPORTANT: เนื่องจากต่อแบบ Pull-down, เมื่อสวิตช์ถูกกด ค่าจะเป็น HIGH
// เมื่อสวิตช์ไม่ถูกกด ค่าจะเป็น LOW
#define LIMIT_SWITCH_MIN 23 // ลิมิตสวิตช์ต้นทาง (Home Position)
#define LIMIT_SWITCH_MAX 22 // ลิมิตสวิตช์ปลายทาง

// กำหนดค่าสำหรับ Stepper Motor
#define STEPS_PER_REV 200     // จำนวน Step ต่อรอบของ Stepper Motor (สำหรับมอเตอร์ 1.8 องศา/step)
#define MICROSTEPPING 1       // ค่า Microstepping ที่ตั้งไว้บน Driver TB6600 (เช่น 1, 2, 4, 8, 16)
                              // ต้องตรงกับที่ตั้งค่าบน Driver เพื่อให้การคำนวณรอบถูกต้อง

void setup() {
  // เริ่มต้น Serial Communication ที่ Baud Rate 9600
  // ใช้สำหรับ Debugging และแสดงสถานะต่างๆ
  Serial.begin(9600);
  Serial.println("Limit Switch and Stepper Motor Test - Initializing...");

  // กำหนดโหมดของขาสำหรับ Limit Switch เป็น INPUT
  // เนื่องจากต่อแบบ Pull-down จึงใช้ INPUT ปกติ ไม่ต้องใช้ INPUT_PULLUP
  pinMode(LIMIT_SWITCH_MIN, INPUT);
  pinMode(LIMIT_SWITCH_MAX, INPUT);

  // กำหนดโหมดของขาสำหรับ Direction และ Step เป็น OUTPUT
  pinMode(STEP_PIN, OUTPUT); // PUL+ (ขา 24)
  pinMode(DIR_PIN, OUTPUT);  // DIR+ (ขา 25)

  // เรียกฟังก์ชัน Homing เพื่อให้ Stepper Motor กลับสู่ตำแหน่งเริ่มต้น
  homing();

  Serial.println("Limit Switch and Stepper Motor Test - Ready!");
}

void loop() {
  // อ่านค่าสถานะจาก Limit Switch
  // เนื่องจากต่อแบบ Pull-down: HIGH = กด/ตรวจจับ, LOW = ไม่กด/ไม่ตรวจจับ
  int minStatus = digitalRead(LIMIT_SWITCH_MIN);
  int maxStatus = digitalRead(LIMIT_SWITCH_MAX);

  // แสดงค่าสถานะของ Limit Switch บน Serial Monitor เพื่อ Debugging
  Serial.print("Limit Switch MIN (Home): ");
  Serial.print(minStatus == HIGH ? "PRESSED" : "RELEASED");
  Serial.print(" | Limit Switch MAX (End): ");
  Serial.println(maxStatus == HIGH ? "PRESSED" : "RELEASED");

  // ตัวอย่างการใช้งาน Stepper Motor และ Limit Switch ใน loop
  // คุณสามารถเพิ่ม Logic การควบคุม Stepper Motor ได้ที่นี่
  // เช่น หมุนไปข้างหน้าจนเจอ LIMIT_SWITCH_MAX แล้วหมุนกลับ
  if (digitalRead(LIMIT_SWITCH_MAX) == LOW) { // ถ้า LIMIT_SWITCH_MAX ไม่ถูกกด (พื้นที่ว่าง)
    Serial.println("Moving forward...");
    rotateMotor(LOW, 1); // หมุนไป 1 รอบ (ปรับทิศทางและจำนวนรอบได้)
  } else {
    Serial.println("LIMIT_SWITCH_MAX PRESSED! Stopping and Homing...");
    homing(); // กลับสู่ตำแหน่ง Home
    delay(2000); // หน่วงเวลาเล็กน้อยก่อนทำงานต่อ
  }

  delay(500); // หน่วงเวลาเล็กน้อยเพื่อไม่ให้ Serial Monitor รันเร็วเกินไป
}

// ฟังก์ชันสำหรับหมุน Stepper Motor
// รับค่า direction (HIGH/LOW) และ rounds (จำนวนรอบ)
void rotateMotor(bool direction, int rounds) {
  digitalWrite(DIR_PIN, direction); // กำหนดทิศทางการหมุน

  long totalSteps = (long)STEPS_PER_REV * MICROSTEPPING * rounds;

  for (long i = 0; i < totalSteps; i++) {
    // ตรวจสอบ Limit Switch MAX ระหว่างการหมุน
    // หาก Limit Switch MAX ถูกกด (HIGH) ให้หยุดมอเตอร์
    if (digitalRead(LIMIT_SWITCH_MAX) == HIGH) {
      Serial.println("🚨 LIMIT REACHED: MAX POSITION! Stopping motor.");
      return; // ออกจากฟังก์ชันทันที
    }
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(500); // ความเร็วในการหมุน (ปรับค่าได้)
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(500); // ความเร็วในการหมุน (ปรับค่าได้)
  }
  Serial.print("Rotated ");
  Serial.print(rounds);
  Serial.println(" rounds.");
}

// ฟังก์ชันสำหรับ Homing (กลับสู่ตำแหน่งเริ่มต้น) โดยใช้ LIMIT_SWITCH_MIN
void homing() {
  Serial.println("Homing...");
  digitalWrite(DIR_PIN, HIGH); // กำหนดทิศทางให้หมุนกลับไปหา LIMIT_SWITCH_MIN (ปรับตามการติดตั้งของคุณ)
  delay(100); // หน่วงเวลาเล็กน้อยเพื่อให้ Direction เปลี่ยน

  // หมุนมอเตอร์ไปเรื่อยๆ จนกว่า LIMIT_SWITCH_MIN จะถูกกด (HIGH)
  while (digitalRead(LIMIT_SWITCH_MIN) == LOW) { // ตราบใดที่ LIMIT_SWITCH_MIN ยังไม่ถูกกด
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(500); // ความเร็วในการหมุน homing
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(500);
  }

  Serial.println("✅ Home Position Reached! (LIMIT_SWITCH_MIN PRESSED)");

  // หลังจากเจอ Limit Switch MIN แล้ว อาจจะหมุนถอยหลังเล็กน้อยเพื่อออกจากสวิตช์
  // หรือหมุนไปตำแหน่งเริ่มต้นที่กำหนดไว้
  Serial.println("🔄 Moving away from Home Switch by 1 round...");
  digitalWrite(DIR_PIN, LOW); // เปลี่ยนทิศทาง (ปรับตามการติดตั้งของคุณ)
  delay(100);
  rotateMotor(LOW, 1); // หมุนกลับ 1 รอบเพื่อออกจากสวิตช์

  Serial.println("✅ Ready to Start!");
}
