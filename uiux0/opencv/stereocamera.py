import numpy as np
import cv2

# 初始化两个摄像头
cap_left = cv2.VideoCapture(0)  # 假设左摄像头的ID为0
cap_right = cv2.VideoCapture(1)  # 假设右摄像头的ID为1

# 创建立体匹配对象
stereo = cv2.StereoBM_create(numDisparities=16, blockSize=15)

frame_count = 0  # 用于记录处理的帧数

while True:
    # 读取左右摄像头图像
    ret_left, frame_left = cap_left.read()
    ret_right, frame_right = cap_right.read()

    if not ret_left or not ret_right:
        print("无法读取摄像头图像")
        break

    # 转换为灰度图像，因为立体匹配需要灰度图像
    gray_left = cv2.cvtColor(frame_left, cv2.COLOR_BGR2GRAY)
    gray_right = cv2.cvtColor(frame_right, cv2.COLOR_BGR2GRAY)

    # 计算视差
    disparity = stereo.compute(gray_left, gray_right)

    # 归一化视差图像以便显示
    disp_display = cv2.normalize(disparity, disparity, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX, dtype=cv2.CV_8U)

    # 构建文件名
    filename = f"disparity_{frame_count}.png"

    # 将视差图像保存到文件
    cv2.imwrite(filename, disp_display)

    print(f"已保存：{filename}")
    frame_count += 1

    if frame_count >= 10:  # 假设我们只处理并保存前10帧
        break

# 释放资源并关闭窗口
cap_left.release()
cap_right.release()
cv2.destroyAllWindows()