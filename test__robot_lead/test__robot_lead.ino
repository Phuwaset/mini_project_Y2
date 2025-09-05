#include <Servo.h>  // ไลบรารีสำหรับควบคุมเซอร์โวมอเตอร์

// กำหนดเซอร์โวมอเตอร์ 4 ตัว
Servo servo1;  // เซอร์โวตัวที่ 1 (น่าจะเป็นฐานหมุน)
Servo servo2;  // เซอร์โวตัวที่ 2 (น่าจะเป็นข้อต่อแขนด้านล่าง)
Servo servo3;  // เซอร์โวตัวที่ 3 (น่าจะเป็นข้อต่อแขนด้านบน)
Servo servo4;  // เซอร์โวตัวที่ 4 (น่าจะเป็น Gripper หรือตัวหนีบ)

// ฟังก์ชันสำหรับเคลื่อนที่เซอร์โวอย่างนุ่มนวล
// servo: อ้างอิงถึงวัตถุ Servo ที่ต้องการควบคุม
// startAngle: มุมเริ่มต้น
// endAngle: มุมสุดท้าย
// stepDelay: หน่วงเวลาในแต่ละขั้น (มิลลิวินาที) เพื่อให้เคลื่อนที่ช้าลง
void smoothMove(Servo &servo, int startAngle, int endAngle, int stepDelay) {
  if (startAngle < endAngle) {
    for (int pos = startAngle; pos <= endAngle; pos++) {
      servo.write(pos);
      delay(stepDelay);  // ปรับความเร็วของการเคลื่อนที่
    }
  } else {
    for (int pos = startAngle; pos >= endAngle; pos--) {
      servo.write(pos);
      delay(stepDelay);
    }
  }
}

// ฟังก์ชันตั้งค่าเริ่มต้น (รันครั้งเดียวเมื่อเปิดบอร์ด)
void setup() {
  Serial.begin(9600);  // เริ่มต้น Serial Monitor ที่ Baud Rate 9600 (สำหรับ Debugging)

  // กำหนดขาเซอร์โวให้กับวัตถุ Servo
  servo1.attach(6);
  servo2.attach(7);
  servo3.attach(8);

  // กำหนดตำแหน่งเริ่มต้นของเซอร์โว
  servo1.write(0);  //ฐานหมุน 0-180
  servo2.write(90);  // แขนยก 90 ตรงกลาง 90+ ยกขึ้น 90- ยกลง
  servo3.write(0);

  Serial.println("Arm Control Ready !!!");  // แสดงข้อความเมื่อพร้อมทำงาน
  delay(2000);                              // หน่วงเวลาเพื่อให้แน่ใจว่าเซอร์โวอยู่ในตำแหน่งเริ่มต้น

  // ตัวอย่างการเรียกใช้ฟังก์ชันควบคุมแขนกล
  // คุณสามารถเพิ่มเงื่อนไขหรือการเรียกใช้ฟังก์ชันเหล่านี้ใน loop() ได้ตามต้องการ
  // เช่น เรียกใช้ PickAndPlace() เมื่อตรวจพบวัตถุ
  // หรือเรียกใช้ HOME() เพื่อกลับสู่ตำแหน่งเริ่มต้น
}

// ฟังก์ชันวนซ้ำหลัก (รันซ้ำไปเรื่อยๆ)
void loop() {


}
