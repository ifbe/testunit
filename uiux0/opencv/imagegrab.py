import numpy as np
from PIL import ImageGrab
import cv2

# 棋盘格尺寸（单位：米）
#square_size = 0.025  # 假设每个棋盘格宽度为2.5cm

# 准备棋盘格图像的对象点坐标
#objp = np.zeros((np.prod(pattern_size), 3), np.float32)
#objp[:, :2] = np.mgrid[0:pattern_size[0], 0:pattern_size[1]].T.reshape(-1, 2) * square_size

# 棋盘格内角点数目
pattern_size = (7, 7)  # 在棋盘格内有9个内角点，6个外角点

# 存储所有棋盘格图像的对象点坐标和图像点坐标
obj_points = []  # 对象点坐标
img_points = []  # 图像点坐标

# 相机内参矩阵
camera_matrix = np.array([[961,   0, 965],
                          [  0, 947, 514],
                          [  0,   0,   1]])

# 畸变系数
dist_coeffs = np.array([0,0,0,0,0])

# 三维点坐标
object_points = np.array([ [-0.5, 0.5, 0],
                           [ 0.5, 0.5, 0],
                           [-0.5,-0.5, 0],
                           [ 0.5,-0.5, 0]
                           ])

countframe = 0
countboard = 0
while True:
    countframe += 1
    print("countframe:", countframe)

    screenshot = ImageGrab.grab()
    screenshot_np = np.array(screenshot)

    gray = cv2.cvtColor(screenshot_np, cv2.COLOR_BGR2GRAY)
    inverted_image = cv2.bitwise_not(gray)

    alpha = 1.5  # 控制对比度（1.0 表示原始对比度）
    beta = 30  # 控制亮度
    adjusted_image = cv2.convertScaleAbs(inverted_image, alpha=alpha, beta=beta)

    # 查找棋盘格角点
    ret, corners = cv2.findChessboardCorners(adjusted_image, pattern_size, None)

    # 如果找到了角点，添加对象点坐标和图像点坐标
    if ret == True:
        countboard += 1
        print("countboard:", countboard)

        # 绘制棋盘格角点
        cv2.drawChessboardCorners(adjusted_image, pattern_size, corners, ret)

        # 找到最外层的四个角点
        sorted_corners = corners.squeeze()  # 去除多余的维度
        sorted_corners = sorted(sorted_corners, key=lambda x: x[0])  # 按 x 坐标排序
        top_left = sorted_corners[0]
        bottom_right = sorted_corners[-1]

        sorted_corners = sorted(sorted_corners, key=lambda x: x[1])  # 按 y 坐标排序
        top_right = sorted_corners[-1]
        bottom_left = sorted_corners[0]

        # 绘制最外层的四个角点
        cv2.circle(screenshot_np, tuple(map(int, top_left)), 5, (0, 0, 255), -1)
        cv2.circle(screenshot_np, tuple(map(int, top_right)), 5, (0, 255, 0), -1)
        cv2.circle(screenshot_np, tuple(map(int, bottom_left)), 5, (255, 0, 0), -1)
        cv2.circle(screenshot_np, tuple(map(int, bottom_right)), 5, (255, 0, 255), -1)

        image_points = np.array([ [top_left],[top_right],[bottom_left],[bottom_right] ])
        success, rvecs, tvecs = cv2.solvePnP(object_points, image_points, camera_matrix, dist_coeffs)
        if success:
            print("Translation Vector:")
            print(tvecs)
            print("Rotation Vector:")
            print(rvecs)
            rotation_matrix, _ = cv2.Rodrigues(rvecs)
            print("Rotation Matrix:")
            print(rotation_matrix)
        else:
            print("solvePnP 失败")

        # 将平移向量转换为字符串
        tvecs[1] = -tvecs[1]
        tvecs_str = np.array2string(tvecs, precision=2, separator=',', suppress_small=True)
        cv2.putText(screenshot_np, "Translation Vector:", (50, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 1)
        cv2.putText(screenshot_np, tvecs_str, (50, 40), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)

        # 将平移向量转换为字符串
        rvecs_str = np.array2string(rvecs, precision=2, separator=',', suppress_small=True)
        cv2.putText(screenshot_np, "Rotation Vector:", (50, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 1)
        cv2.putText(screenshot_np, rvecs_str, (50, 80), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)

        # 将旋转矩阵转换为字符串
        rotation_matrix_str = np.array2string(rotation_matrix, precision=2, separator=',', suppress_small=True)
        cv2.putText(screenshot_np, "Rotation Matrix:", (50, 100), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 1)
        cv2.putText(screenshot_np, rotation_matrix_str, (50, 120), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)

    cv2.imshow('Chessboard Corners', screenshot_np)
    if cv2.waitKey(1) == ord('q'):
        break

cv2.destroyAllWindows()
