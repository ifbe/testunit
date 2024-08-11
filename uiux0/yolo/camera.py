import cv2
from ultralytics import YOLO

# Load the YOLOv8 model
#model = YOLO('yolov8n.pt')
model = YOLO('runs/detect/train12/weights/best.pt')

# Open the video file
#video_path = "test.mp4"
#cap = cv2.VideoCapture(video_path)
cap = cv2.VideoCapture(0)

# Loop through the video frames
while cap.isOpened():
    # Read a frame from the video
    print("111")
    success, frame = cap.read()
    print("222")

    if success:
        # Run YOLOv8 inference on the frame
        print("333")
        results = model(frame)
        print("444")

        # print necessary information to stdout
        for r in results:
            print(r.path)
            names = r.names
            count = len(r.boxes.cls)
            for i in range(0,count):
                xyxy = r.boxes.xyxy[i]
                conf = r.boxes.conf[i]
                cls = int(r.boxes.cls[i])
                print("index={}".format(i), end=", ")
                print("xyxy={},{},{},{}".format(int(xyxy[0]), int(xyxy[1]), int(xyxy[2]), int(xyxy[3])), end=", ")
                print("name={}".format(names[cls]), end=", ")
                print("conf={:.2f}%".format(conf))

        # Visualize the results on the frame
        annotated_frame = results[0].plot()
        cv2.imshow("YOLOv8 Inference", annotated_frame)

        # Break the loop if 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break
    else:
        # Break the loop if the end of the video is reached
        break

# Release the video capture object and close the display window
cap.release()
cv2.destroyAllWindows()
