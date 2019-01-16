**1．概述** 

鉴于ZigBee技术适合用于数据采集系统的的特点, 提出了基于ZigBee的数据采集系统的设计方案, 着重探讨ZigBee节点的硬件设计及其组网设计. 并详细讨论了基于CC2530芯片的数据采集节点的硬件设计方案, 组网设计中的协调器建立网络、节点加入网络的设计方法, 以及数据采集系统的软件设计方法. 最后通过采集ZigBee网络传感器数据的实验, 证明该方案能取得良好的通信效果.

**1.1 系统描述**

利用ZigBee传感器网络、网关、服务器实现简单的数据采集系统。项目中把使用了三种传感器，分别是：温湿度传感器，烟雾传感器，光敏传感器。终端节点传感器采集到数据之后发送给协调器之后，由协调器通过串口将数据发至给电脑客户端，最后电脑客户端将串口发送上来的传感器数据使用套接字封装成http格式后通过post方式发送到服务端，并且存储到数据库中。客户端通过访问服务器，获取数据展示出来。
**1.2 系统结构介绍**

![这里写图片描述](http://img.blog.csdn.net/20170614234814872?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAwNzA0MzM=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
											


----------


 图1：系统结构图
 
 如图1所示，本系统主要有传感器节点、协调器、网关、服务器、客户端四个部分组成。传感器部分采集温湿度，可燃气，光照数据。通过使用Zstack协议栈来进行zigbee传感器节点组网，使用c语言来控制zigbee终端节点io口采集到传感器数据，协调器再将数据通过串口上传到网关。网关部分从串口接收来自协调器发来的传感器数据，并以及解析从串口收到的数据，并且把数据封装成http格式通过post方式可以实现远程发送数据到服务器。客户端使用python从服务器获取数据，设计图形界面用于数据展示。
 
 **1.3 软硬件开发平台**
 
 软件：   
VMware Workstation（linux OS）
      TCP&UDP测试工具（测试）
      Configure Virtual Serial Port Driver（测试）
      IAR Embedded Workbench（CC2530 传感器）
      sublime（C 网关）
      IntelliJ IDEA 2017.1.1（JAVA 服务器）
      JetBrains CLion 2016.3.4（Python 显示）
      xampp（数据库）
      串口调试助手（测试）  
硬件：温湿度传感器、光照传感器、可燃气传感器、CC2530开发板*4
**1.4 实现功能**

  **通过传感器采集环境数据，并将采集到的数据进行处理后在客户端做展现，客户端可以查询某一日下的数据变化情况。**
  
  **2 系统设计方案** 
  
数据格式：
•	FF00 01 0600 1A 27
•	  FF00: 包标志  2Byte 
•	  01: 传感器类型 1Byte 
•	  0600: 短地址  2Byte 
•	  1A27: 传感器数据 1A：温度26  27：湿度39
•	  传感器类型数据大小
•	  温湿度 PACKET_DHT11_DATA_SIZE         		  2byte
•	  光敏 PACKET_PHOTORESUSTANCE_DATA_SIZE        1byte
•	  可燃气体 PACKET_COMBUSTIBLEGAS_DATA_SIZE     1byte

**3 系统具体实现 （项目整体实现细节、各个模块的流程，关键代码）**

**传感器部分：**

整体流程:
![这里写图片描述](http://img.blog.csdn.net/20170614235006406?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAwNzA0MzM=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
传感器节点接入协调器网络，封装传感器数据

```
 if ( event & MY_REPORT_EVT )      // MY_REPORT_EVT事件触发处理
  {
    if ( appState == APP_REPORT ) 
    {
      pData[PACKET_FLAG_OFFSET] = PACKET_L_FLAG;
      pData[PACKET_FLAG_OFFSET + 1] = PACKET_H_FLAG;
      pData[PACKET_SENSOR_SHORT_ADDR_OFFSET] = (NLME_GetShortAddr() & 0xff);
      pData[PACKET_SENSOR_SHORT_ADDR_OFFSET + 1] = (NLME_GetShortAddr() >> 8);
#if PACKET_SENSOR_TYPE == PACKET_DHT11
    dht11_update(); 
    pData[PACKET_SENSOR_TYPE_OFFSET] = PACKET_DHT11;
    pData[PACKET_SENSOR_DATA_OFFSET] = dht11_temp();
    pData[PACKET_SENSOR_DATA_OFFSET + 1] = dht11_humidity();
#elif PACKET_SENSOR_TYPE == PACKET_PHOTORESUSTANCE
    pData[PACKET_SENSOR_TYPE_OFFSET] = PACKET_PHOTORESUSTANCE;
    pData[PACKET_SENSOR_DATA_OFFSET] = HalAdcRead (HAL_ADC_CHN_AIN1,HAL_ADC_RESOLUTION_8);
#else
    pData[PACKET_SENSOR_TYPE_OFFSET] = PACKET_COMBUSTIBLEGAS;
    pData[PACKET_SENSOR_DATA_OFFSET] = HalAdcRead (HAL_ADC_CHN_AIN1,HAL_ADC_RESOLUTION_8);

```

**上传数据包结构**

【包标志】【传感器类型】【短地址】【数据】	
包标志 2Byte  FF00
传感器类型
温湿度    2byte  传感器类型编号    01
光敏      1byte  传感器类型编号    02
可燃气体  1byte  传感器类型编号    03
将数据包发送给协调器

```
#if PACKET_SENSOR_TYPE == PACKET_DHT11
      zb_SendDataRequest( 0xFFFE, SENSOR_REPORT_CMD_ID, PACKET_BASE_SIZE + PACKET_DHT11_DATA_SIZE, pData, 0, AF_TX_OPTIONS_NONE, 0 );
#elif PACKET_SENSOR_TYPE == PACKET_PHOTORESUSTANCE
      zb_SendDataRequest( 0xFFFE, SENSOR_REPORT_CMD_ID, PACKET_BASE_SIZE + PACKET_PHOTORESUSTANCE_DATA_SIZE, pData, 0, AF_TX_OPTIONS_NONE, 0 );
#else
      zb_SendDataRequest( 0xFFFE, SENSOR_REPORT_CMD_ID, PACKET_BASE_SIZE + PACKET_COMBUSTIBLEGAS_DATA_SIZE, pData, 0, AF_TX_OPTIONS_NONE, 0 );

```

协调器接收到数据包通过串口上传

```
void zb_ReceiveDataIndication( uint16 source, uint16 command, uint16 len, uint8 *pData  )
{ 
  HalUARTWrite(HAL_UART_PORT_0, pData, len);
}

```
协调器接收到网关发送的命令做出命令判断后将其发送给传感器节点

```

  while (Hal_UART_RxBufLen(port))		//计算并返回接收缓冲区的长度
  {
    len = HalUARTRead (port, pBuf, RX_BUF_LEN);
    HalUARTWrite (port, pBuf, len);
    if(len >= PACKET_BASE_SIZE 
       && pBuf[PACKET_FLAG_OFFSET] == PACKET_L_FLAG 
         && pBuf[PACKET_FLAG_OFFSET + 1] == PACKET_H_FLAG)
    {
      zb_SendDataRequest( *(uint16 *)(pBuf + PACKET_SENSOR_SHORT_ADDR_OFFSET), OPERATION_CMD_ID, len, pBuf, 0, AF_TX_OPTIONS_NONE, 0 );
    }
  }


```

**命令数据包结构**

【包标志】【操作指令】【对应传感器短地址】
包标志   2Byte  FF00
操作指令 1Byte  
01  LED开启
02  LED关闭
03  LED状态反转
04  LED闪烁
对应传感器短地址 2Byte
传感器节点接收到协调器控制数据包并解析做出相应的反应

```
void zb_ReceiveDataIndication( uint16 source, uint16 command, uint16 len, uint8 *pData  )
{
  if(len >= PACKET_BASE_SIZE 
     && pData[PACKET_FLAG_OFFSET] == PACKET_L_FLAG 
     && pData[PACKET_FLAG_OFFSET + 1] == PACKET_H_FLAG
     && *(uint16 *)(pData + PACKET_SENSOR_SHORT_ADDR_OFFSET) == NLME_GetShortAddr())
  {
    switch(pData[PACKET_OPERATION_TYPE_OFFSET])
    {
      case PACKET_ON:
        HalLedSet( HAL_LED_1, HAL_LED_MODE_ON );
        break;
      case PACKET_OFF:
        HalLedSet( HAL_LED_1, HAL_LED_MODE_OFF );
        break;
      case PACKET_TOGGLE:
        HalLedSet( HAL_LED_1, HAL_LED_MODE_TOGGLE );
        break;
      case PACKET_BLINK:
        HalLedBlink(HAL_LED_1, 0, 50, 500);
        break;
      default:
        break;
      }
  }
}


```

**网关部分：**

程序流程：
![这里写图片描述](http://img.blog.csdn.net/20170614235233894?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAwNzA0MzM=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
关键代码：

```
//serial part串口部分
    	cssl_t *ser;     
    	cssl_start();
    	ser=cssl_open(dev_path,callback,0,38400,8,0,1);   
    	if (!ser){
        		printf("%s\n",cssl_geterrormsg());
    	}
    	printf("Serial connection successful, DEV = %s\n", dev_path);

```
//create socket
建立socket 建立一个sock连接

```
sockfd = socket( AF_INET,SOCK_STREAM,0 );

	if ( sockfd < 0 )
	{
		fprintf( stderr, "socket:%s\n", strerror(errno));
		exit(1);		
	}
	else{
		printf("success to connect the service\n");
	}

	struct sockaddr_in sockaddr;
	//设置连接信息结构
	memset( &sockaddr,0,sizeof(sockaddr) );
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons ( atoi (port) );
	inet_pton(AF_INET,ip,&sockaddr.sin_addr.s_addr);
	socklen_t len = sizeof(sockaddr);
	//连接到远端服务器 
	if( connect(sockfd,(struct sockaddr*)&sockaddr,len) <0 ){
		fprintf(stderr, "connect: %s\n", strerror(errno) );
		exit(1);
	}

```
回调函数，打印串口收到的数据的信息，在这个方法中对串口收到的数据进行解析，并且调用通过socket发送数据给服务器

```
static void callback(int id, uint8_t *buf,int length)    
{
    int i;
    char dtype[2],data[50];
    printf("the date from serial:%s\n", buf);
    printf("\nread serial success:  length = %d\n", length); 
    sprintf(dtype,"%d",buf[2]);
    switch(dtype[0]){
	case '1':
		sprintf(data,"wendu=%d&shidu=%d",buf[5],buf[6]);
		puts(data);
		sendDatawenshidu(data,strlen(data));
		break;
	case '2':
		sprintf(data,"gz=%d",buf[5]);
		puts(data);
		sendDataguangzhao(data,strlen(data));
		break;
	case '3':
		sprintf(data,"gs=%d",buf[5]);
		puts(data);
		sendDataGas(data,strlen(data));
		break;
	default:
		puts("the date is error"); 
		break;
	}
}

```

把数据封装成http格式，并且通过post方式发送传感器数据

```
void sendDatawenshidu(uint8_t *buf,int length)
{
    printf("buf:%s\n",buf);
    //接口
    char page[] = "addWendu";  
    char content[4096];  
    char content_page[50];  
    char content_host[50];  
    char content_len[50]; 
    //1请求行
sprintf(content_page,"POST /%s HTTP/1.1\r\n",page);  

//把数据封装成http格式，并且通过post方式发送光照值数据
void sendDataguangzhao(uint8_t *buf,int length)
{
    printf("buf:%s\n",buf);
    char page[] = "Guangzhao";  
    char content[4096];  
    char content_page[50];  
    char content_len[50];  
    char content_host[50]; 
    sprintf(content_page,"POST /%s HTTP/1.1\r\n",page);  
    sprintf(content_host,"HOST: %s:%d\r\n",ip,port);  
    char content_type[] = "Content-Type: application/x-www-form-urlencoded\r\n";  
    sprintf(content_len,"Content-Length: %d\r\n\r\n",strlen(buf));  
    sprintf(content,"%s%s%s%s%s",content_page,content_host,content_type,content_len,buf);  
    send(sockfd,content,strlen(content),0);  
}
//把数据封装成http格式，并且通过post方式发送可燃气体值数据
void sendDataGas(uint8_t *buf,int length)
{
    printf("buf:%s\n",buf);
    char page[] = "Gas";  
    char content[4096];  
    char content_page[50];  
    char content_host[50];
    char content_len[50]; 
    sprintf(content_page,"POST /%s HTTP/1.1\r\n",page);       
    sprintf(content_host,"HOST: %s:%d\r\n",ip,port);  
    char content_type[] = "Content-Type: application/x-www-form-urlencoded\r\n";   
    sprintf(content_len,"Content-Length: %d\r\n\r\n",strlen(buf));  
    sprintf(content,"%s%s%s%s%s",content_page,content_host,content_type,content_len,buf); 
    send(sockfd,content,strlen(content),0);  
}

一个HTTP请求报文由请求行（request line）、请求头部（header）、空行和请求数据4个部分组成，下图给出了请求报文的一般格式。
    /*2.请求头部
    /Host：请求的主机名，允许多个域名同处一个IP地址，即虚拟主机。
    */
    sprintf(content_host,"HOST: %s:%d\r\n",ip,port);  

 /*4.请求数据
    /请求数据不在GET方法中使用，而是在POST方法中使用。
    /POST方法适用于需要客户填写表单的场合。与请求数据相
    /关的最常使用的请求头是Content-Type和Content-Length。
    */
    char content_type[] = "Content-Type: application/x-www-form-urlencoded\r\n";  
    sprintf(content_len,"Content-Length: %d\r\n\r\n",strlen(buf));  
    sprintf(content,"%s%s%s%s%s",content_page,content_host,content_type,content_len,buf);  
    send(sockfd,content,strlen(content),0);  

}

```

**服务端：**

设计方案
服务端用maven整合springboot框架搭建项目，数据库使用mysql，代码主要分为四个层，POJO层、Dao层、Service层、Controller层。

POJO层：该层包含三个实体类，温湿度封装类、光照值封装类、可燃气体封装类；
Dao层：负责与数据库交互，提供数据源；
Service层：该层主要负责管理事务；
Controller层；负责提供json数据接口。

**主要代码：**

```
@RequestMapping(value = "/findAllShidu")
public Map<String,Object> findAllShidu(){
    Map<String,Object> wenduMap = new HashMap<String,Object>();
    List<Wendu> list = wenduService.findAllShidu();
    wenduMap.put("data",list);
    return wenduMap;
}
@RequestMapping(value = "/addWendu")
public Map<String,Object> addWendu(Double wendu,Double shidu) {
    Map<String, Object> wenduMap = new HashMap<String, Object>();
    System.out.print(wendu);
    try {
      wenduService.saveWendu(wendu,shidu);
    } catch (Exception e) {
        wenduMap.put("status", "fail");
        return wenduMap;
    }
    wenduMap.put("status", "success");
    return wenduMap;
}

```

**Web显示端**

主要代码：

```
<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml"
      xmlns:th="http://www.thymeleaf.org">
    <head>
        <meta charset="utf-8">
        <link rel="icon" href="https://static.jianshukeji.com/highcharts/images/favicon.ico">
        <meta name="viewport"  content="width=device-width,initial-scale=1">
        <script src="https://img.hcharts.cn/jquery/jquery-1.8.3.min.js"></script>
        <script src="https://img.hcharts.cn/highcharts/highcharts.js"></script>
        <script src="https://img.hcharts.cn/highcharts/modules/exporting.js"></script>
        <script src="https://img.hcharts.cn/highcharts-plugins/highcharts-zh_CN.js"></script>
    </head>
    <body>
        <div id="container" style="min-width:400px;height:400px"></div>
        <script>
		var wendu=0;
			$(function(){
	getallRepairOrder();
	
	function getallRepairOrder(){
		$.ajax({
			url:"http://localhost:8080/findGas",
			type:"post",
			async:true,
			success:function(data){
				//oOrderContainer.text("");
				//回调
				 wendu=data.data[0][0];			
				//alert(data.data[0][0]);
				}
		});
		setTimeout(getallRepairOrder, 1000);
	
	}
	});
			Highcharts.setOptions({
                global: {
                    useUTC: false
                }
            });
            function activeLastPointToolip(chart) {
                var points = chart.series[0].points;
                chart.tooltip.refresh(points[points.length -1]);
            }
            $('#container').highcharts({
                chart: {
                    type: 'spline',
                    animation: Highcharts.svg, // don't animate in old IE
                    marginRight: 10,
                    events: {
                        load: function () {
                            // set up the updating of the chart each second
                            var series = this.series[0],
                                chart = this;
                            setInterval(function () {
                                var x = (new Date()).getTime(), // current time
                                    y = wendu;
                                series.addPoint([x, y], true, true);
                                activeLastPointToolip(chart)
                            }, 1000);
                        }
                    }
                },
                title: {
                    text: '可燃气体实时数据'
                },
                xAxis: {
                    type: 'datetime',
                    tickPixelInterval: 150
                },
                yAxis: {
                    title: {
                        text: '可燃气体值 %'
                    },
                    plotLines: [{
                        value: 0,
                        width: 1,
                        color: '#808080'
                    }]
                },
                tooltip: {
                    formatter: function () {
                        return '<b>' + this.series.name + '</b><br/>' +
                            Highcharts.dateFormat('%Y-%m-%d %H:%M:%S', this.x) + '<br/>' +
                            Highcharts.numberFormat(this.y, 2);
                    }
                },
                legend: {
                    enabled: false
                },
                exporting: {
                    enabled: false
                },
                series: [{
                    name: '初始数据',
                    data: (function () {
                        // generate an array of random data
                        var data = [],
                            time = (new Date()).getTime(),
                            i;
                        for (i = -19; i <= 0; i += 1) {
                            data.push({
                                x: time + i * 1000,
                                y: 0
                            });
                        }
                        return data;
                    }())
                }]
            }, function(c) {
                activeLastPointToolip(c)
            });
	
        </script>
    </body>

</html>
```

**客户端可视化展示部分：**

主要流程：
![这里写图片描述](http://img.blog.csdn.net/20170614235847216?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAwNzA0MzM=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

数据访问：

```
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
                yar.append(float(y)
    return xar,yar
参数：
url：访问数据的接口。
对接口进行封装：
wendu_url = 'http://localhost:8080/findAllWendu'
shidu_url = 'http://localhost:8080/findAllShidu'
gas_url = 'http://localhost:8080/findGas'
sun_url='http://localhost:8080/findGuangzhao'
data = [('温度', wendu_url), ('湿度', shidu_url), ('可燃气', gas_url),('光照',sun_url)]
date：对其他日期进行过滤，可以查询特定日期下的数据变化情况。
对数据绘图：
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
for xl in xlabels:
    xl.set_rotation(45)
drawPic.a.set_title('Demo: DATA Display')
drawPic.a.set_xlabel(sampleCount.date())
drawPic.canvas.show()

```
图形界面：

```
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
#单选框，选择可以传入某一接口
inputEntry=Entry(root)
inputEntry.grid(row=2,column=1)
inputEntry.insert(0,'2017-6-12')
Button(root,text='画图',command=drawPic).grid(row=2,column=2,columnspan=3)
#command=drwaPic 按下Button执行drwaPic函数绘图

```

**4 系统调试及运行测试截图**

![这里写图片描述](http://img.blog.csdn.net/20170615000014023?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAwNzA0MzM=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
上图是网关运行截图
![这里写图片描述](http://img.blog.csdn.net/20170615000116218?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAwNzA0MzM=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
上图是服务器运行截图
![这里写图片描述](http://img.blog.csdn.net/20170615000156110?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAwNzA0MzM=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
上图是web显示截图
![这里写图片描述](http://img.blog.csdn.net/20170615000226291?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAwNzA0MzM=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
上图是python可视化窗口端截图

**5 结束语**

提出了一种基于ZigBee的数据采集系统的设计方案. 在实验中搭建了温度采集的ZigBee网络, 通信良好, 并实现了该系统的功能. ZigBee网络的覆盖范围一般为几十米, 但是可以通过网关与其他网络(例如WiFi、以太网)进行连接, 从而达到扩大网络覆盖范围或远程监控的目的, 这将使得ZigBee优良特性得到更好的体现, 使得它有更广阔的应用前景.