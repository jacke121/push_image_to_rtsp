
import cv2
import numpy as np
img =np.asarray( [[[1,2,3],[4,5,6]],[[11,12,13],[14,15,16]],[[111,112,113],[114,115,116]]])

img2=img.reshape(-1)
print(img2)

# b, g, r = cv2.split(img)
# b = b.reshape(-1)
# g = g.reshape(-1)
# r = r.reshape(-1)
# b = np.append(b, g)
# img = np.append(b, r)
# print(img)