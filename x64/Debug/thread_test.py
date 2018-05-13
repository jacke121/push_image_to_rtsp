#!/usr/bin/env Python
# coding=utf-8
from ctypes import *

from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
import time
import numpy as np
import cv2
import struct

import datetime

from numba import jit
import os
cam_dict={}

class CamInfo:
    def __init__(self, cam_no=0,deviceid="default",cam_name="default"):
        self.cam_no = cam_no
        self.deviceid = deviceid
        self.cam_name = cam_name

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
    r = cv2.cvtColor(r, cv2.COLOR_RGB2BGR)
    counter = datetime.datetime.now().strftime('%Y%m%d_%H%M%S_%f')
    # print(1, counter)
    cv2.imshow(str(cam_no), r)
    cv2.waitKey(1)

# def callb_error(self,err_type, cam_no, msg_no,msg_level,msg_txt,msg_txtlen):
#     print("error", err_type, cam_no, msg_no,msg_level,msg_txt,msg_txtlen)

def callb_camerainfo(cam_no,camera_info,camera_info_size):
    # print(cast(camera_info,c_char_p).value)
    # print(str(cast(camera_info, c_char_p).value))
    bbb = string_at(camera_info, camera_info_size)
    info=str(bbb,encoding="utf-8").split(",")
    cam_dict[cam_no]= CamInfo(cam_no,info[1],info[2])
    print("camerainfo",cam_dict[cam_no].cam_no,cam_dict[cam_no].cam_name,cam_dict[cam_no].deviceid)

class WorkThread(QThread):
    trigger = pyqtSignal()
    def __int__(self):
        super(WorkThread, self).__init__()
    def param(self,dll,deviceId,cam_no):
        self.dll=dll
        self.deviceId = deviceId
        self.cam_no=cam_no
        # self.m_callback=m_callback
    def run(self):
        ip = b"127.0.0.1"
        port = 8888
        CMPFUNC = CFUNCTYPE(c_void_p, c_void_p, c_int, c_int, c_int, c_int)
        m_callback = CMPFUNC(callb_stream)
        self.dll.tcp_recv_conn(str2char_p(ip), port, str2char_p(self.deviceId), len(self.deviceId), self.cam_no, m_callback)
        # self.dll.ffmpeg_recv(self.cam_no, self.m_callback)
        for i in range(203300030):
            pass
class Mythread(QThread):
    # 定义信号,定义参数为str类型
    breakSignal = pyqtSignal(str,list)

    def __init__(self, parent=None):
        super().__init__(parent)
        #  super(Mythread, self).__init__()

    def callb_error(self, err_type, cam_no, msg_no, msg_level, msg_txt, msg_txtlen):
        print("myerror", err_type, cam_no, msg_no, msg_level, msg_txt, msg_txtlen)
    def run(self):
        dll = CDLL(r"./hik_client.dll")
        dll.testchar.restype = c_char_p
        a=dll.testchar(11)
        b = string_at(a, 20)
        print(b)
        ErrorCall = CFUNCTYPE(c_void_p, c_int, c_int, c_int,c_int,c_char_p,c_int)
        error_callback = ErrorCall(self.callb_error)
        dll.set_callback(error_callback)
        CamCall = CFUNCTYPE(c_void_p, c_int, c_char_p,c_int)
        caminfo_CamCall = CamCall(callb_camerainfo)
        # print(b)
        if not os.path.exists("video"):
            os.makedirs("video")
        ip = b"127.0.0.1"
        port = 8888
        print("start conn")
        ret=-1
        while(ret):
            print("conn server...")
            ret= dll.tcp_init(str2char_p(ip), port)
            time.sleep(0.3)
        if (ret==0):
            type=1
            ret = dll.getcameralist(type, caminfo_CamCall)
            if (1):
                # deviceId = b"af94a68df0124d1fbf0fc2b07f3b3c3a"
                cam_no=14
                thread = WorkThread()
                # thread.param(dll,cam_dict[cam_no].cam_name,cam_no)
                thread.param(dll, b"video/3_", cam_no)
                thread.start()

                # cam_no = 1
                # thread1 = WorkThread()
                # # thread.param(dll, cam_dict[cam_no].cam_name, cam_no)
                # thread1.param(dll, b"video/bangongshi", cam_no)
                # thread1.start()
                # cam_no = 15
                # thread2 = WorkThread()
                # # thread.param(dll, cam_dict[cam_no].cam_name, cam_no)
                # thread2.param(dll, b"video/2", cam_no)
                # thread2.start()
                #
                # cam_no = 12
                # thread3 = WorkThread()
                # # thread.param(dll, cam_dict[cam_no].cam_name, cam_no)
                # thread3.param(dll, b"video/chaoshicangku", cam_no)
                # thread3.start()
                # # dll.ffmpeg_recv(cam_no,m_callback)



        else:
            print("tcp error")
        for i in range(2000000):
            # 发出信号
            a=[i,i+1]
            self.breakSignal.emit(str(i),a)
            # 让程序休眠
            time.sleep(0.5)

if __name__ == '__main__':

    app = QApplication([])
    dlg = QDialog()
    dlg.resize(400, 300)
    dlg.setWindowTitle("自定义按钮测试")
    dlgLayout = QVBoxLayout()
    dlgLayout.setContentsMargins(40, 40, 40, 40)
    btn = QPushButton('测试按钮')
    dlgLayout.addWidget(btn)
    dlgLayout.addStretch(40)
    dlg.setLayout(dlgLayout)
    dlg.show()

    def chuli(a,s):
        # dlg.setWindowTitle(s)
        btn.setText(a+str(s[0]*10))

    # 创建线程
    thread = Mythread()
    # # 注册信号处理函数
    thread.breakSignal.connect(chuli)
    # # 启动线程
    thread.start()
    dlg.exec_()
    app.exit()