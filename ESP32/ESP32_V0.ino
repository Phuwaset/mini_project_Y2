// ===== ESP32 ===== RX=16, TX=17
HardwareSerial U2(2); // UART2

#define sw1_emergency 21   // Emergency Stop
#define sw2_BTN_RED   23   // Stop (ใช้เป็น Reset ด้วย)
#define sw3_BTN_GREEN 22   // Start

int last_emg   = LOW;
int last_red   = LOW;
int last_green = LOW;

unsigned long lastReadTime = 0;  // สำหรับหน่วงเวลา print ทุก 1 วิ
bool emergencyActive = false;    // flag Emergency
bool systemRunning   = false;    // flag การทำงานระบบ

String inputBuffer = "";
float latestDistance = 0.0;      // เก็บค่าระยะล่าสุดจาก Mega

void setup() {
  Serial.begin(9600);
  U2.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17

  pinMode(sw1_emergency, INPUT); // ปุ่ม Pull-down
  pinMode(sw2_BTN_RED,   INPUT);
  pinMode(sw3_BTN_GREEN, INPUT);

  Serial.println(" ESP32_Ready!! ");
}

void loop() {
  // ===== ตรวจจับ Emergency =====
  int emg = digitalRead(sw1_emergency);
  if (last_emg == LOW && emg == HIGH) {   // Rising Edge
    emergencyActive = true;
    systemRunning   = false;  // หยุดระบบด้วย
    Serial.println("🚨 Emergency STOP! System halted.");
  }
  last_emg = emg;

  // ===== ปุ่ม STOP (Stop + Reset Emergency) =====
  int red = digitalRead(sw2_BTN_RED);
  if (last_red == LOW && red == HIGH) {
    if (emergencyActive) {
      emergencyActive = false;
      Serial.println("✅ Emergency Reset, System can run again.");
    } else {
      systemRunning = false;
      Serial.println("⏹ STOP button pressed. System stopped.");
    }
  }
  last_red = red;

  // ===== ปุ่ม START =====
  int green = digitalRead(sw3_BTN_GREEN);
  if (last_green == LOW && green == HIGH) {
    if (!emergencyActive) {
      systemRunning = true;
      Serial.println("▶️ START button pressed. System running.");
    } else {
      Serial.println("⚠️ Cannot START. Emergency Active!");
    }
  }
  last_green = green;

  // ===== ถ้า Emergency Active → ไม่ทำงานอื่น =====
  if (emergencyActive) {
    delay(50);
    return;
  }

  // ===== อ่าน Serial จาก MEGA ตลอดเวลา =====
  while (U2.available()) {
    char c = U2.read();
    if (c == '\n') {
      inputBuffer.trim();
      if (inputBuffer.startsWith("DIST:")) {
        latestDistance = inputBuffer.substring(5).toFloat(); // เก็บค่าใหม่ล่าสุด
      }
      inputBuffer = ""; // เคลียร์ buffer
    } else {
      inputBuffer += c;
    }
  }

  // ===== แสดงผล Ultrasonic ทุก 1 วิ =====
  if (systemRunning && (millis() - lastReadTime >= 1000)) {
    lastReadTime = millis();

    Serial.print("📏 Distance: ");
    Serial.print(latestDistance);
    Serial.println(" cm");

    if (latestDistance < 20.0) {
      Serial.println("⚠️ Object detected < 20 cm");
    }
  }

  delay(50); 
}
// เวอร์ชั่น แก้ buffer overflow
// ===== ESP32 ===== RX=16, TX=17