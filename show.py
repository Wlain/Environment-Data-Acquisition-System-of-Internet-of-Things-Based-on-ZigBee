#-*- coding:utf-8 -*-
from tkinter import *
import matplotlib
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import urllib.request
import json
from datetime import datetime,timedelta


def get_data(url,date):
    reponse = urllib.request.urlopen(url)
    html = reponse.read().decode('utf-8')
    PullData= json.loads(html)
    DataArry = PullData['data']
    xar,yar=[],[]
    for each in DataArry:

        if len(each)>1:
            y,x=each
            x=datetime.strptime(x,'%Y-%m-%d %H:%M:%S')
            if x>date and x<(date+timedelta(days=1)):
                xar.append(x)
                yar.append(float(y))

    return xar,yar


def drawPic():
    url=var.get()
    try:
        sampleCount=datetime.strptime(inputEntry.get(),'%Y-%m-%d')

    except:
        sampleCount='2017-6-12'
        print ('输入正确日期格式')
        inputEntry.delete(0,END)
        inputEntry.insert(0,'2017-6-12')
    drawPic.f.clf()
    drawPic.a=drawPic.f.add_subplot(111)

    x,y=get_data(url,sampleCount)
    drawPic.a.set_xlim(min(x), max(x))
    drawPic.a.plot(x,y,color='r',label='wendu',)
    xlabels = drawPic.a.get_xticklabels()
    for label in drawPic.a.xaxis.get_ticklabels():
        label.set_fontsize(5)
    for label in drawPic.a.yaxis.get_ticklabels():
        label.set_fontsize(5)
        # 设置x轴刻度显示时间格式
    for xl in xlabels:
        xl.set_rotation(45)

    drawPic.a.set_title('Demo: DATA Display')
    drawPic.a.set_xlabel(sampleCount.date())
    drawPic.canvas.show()


if __name__ == '__main__':
    wendu_url = 'http://localhost:8080/findAllWendu'
    shidu_url = 'http://localhost:8080/findAllShidu'
    gas_url = 'http://localhost:8080/findGas'
    sun_url='http://localhost:8080/findGuangzhao'
    data = [('温度', wendu_url), ('湿度', shidu_url), ('可燃气', gas_url),('光照',sun_url)]

    matplotlib.use('TkAgg')

    root=Tk()
    drawPic.f = Figure(figsize=(9,7), dpi=100)

    drawPic.canvas = FigureCanvasTkAgg(drawPic.f, master=root)

    drawPic.canvas.show()

    drawPic.canvas.get_tk_widget().grid(row=0, columnspan=3)
    var = StringVar()
    var.set(1)
    count = 0
    for each in data:
        Radiobutton(root, text=each[0], variable=var, value=each[1]).grid(row=1, column=count, sticky=W)
        count += 1

    Label(root,text='请输入查询的日期(Y-m-d)：').grid(row=2,column=0)

    inputEntry=Entry(root)

    inputEntry.grid(row=2,column=1)

    inputEntry.insert(0,'2017-6-12')

    Button(root,text='画图',command=drawPic).grid(row=2,column=2,columnspan=3)


    root.mainloop()