from ultralytics import YOLO
from PIL import Image
import cv2

'''
model = YOLO("model.pt")
# accepts all formats - image/dir/Path/URL/video/PIL/ndarray. 0 for webcam
results = model.predict(source="0")
results = model.predict(source="folder", show=True) # Display preds. Accepts all YOLO predict arguments

# from PIL
im1 = Image.open("bus.jpg")
results = model.predict(source=im1, save=True)  # save plotted images

# from ndarray
im2 = cv2.imread("bus.jpg")
results = model.predict(source=im2, save=True, save_txt=True)  # save predictions as labels

# from list of PIL/ndarray
results = model.predict(source=[im1, im2])
'''

#model = YOLO('yolov8n.pt')
model = YOLO('runs/detect/train12/weights/best.pt')

results = model.predict(source=["mydataset/images/dog.webp","datasets/coco128/images/train2017/000000000368.jpg"])
#results = model.predict(source="datasets/coco128/images/train2017")

#print(results)
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
    r.show()
