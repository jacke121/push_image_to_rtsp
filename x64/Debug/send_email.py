# !/usr/bin/python
# -*- coding: UTF-8 -*-

import smtplib
from email.mime.text import MIMEText
from email.header import Header

sender = 'sbd_cv_notify@sibaida.com'
mail_user ='sbd_cv_notify@sibaida.com'
receivers = []  # 接收邮件，可设置为你的QQ邮箱或者其他邮箱

# 第三方 SMTP 服务
mail_host = "smtp.sibaida.com"  # 设置服务器

receivers.append("libanggeng@sibaida.com")  # 用户名
receivers.append("mamingyang@sibaida.com")  # 用户名
receivers.append("wangyifan@sibaida.com")  # 用户名
receivers.append("sangyuanbo@sibaida.com")  # 用户名
mail_pass = "Sibaida123456"  # 口令



def sendMsa(subject,content):
    try:
        message = MIMEText(content, 'plain', 'utf-8')
        message['From'] = Header("思百达物联-摄像头异常通知", 'utf-8')
        # message['To'] = Header("监控平台异常", 'utf-8')
        message['Subject'] = Header(subject, 'utf-8')
        smtpObj = smtplib.SMTP()
        smtpObj.connect(mail_host, 25)  # 25 为 SMTP 端口号
        smtpObj.login(mail_user, mail_pass)
        smtpObj.sendmail(sender, receivers, message.as_string())
        print("邮件发送成功")

    except smtplib.SMTPException as e:
        print("Error:",e.__cause__)

# sendMsa("邮件标题","邮件内容")