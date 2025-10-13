String inputString = "";

// ขา Output จาก UNO ไป Mega
const int pinBKK = 4;
const int pinCHA = 5;
const int pinCBI = 6;
const int pinLPG = 7;

// timeout ป้องกันไฟติดค้างถ้า QR หายไป
unsigned long lastSeenBKK = 0;
unsigned long lastSeenCHA = 0;
unsigned long lastSeenCBI = 0;
unsigned long lastSeenLPG = 0;

const unsigned long holdTime = 500;  // ค้าง 0.5 วินาทีต่อการสแกน

void setup() {
  Serial.begin(9600);

  pinMode(pinBKK, OUTPUT);
  pinMode(pinCHA, OUTPUT);
  pinMode(pinCBI, OUTPUT);
  pinMode(pinLPG, OUTPUT);

  digitalWrite(pinBKK, LOW);
  digitalWrite(pinCHA, LOW);
  digitalWrite(pinCBI, LOW);
  digitalWrite(pinLPG, LOW);
}

void loop() {
  // ---------------- อ่าน Serial (จาก Python) ----------------
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      inputString.trim();
      Serial.print("Received: ");
      Serial.println(inputString);

      unsigned long now = millis();

      if (inputString == "BKK2025141902") {
        digitalWrite(pinBKK, HIGH);
        lastSeenBKK = now;
      } 
      else if (inputString == "CHA2025148531") {
        digitalWrite(pinCHA, HIGH);
        lastSeenCHA = now;
      } 
      else if (inputString == "CBI2025892031") {
        digitalWrite(pinCBI, HIGH);
        lastSeenCBI = now;
      } 
      else if (inputString == "LPG2025752031") {
        digitalWrite(pinLPG, HIGH);
        lastSeenLPG = now;
      }

      inputString = "";
    } else {
      inputString += c;
    }
  }

  // ---------------- ปล่อยสัญญาณกลับ LOW ถ้าไม่มีการสแกนซ้ำ ----------------
  unsigned long now = millis();

  if (digitalRead(pinBKK) == HIGH && (now - lastSeenBKK > holdTime)) {
    digitalWrite(pinBKK, LOW);
  }
  if (digitalRead(pinCHA) == HIGH && (now - lastSeenCHA > holdTime)) {
    digitalWrite(pinCHA, LOW);
  }
  if (digitalRead(pinCBI) == HIGH && (now - lastSeenCBI > holdTime)) {
    digitalWrite(pinCBI, LOW);
  }
  if (digitalRead(pinLPG) == HIGH && (now - lastSeenLPG > holdTime)) {
    digitalWrite(pinLPG, LOW);
  }
}
