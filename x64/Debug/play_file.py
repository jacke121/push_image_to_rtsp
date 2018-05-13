#!/usr/bin/env Python
# coding=utf-8

from ctypes import *

import time
import numpy as np
import cv2
import struct

import datetime

from numba import jit

cam_dict={}


@jit
def trans(data,size,height,width):
    bbb = string_at(data,size)
    nparr = np.fromstring(bbb, np.uint8)
    r = nparr.reshape(height,width, 3)
    return r
def str2char_p(str_v):
    pStr = c_char_p( )
    pStr.value = str_v
    return pStr
def callb_stream(data,size,cam_no,height,width):
    r = trans(data, size,height,width)
    counter = datetime.datetime.now().strftime('%Y%m%d_%H%M%S_%f')
    # print(1, counter)
    cv2.imshow(str(cam_no), r)
    cv2.waitKey(1)



if __name__ == '__main__':
    dll = CDLL(r"./hik_client.dll")
    dll.testchar.restype = c_char_p
    a = dll.testchar(11)
    b = string_at(a, 20)
    print(b)
    CMPFUNC = CFUNCTYPE(c_void_p, c_void_p, c_int, c_int, c_int, c_int)
    m_callback = CMPFUNC(callb_stream)
    filename=b"1225326282018030214.dat"
    dll.play_file(str2char_p(filename), m_callback)
