// ===== MEGA2560 ===== (TX2=16, RX2=17)
#define TRIG_PIN 9
#define ECHO_PIN 10

void setup() {
  Serial.begin(9600);     // Monitor ผ่าน USB
  Serial2.begin(9600);    // UART2 (TX2=16, RX2=17)

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

//  Serial.println("MEGA ready");
}

void loop() {
  float distance = readUltrasonic();

  // ส่งค่าระยะจริงเสมอ
  Serial2.print("DIST:");
  Serial2.println(distance);

  // แสดงที่ Monitor Mega ด้วย
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000); // ทุก 1 วิ
}

float readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 20000); // timeout 20ms
  return duration * 0.034 / 2; // cm
}
