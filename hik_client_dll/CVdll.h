#pragma once
#ifndef TestDll_H_
#define TestDll_H_
#ifdef MYLIBDLL
#define MYLIBDLL extern "C" _declspec(dllimport) 
#else
#define MYLIBDLL extern "C" _declspec(dllexport) 
#endif

#include <iostream>
#include<fstream>
using namespace std;
typedef int(*FrameFunc)(char*, int, int, int, int);

//类别 0摄像头信息，cam_id,宽,高,"编码格式",备用
//类别 1异常，cam_id,error_code,级别,信息,备用
//类别 2监控信息，cam_id,信息内容 1收数据 2保存dat 3 获取packet 3解码 4预测 5发送 级别,信息,备用
typedef int(*ErrorCallBack)(int, int, int,int,char*,int);// "ERROR" "WARNING""DEBUG" "INFO"
						//err_type, cam_no, msg_no, msg_level, msg_txt, spare

typedef int(*CamInfoCallBack)(int, char*, int);
MYLIBDLL int sendcmd(char* deviceId, int cam_no, int type, int size);
MYLIBDLL int set_callback(ErrorCallBack errorcall(int error_type, int cam_no, int err_no, int msg_level, char* msg_txt, int spare));
MYLIBDLL int getcameralist(int type, CamInfoCallBack caminforcall(int cam_no, char* cam_info, int cam_info_size));
MYLIBDLL int tcp_init(char* ip, int port);
MYLIBDLL int tcp_recv_conn(char* ip, int port, char* cam_name, int size, int cam_no, FrameFunc tcallback(char* a, int size, int cam_no, int height, int width));
MYLIBDLL char* testchar(int plus1);
MYLIBDLL char* testimg(char* data, int length);
MYLIBDLL void* pre_encode(int width, int height);
MYLIBDLL char* push_rtsp(int* plus1, int len, void* vp);
MYLIBDLL int writedata(const char* data, int size);
MYLIBDLL int dataclose();
//MYLIBDLL int CallPFun(CompareFunc callback, string a, string b);
MYLIBDLL int ffmpeg_recv(int cam_no, FrameFunc tcallback(char* a, int size, int cam_no, int height, int width));
MYLIBDLL int play_file(char* file_name, FrameFunc tcallback(char* a, int size, int num, int height, int width));
//You can also write like this:
//extern "C" {
//_declspec(dllexport) int Add(int plus1, int plus2);
//};
#endif