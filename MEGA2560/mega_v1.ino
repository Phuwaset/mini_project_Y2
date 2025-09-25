// ===== MEGA2560 ===== (TX2=16, RX2=17)
#define TRIG_PIN 9
#define ECHO_PIN 10

// Output Relay Control
#define R_Start 5
#define R_Stop 6
#define R_Emg 7

// ================== Input Relay Monitor (6 ตัว) : Robot Arm ==================
#define ARM_HOME     36  // จุด home ของ robot arm
#define ARM_POINT1   34  // จุดรอรับพัสดุ ของ robot arm
#define ARM_POINTA   32  // จุดใส่กล่องจุด A ของ robot arm
#define ARM_POINTB   30  // จุดใส่กล่องจุด B ของ robot arm
#define ARM_POINTC   28  // จุดใส่กล่องจุด C ของ robot arm
#define ARM_POINTD   26  // จุดใส่กล่องจุด D ของ robot arm

// ================== Input Relay Monitor (4 ตัว) : Robot Lead ==================
#define LEAD_HOME    24  // จุด home ของ robot lead
#define LEAD_POINTA  22  // จุด 'รอพัสดุจากรถ' ของ robot lead
#define LEAD_POINTB  33  // จุด 'หยิบจากรถ' ของ robot lead
#define LEAD_POINTC  31  // จุด 'วางพัสดุบนแท่น' ของ robot lead 

// ---------------- Timer ----------------
unsigned long lastUltra = 0;
unsigned long lastRelayRead = 0;
const unsigned long ultraInterval = 1000;   // Ultrasonic ทุก 1 วิ
const unsigned long relayInterval = 1000;    // Relay ทุก 0.5 วิ

void setup() {
  Serial.begin(9600);   // Monitor ผ่าน USB
  Serial2.begin(9600);  // UART2 (TX2=16, RX2=17)

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Relay control outputs
  pinMode(R_Start, OUTPUT);
  pinMode(R_Stop, OUTPUT);
  pinMode(R_Emg, OUTPUT);

  digitalWrite(R_Start, HIGH);
  digitalWrite(R_Stop, HIGH);
  digitalWrite(R_Emg, HIGH);

  // ตั้งค่า pinMode ของ Robot Arm
  pinMode(ARM_HOME,    INPUT_PULLUP);
  pinMode(ARM_POINT1,  INPUT_PULLUP);
  pinMode(ARM_POINTA,  INPUT_PULLUP);
  pinMode(ARM_POINTB,  INPUT_PULLUP);
  pinMode(ARM_POINTC,  INPUT_PULLUP);
  pinMode(ARM_POINTD,  INPUT_PULLUP);

  // ตั้งค่า pinMode ของ Robot Lead
  pinMode(LEAD_HOME,   INPUT_PULLUP);
  pinMode(LEAD_POINTA, INPUT_PULLUP);
  pinMode(LEAD_POINTB, INPUT_PULLUP);
  pinMode(LEAD_POINTC, INPUT_PULLUP);

  Serial.println("MEGA ready");
}

void loop() {
  unsigned long now = millis();

  // ---- อ่าน Ultrasonic ทุก 1 วิ ----
  if (now - lastUltra >= ultraInterval) {
    lastUltra = now;
    float distance = readUltrasonic();

    Serial2.print("DIST:");
    Serial2.println(distance);

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  // ---- อ่านรีเลย์ทุก 0.5 วิ ----
  if (now - lastRelayRead >= relayInterval) {
    lastRelayRead = now;
    readRelayStates();
  }

  // ---- อ่านข้อความจาก ESP32 ----
  if (Serial2.available()) {
    String msg = Serial2.readStringUntil('\n');
    msg.trim();

    if (msg == "STATE:EMG") {
      Serial.println("ESP32 Status: Emergency Button Pressed");
      pulsePin(R_Emg);
      Serial2.print("R_EMG");
    } 
    else if (msg == "STATE:GRE") {
      Serial.println("ESP32 Status: Green Button Pressed (START)");
      pulsePin(R_Start);
      Serial2.print("R_GRE");
    } 
    else if (msg == "STATE:RED") {
      Serial.println("ESP32 Status: Red Button Pressed (STOP/RESET)");
      pulsePin(R_Stop);
      Serial2.print("R_RED");
    } 
    else {
      Serial.print("ESP32 Status: ");
      Serial.println(msg);
      Serial2.println(" ");
    }
  }
}

// ---------------- ฟังก์ชันอ่าน Ultrasonic ----------------
float readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 20000);  // timeout 20ms
  return duration * 0.034 / 2;                     // cm
}

// ---------------- ฟังก์ชัน pulse output ----------------
void pulsePin(int pin) {
  digitalWrite(pin, LOW);   // active
  delay(200);               // กดค้างสั้น ๆ
  digitalWrite(pin, HIGH);  // ปล่อยกลับ
}

// ---------------- ฟังก์ชันอ่านสถานะรีเลย์ ----------------
void readRelayStates() {
  Serial.print("Robot Arm: ");
  Serial.print("HOME=");   Serial.print(digitalRead(ARM_HOME)    == LOW ? "1 " : "0 ");
  Serial.print("P1=");     Serial.print(digitalRead(ARM_POINT1)  == LOW ? "1 " : "0 ");
  Serial.print("A=");      Serial.print(digitalRead(ARM_POINTA)  == LOW ? "1 " : "0 ");
  Serial.print("B=");      Serial.print(digitalRead(ARM_POINTB)  == LOW ? "1 " : "0 ");
  Serial.print("C=");      Serial.print(digitalRead(ARM_POINTC)  == LOW ? "1 " : "0 ");
  Serial.print("D=");      Serial.print(digitalRead(ARM_POINTD)  == LOW ? "1 " : "0 ");
  Serial.println();

  Serial.print("Robot Lead: ");
  Serial.print("HOME=");   Serial.print(digitalRead(LEAD_HOME)   == LOW ? "1 " : "0 ");
  Serial.print("A=");      Serial.print(digitalRead(LEAD_POINTA) == LOW ? "1 " : "0 ");
  Serial.print("B=");      Serial.print(digitalRead(LEAD_POINTB) == LOW ? "1 " : "0 ");
  Serial.print("C=");      Serial.print(digitalRead(LEAD_POINTC) == LOW ? "1 " : "0 ");
  Serial.println();
}
