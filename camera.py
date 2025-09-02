import cv2
from pyzbar.pyzbar import decode

cap = cv2.VideoCapture(1)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    decoded_list = decode(frame)
    for obj in decoded_list:
        # ดึงข้อมูล
        data = obj.data.decode()
        type_ = obj.type

        (x, y, w, h) = obj.rect
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        pts = obj.polygon
        if len(pts) > 4:
            hull = cv2.convexHull(np.array([pt for pt in pts], dtype=np.float32))
            hull = list(map(tuple, np.squeeze(hull)))
        else:
            hull = pts
        for j in range(len(hull)):
            cv2.line(frame, hull[j], hull[(j + 1) % len(hull)], (255, 0, 0), 2)  

        cv2.putText(frame, f'{data} ({type_})', (x, y - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2) 

    cv2.imshow("QR & Barcode Scanner", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
