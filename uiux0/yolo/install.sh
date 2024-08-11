#pip
wget https://bootstrap.pypa.io/get-pip.py
python .\get-pip.py -i https://pypi.tuna.tsinghua.edu.cn/simple
edit python311._pth, remove "#" before "import site"

#yolo8
pip install ultralytics -i https://pypi.tuna.tsinghua.edu.cn/simple
