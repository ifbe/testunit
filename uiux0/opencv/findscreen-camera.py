import cv2
import numpy as np

cap = cv2.VideoCapture(0)  # 打开默认的摄像头

ret, oldframe = cap.read()
oldframe_gray = cv2.cvtColor(oldframe, cv2.COLOR_BGR2GRAY)

while True:
    ret, frame = cap.read()  # 读取视频流的帧
    if not ret:
        break

    frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    diff = cv2.absdiff(oldframe_gray, frame_gray)

    total_sum = np.sum(diff)
    print("灰度图像所有像素值的总和为:", total_sum)

    oldframe_gray = frame_gray

    cv2.imshow('Detected Screens', diff)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()




'''
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    edges = cv2.Canny(gray, 50, 150, apertureSize=3)
    lines = cv2.HoughLines(edges, 1, np.pi/180, threshold=100)

    if lines is not None:
        for line in lines:
            rho, theta = line[0]
            a = np.cos(theta)
            b = np.sin(theta)
            x0 = a * rho
            y0 = b * rho
            x1 = int(x0 + 1000 * (-b))
            y1 = int(y0 + 1000 * (a))
            x2 = int(x0 - 1000 * (-b))
            y2 = int(y0 - 1000 * (a))

            cv2.line(frame, (x1, y1), (x2, y2), (0, 0, 255), 2)
'''