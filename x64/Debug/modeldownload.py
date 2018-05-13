# -*- coding:utf-8 -*-
import sys, os, re

def copyFileDir(srcFilename, desFilename):
    status = False
    try:
        fileList = os.listdir(srcFilename)
        for eachFile in fileList:
            sourceF = os.path.join(srcFilename, eachFile)
            targetF = os.path.join(desFilename, eachFile)

            if os.path.isdir(sourceF):
                if not os.path.exists(targetF):
                    os.makedirs(targetF)
                status = copyFileDir(sourceF, targetF)
            else:
                status = copyFile(sourceF, targetF)
    except Exception as e:
        print(e)
        status = False
    return status


def copyFile(srcFilename, desFilename):
    status = False
    if os.path.exists(desFilename) and os.path.getsize(desFilename)==os.path.getsize(srcFilename):
        return True
    copyCommand = 'copy %s %s' % (srcFilename, desFilename)

    try:
        if (os.popen(copyCommand)):  # 不用op.system(copyCommand),因为这个会弹出命令行界面
            print(srcFilename,'copy ok!')
            status = True
        else:
            print('copy failed!')
            status = False
    except Exception as e:
        print(e)
        status = False
    return status


def copyFromSharePath(srcFilename, desFilename):
    if not os.path.exists(srcFilename):
        print('no found ' + srcFilename)
        return False
    if not os.path.exists(desFilename):
        print('no found ' + desFilename)
        os.makedirs(str(desFilename))
        print('create ' + desFilename)

    copyStatus = False
    if os.path.isdir(srcFilename):
        copyStatus = copyFileDir(srcFilename, desFilename)
    else:
        copyStatus = copyFile(srcFilename, desFilename)
    return copyStatus


def model_down():
    sharePath = r'\\192.168.25.20\SBDFileSharing\CVTeam\CVmodel\CV_client\py_client'
    resultStr = []
    resultStr.append([])
    srcFilename = sharePath + '\\'
    desFilename = '.'  # 表示当前目录

    print(u'源目录：' + srcFilename)
    print(u'目标目录：' + desFilename)
    copyFromSharePath(srcFilename, desFilename)
    print("model_down_all")


if __name__ == '__main__':

    sharePath = r'\\192.168.25.20\SBDFileSharing\CVTeam\CVmodel\CV_client'
    resultStr = []
    resultStr.append([])
    srcFilename =  sharePath + '\\'
    desFilename = '.'#表示当前目录

    print(u'源目录：' + srcFilename)
    print(u'目标目录：' + desFilename)
    copyFromSharePath(srcFilename, desFilename)

    print('ok')