#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h> 
#include <string.h>
#include <netdb.h>
#include "cssl.h"

//全局变量这边设置设备地址,IP，端口号，以及sockfd
char* dev_path;
char* ip = NULL;
int port;
int sockfd;
//把数据封装成http格式，并且通过post方式发送温湿度数据
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
//回调函数，打印串口收到的数据的信息，在这个方法中对串口收到的数据进行解析，并且调用通过socket发送数据给服务器
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

int main( int argc, char **argv ){	
	dev_path = (char*)argv[1];
	ip  = (char*)argv[2];
	port = atoi(argv[3]);
	char buffer[64];
	if( argc < 4 ){
		fprintf( stderr,"usage: %s dev_path,ip and post\n",argv[0] );
		exit(1);
	}
	//serial part串口部分
    	cssl_t *ser;     
    	cssl_start();
    	ser=cssl_open(dev_path,callback,0,38400,8,0,1);   
    	if (!ser){
        		printf("%s\n",cssl_geterrormsg());
    	}
    	printf("Serial connection successful, DEV = %s\n", dev_path);
    //create socket
    //建立socket 建立一个sock连接
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

	//receive from service :
	memset( buffer,0,sizeof(buffer) );
	ssize_t n;
	while(1){
		if( (n = read(sockfd,buffer,1024)) > 0) {
			printf("receive from service :");
			puts(buffer);
			printf("\n");
			cssl_putstring(ser,buffer);
			printf("the serial receive the commend from service  !\n");
		}		
	}
   	for(;;);
	close(sockfd);
   	return 0;
}
