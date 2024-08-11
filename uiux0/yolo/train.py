from ultralytics import YOLO

# Load a model
#model = YOLO('yolov8n.pt')
#results = model.train(data='coco128.yaml', epochs=100, imgsz=640)

model = YOLO('yolov8n.yaml')
results = model.train(data='D:/www/code/ifbe/unittest/uiux0/yolo/datasets/coco128.yaml', epochs=100, imgsz=640)

#
#model.export(format='onnx')
