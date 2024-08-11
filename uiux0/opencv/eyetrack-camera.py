import numpy as np
from PIL import ImageGrab
import cv2

face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')
eye_cascade = cv2.CascadeClassifier('D:/tool/python-3.11.2-embed-amd64/Lib/site-packages/cv2/data/haarcascade_eye.xml')

cap = cv2.VideoCapture(0)  # 打开默认的摄像头

countframe = 0
countboard = 0
while True:
    ret, darkframe = cap.read()  # 读取视频流的帧
    if not ret:
        break

    brightness = 50
    frame = cv2.convertScaleAbs(darkframe, alpha=1, beta=brightness)

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    countframe += 1
    print("countframe:", countframe)

    #face
    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.3, minNeighbors=5)
    for (x, y, w, h) in faces:
        print("face:", x, y)
        cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 0, 255), 2)

        roi_gray = gray[y:y+h, x:x+w]
        roi_color = frame[y:y+h, x:x+w]

        eyes = eye_cascade.detectMultiScale(roi_gray)
        for (ex, ey, ew, eh) in eyes:
            cv2.rectangle(roi_color, (ex, ey), (ex+ew, ey+eh), (0, 255, 0), 2)

    # 显示图像
    cv2.imshow('Eye Tracking', frame)

    # 按下 'q' 键退出循环
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 释放资源
cv2.destroyAllWindows()



'''
    #eye
    eyes = eye_cascade.detectMultiScale(frame, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))

    for (x, y, w, h) in eyes:
        cv2.rectangle(frame, (x, y), (x+w, y+h), (255, 0, 0), 2)
'''

'''
import numpy as np
from PIL import ImageGrab
import cv2

# 加载人眼检测器
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')
eye_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_eye.xml')
nose_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_mcs_nose.xml')
mouth_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_mcs_mouth.xml')

countframe = 0
countboard = 0
while True:
    countframe += 1
    print("countframe:", countframe)

    screenshot = ImageGrab.grab()

    image_bgr = np.array(screenshot)

    frame = cv2.cvtColor(image_bgr, cv2.COLOR_BGR2RGB)
    gray = cv2.cvtColor(image_bgr, cv2.COLOR_BGR2GRAY)
    
    # 检测人脸
    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.3, minNeighbors=5)

    for (x, y, w, h) in faces:
        roi_gray = gray[y:y+h, x:x+w]
        roi_color = frame[y:y+h, x:x+w]
        
        # 检测眼睛
        eyes = eye_cascade.detectMultiScale(roi_gray)
        for (ex, ey, ew, eh) in eyes:
            cv2.rectangle(roi_color, (ex, ey), (ex+ew, ey+eh), (0, 255, 0), 2)
        
        # 检测鼻子
        noses = nose_cascade.detectMultiScale(roi_gray)
        for (nx, ny, nw, nh) in noses:
            cv2.rectangle(roi_color, (nx, ny), (nx+nw, ny+nh), (255, 0, 0), 2)
        
        # 检测嘴巴
        mouths = mouth_cascade.detectMultiScale(roi_gray)
        for (mx, my, mw, mh) in mouths:
            cv2.rectangle(roi_color, (mx, my), (mx+mw, my+mh), (0, 0, 255), 2)

    # 显示结果
    cv2.imshow('Facial Features Detection', gray)

    # 按下 'q' 键退出循环
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 释放资源
cv2.destroyAllWindows()
'''