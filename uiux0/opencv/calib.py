import numpy as np
from PIL import ImageGrab
import cv2

# 棋盘格尺寸（单位：米）
square_size = 0.025  # 假设每个棋盘格宽度为2.5cm

# 棋盘格内角点数目
pattern_size = (7, 7)  # 在棋盘格内有9个内角点，6个外角点

# 准备棋盘格图像的对象点坐标
objp = np.zeros((np.prod(pattern_size), 3), np.float32)
objp[:, :2] = np.mgrid[0:pattern_size[0], 0:pattern_size[1]].T.reshape(-1, 2) * square_size

# 存储所有棋盘格图像的对象点坐标和图像点坐标
obj_points = []  # 对象点坐标
img_points = []  # 图像点坐标

# 加载并处理棋盘格图像
#images = ["image1.jpg", "image2.jpg", "image3.jpg"]  # 替换为你的棋盘格图像路径

#for fname in images:
    #img = cv2.imread(fname)
    #gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

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

        obj_points.append(objp)
        img_points.append(corners)

        # 可视化角点
        cv2.drawChessboardCorners(adjusted_image, pattern_size, corners, ret)

        if(countboard > 10):
            # 进行相机标定
            ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(obj_points, img_points, adjusted_image.shape[::-1], None, None)

            # 打印标定结果
            print("相机矩阵:\n", mtx)
            print("畸变系数:\n", dist)


            focal_length = mtx[0, 0]  # 焦距在相机内参矩阵的第一行第一列

            # 图像宽度和高度（假设为 640x480）
            image_width = 1920
            image_height = 1080

            # 计算水平和垂直视场角
            FOVx = 2 * np.arctan(image_width / (2 * focal_length)) * (180/np.pi)
            FOVy = 2 * np.arctan(image_height / (2 * focal_length)) * (180/np.pi)

            print("水平视场角（FOVx）: {:.2f} 度".format(FOVx))
            print("垂直视场角（FOVy）: {:.2f} 度".format(FOVy))

    cv2.imshow('Chessboard Corners', adjusted_image)
    if cv2.waitKey(1) == ord('q'):
        break

cv2.destroyAllWindows()
