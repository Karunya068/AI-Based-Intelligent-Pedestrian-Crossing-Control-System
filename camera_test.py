import cv2
from ultralytics import YOLO

# Load YOLO model
model = YOLO("yolov8n.pt")  # nano = fastest

cap = cv2.VideoCapture(0)

print("YOLO Detection Started... Press ESC to exit")

while True:
    ret, frame = cap.read()
    if not ret:
        break

    results = model(frame, conf=0.4)

    for r in results:
        for box in r.boxes:
            cls = int(box.cls[0])

            # 0 = person class
            if cls == 0:
                x1, y1, x2, y2 = map(int, box.xyxy[0])
                conf = float(box.conf[0])

                label = f"Person {conf:.2f}"
                cv2.rectangle(frame, (x1,y1), (x2,y2), (0,255,0), 2)
                cv2.putText(frame, label, (x1, y1-10),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,255,0), 2)

    cv2.imshow("YOLO Human Detection", frame)

    if cv2.waitKey(1) == 27:
        break

cap.release()
cv2.destroyAllWindows()