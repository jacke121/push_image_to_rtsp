

#coding=utf-8
from PIL import Image
import os

ppath="D:/mouse_p"
# ppath="bmp"
files = os.listdir(ppath)
index=1
for fi in files:
    fi_d = os.path.join(ppath, fi)
    if (fi_d.endswith('jpg')):
        img = Image.open(fi_d)
        img = img.resize((60, 40))
        print(img.format)
        img.save("bmp/"+str(index)+".bmp")
        index+=1
        if index>200:
            break
        # img.show()