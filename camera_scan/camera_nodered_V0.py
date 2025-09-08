import cv2
from pyzbar.pyzbar import decode
import numpy as np
import paho.mqtt.client as mqtt
import time

# -----------------------------
# MQTT Configuration
# -----------------------------
broker_address = "localhost"  # ถ้า Mosquitto อยู่ในเครื่องอื่นให้เปลี่ยนเป็น IP
broker_port = 1883
topic = "qr_code/data"

# เก็บ QR code ล่าสุดเพื่อป้องกัน publish ซ้ำ
last_published = set()
publish_interval = 2  # วินาที ก่อน publish same QR code อีกครั้ง
last_publish_time = {}

# -----------------------------
# MQTT Callbacks
# -----------------------------
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to Mosquitto MQTT Broker!")
    else:
        print(f"Failed to connect, return code {rc}")

def on_disconnect(client, userdata, rc):
    print("Disconnected from MQTT Broker. Reconnecting...")
    while True:
        try:
            client.reconnect()
            print("Reconnected!")
            break
        except:
            print("Reconnect failed. Retrying in 2s...")
            time.sleep(2)

# -----------------------------
# Setup MQTT Client
# -----------------------------
client = mqtt.Client(client_id="QR_Scanner")
client.on_connect = on_connect
client.on_disconnect = on_disconnect

print(f"Connecting to MQTT Broker at {broker_address}:{broker_port} ...")
try:
    client.connect(broker_address, broker_port, 60)
except Exception as e:
    print("Could not connect to MQTT Broker:", e)
    exit(1)

client.loop_start()  # Start loop in background

# -----------------------------
# Video Capture
# -----------------------------
cap = cv2.VideoCapture(2)
if not cap.isOpened():
    print("Cannot open camera")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab frame")
        break

    decoded_list = decode(frame)
    for obj in decoded_list:
        data = obj.data.decode()
        type_ = obj.type

        # Draw rectangle around QR
        (x, y, w, h) = obj.rect
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        # Draw polygon
        pts = obj.polygon
        if len(pts) > 4:
            hull = cv2.convexHull(np.array([pt for pt in pts], dtype=np.float32))
            hull = list(map(tuple, np.squeeze(hull)))
        else:
            hull = pts
        for j in range(len(hull)):
            cv2.line(frame, hull[j], hull[(j + 1) % len(hull)], (255, 0, 0), 2)

        # Put text
        cv2.putText(frame, f'{data} ({type_})', (x, y - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)

        # -----------------------------
        # Publish QR code to MQTT (with debounce)
        # -----------------------------
        current_time = time.time()
        if data not in last_published or (current_time - last_publish_time.get(data, 0) > publish_interval):
            try:
                client.publish(topic, data)
                print(f"📤 Published to MQTT topic [{topic}]: {data}")
                last_published.add(data)
                last_publish_time[data] = current_time
            except Exception as e:
                print("Failed to publish:", e)

    cv2.imshow("QR & Barcode Scanner", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# -----------------------------
# Cleanup
# -----------------------------
print("Stopping...")
cap.release()
cv2.destroyAllWindows()
client.loop_stop()
client.disconnect()
