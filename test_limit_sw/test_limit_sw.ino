// กำหนดขา Digital สำหรับ Limit Switch แต่ละตัว
const int limitSwitchPin1 = 22;
const int limitSwitchPin2 = 23;

void setup() {
  // ตั้งค่าขา Limit Switch เป็น INPUT
  // เนื่องจากต่อแบบ Pull-down, เราไม่จำเป็นต้องใช้ INPUT_PULLUP
  pinMode(limitSwitchPin1, INPUT);
  pinMode(limitSwitchPin2, INPUT);

  // เริ่มต้น Serial Communication ที่ Baud Rate 9600
  // คุณสามารถเปลี่ยน Baud Rate ได้ตามต้องการ แต่ต้องตรงกับ Serial Monitor
  Serial.begin(9600);
  Serial.println("Arduino Limit Switch Reader - Ready");
}

void loop() {
  // อ่านค่าสถานะจาก Limit Switch ตัวที่ 1
  // เมื่อกด Limit Switch (วงจรปิด) ค่าจะเป็น HIGH (1)
  // เมื่อไม่กด Limit Switch (วงจรเปิด) ค่าจะเป็น LOW (0)
  int switchState1 = digitalRead(limitSwitchPin1);

  // อ่านค่าสถานะจาก Limit Switch ตัวที่ 2
  int switchState2 = digitalRead(limitSwitchPin2);

  // แสดงค่าสถานะของ Limit Switch ทั้งสองตัวผ่าน Serial Monitor
  Serial.print("Limit Switch 1: ");
  Serial.print(switchState1); // 0 หรือ 1
  Serial.print(" | Limit Switch 2: ");
  Serial.println(switchState2); // 0 หรือ 1

  // หน่วงเวลาเล็กน้อยเพื่อไม่ให้ส่งข้อมูลเร็วเกินไป
  delay(100);
}
