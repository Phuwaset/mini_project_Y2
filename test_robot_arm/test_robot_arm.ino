#include <Servo.h> // ไลบรารีสำหรับควบคุมเซอร์โวมอเตอร์

// กำหนดเซอร์โวมอเตอร์ 4 ตัว
Servo servo1; // เซอร์โวตัวที่ 1 (น่าจะเป็นฐานหมุน)
Servo servo2; // เซอร์โวตัวที่ 2 (น่าจะเป็นข้อต่อแขนด้านล่าง)
Servo servo3; // เซอร์โวตัวที่ 3 (น่าจะเป็นข้อต่อแขนด้านบน)
Servo servo4; // เซอร์โวตัวที่ 4 (น่าจะเป็น Gripper หรือตัวหนีบ)

// ฟังก์ชันสำหรับเคลื่อนที่เซอร์โวอย่างนุ่มนวล
// servo: อ้างอิงถึงวัตถุ Servo ที่ต้องการควบคุม
// startAngle: มุมเริ่มต้น
// endAngle: มุมสุดท้าย
// stepDelay: หน่วงเวลาในแต่ละขั้น (มิลลิวินาที) เพื่อให้เคลื่อนที่ช้าลง
void smoothMove(Servo &servo, int startAngle, int endAngle, int stepDelay) {
  if (startAngle < endAngle) {
    for (int pos = startAngle; pos <= endAngle; pos++) {
      servo.write(pos);
      delay(stepDelay); // ปรับความเร็วของการเคลื่อนที่
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
  Serial.begin(9600); // เริ่มต้น Serial Monitor ที่ Baud Rate 9600 (สำหรับ Debugging)

  // กำหนดขาเซอร์โวให้กับวัตถุ Servo
  servo1.attach(9);
  servo2.attach(10);
  servo3.attach(11);
  servo4.attach(6); // Gripper (ตัวหนีบ)

  // กำหนดตำแหน่งเริ่มต้นของเซอร์โว
  servo1.write(95); // เซอร์โวฐานหมุน
  servo2.write(60); // เซอร์โวข้อต่อแขนด้านล่าง
  servo3.write(90); // เซอร์โวข้อต่อแขนด้านบน
  servo4.write(150); // Gripper เปิด 150 ปิด 90

  Serial.println("Arm Control Ready !!!"); // แสดงข้อความเมื่อพร้อมทำงาน
  delay(2000); // หน่วงเวลาเพื่อให้แน่ใจว่าเซอร์โวอยู่ในตำแหน่งเริ่มต้น

  // ตัวอย่างการเรียกใช้ฟังก์ชันควบคุมแขนกล
  // คุณสามารถเพิ่มเงื่อนไขหรือการเรียกใช้ฟังก์ชันเหล่านี้ใน loop() ได้ตามต้องการ
  // เช่น เรียกใช้ PickAndPlace() เมื่อตรวจพบวัตถุ
  // หรือเรียกใช้ HOME() เพื่อกลับสู่ตำแหน่งเริ่มต้น
}

// ฟังก์ชันวนซ้ำหลัก (รันซ้ำไปเรื่อยๆ)
void loop() {
  // ในส่วนนี้ คุณสามารถเพิ่มตรรกะการควบคุมแขนกลตามที่คุณต้องการ
  // เช่น การรับคำสั่งจาก Serial, การกดปุ่ม, หรือการรวมกับเซ็นเซอร์อื่นๆ

  // ตัวอย่าง: การเคลื่อนที่แขนกลเพื่อหยิบและวางวัตถุ (จำลอง)
  // หากคุณต้องการให้แขนกลทำงานอัตโนมัติ ให้ลบ comment ออกและปรับเงื่อนไข
  /*
  Serial.println("Performing Pick and Place sequence...");
  pickObject(); // เรียกฟังก์ชันหยิบวัตถุ
  delay(2000); // รอสักครู่

  // สมมติว่านี่คือตำแหน่งสำหรับวางวัตถุ
  smoothMove(servo1, servo1.read(), 0, 10); // เลื่อนไปตำแหน่งวาง
  delay(1000);
  smoothMove(servo4, servo4.read(), 150, 5); // ปล่อยวัตถุ
  delay(1000);
  HOME(); // กลับสู่ตำแหน่งเริ่มต้น
  delay(5000); // รอ 5 วินาทีก่อนทำซ้ำ
  */

  // หากไม่มีการทำงานใดๆ ใน loop ให้ใส่ delay เพื่อไม่ให้ CPU ทำงานหนักเกินไป
  delay(100);
}

// ฟังก์ชันสำหรับขั้นตอนการหยิบวัตถุ
void pickObject() {
  Serial.println("Picking up object...");
  smoothMove(servo1, servo1.read(), 100, 10); // เลื่อน servo1 ไปตำแหน่ง 100
  smoothMove(servo3, servo3.read(), 30, 10);  // เลื่อน servo3 ไปตำแหน่ง 30 (ลดระดับแขน)
  delay(2000); // รอให้แขนเคลื่อนที่

  servo4.write(150); // ตรวจสอบให้แน่ใจว่า Gripper เปิดอยู่
  smoothMove(servo2, servo2.read(), 150, 10); // เลื่อน servo2 ไปตำแหน่ง 150 (ยื่นแขนออกไป)
  delay(2000); // รอให้แขนยื่นออกไป

  servo4.write(110); // หนีบวัตถุ (110 คือหนีบ)
  delay(1000); // หน่วงเวลาให้หนีบสนิท

  smoothMove(servo2, servo2.read(), 90, 10); // ยกแขนขึ้นเล็กน้อย
  delay(1000);

  smoothMove(servo1, servo1.read(), 42, 10); // เลื่อน servo1 ไปตำแหน่ง 42 (เตรียมพร้อมสำหรับเคลื่อนที่ต่อไป)
  Serial.println("Object picked up.");
}

// ฟังก์ชัน HOME: นำแขนกลกลับสู่ตำแหน่งเริ่มต้น
void HOME() {
  Serial.println("Returning to HOME position...");
  // ปล่อยวัตถุหากยังหนีบอยู่ (เผื่อกรณีที่เรียก HOME ขณะหนีบอยู่)
  smoothMove(servo4, servo4.read(), 150, 5); // เปิด Gripper
  delay(500);

  // เคลื่อนแขนกลับสู่ตำแหน่งเริ่มต้นอย่างเป็นลำดับ
  smoothMove(servo2, servo2.read(), 70, 10); // ยกแขนขึ้น
  smoothMove(servo3, servo3.read(), 90, 10); // ปรับระดับข้อต่อแขนบน
  smoothMove(servo1, servo1.read(), 95, 10); // หมุนฐานกลับตำแหน่งกลาง
  delay(1000); // รอให้แขนกลับตำแหน่ง

  // ตรวจสอบให้แน่ใจว่า Gripper เปิดอยู่
  servo4.write(150);

  Serial.println("Finished HOME sequence.");
}
