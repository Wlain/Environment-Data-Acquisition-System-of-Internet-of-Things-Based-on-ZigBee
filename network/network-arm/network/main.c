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

char* ip = NULL;
int port;

int sockfd;

void sendDatawenshidu(uint8_t *buf,int length)
{
    printf("buf:%s\n",buf);
    char page[] = "addWendu";  
    char content[4096];  
    char content_page[50];  
    sprintf(content_page,"POST /%s HTTP/1.1\r\n",page);  
    char content_host[50];  
    sprintf(content_host,"HOST: %s:%d\r\n",ip,port);  
    char content_type[] = "Content-Type: application/x-www-form-urlencoded\r\n";  
    char content_len[50];  
    sprintf(content_len,"Content-Length: %d\r\n\r\n",strlen(buf));  
    sprintf(content,"%s%s%s%s%s",content_page,content_host,content_type,content_len,buf);  
    send(sockfd,content,strlen(content),0);  

}
void sendDataguangzhao(uint8_t *buf,int length)
{
    printf("buf:%s\n",buf);
    char page[] = "Guangzhao";  
    char content[4096];  
    char content_page[50];  
    sprintf(content_page,"POST /%s HTTP/1.1\r\n",page);  
    char content_host[50];  
    sprintf(content_host,"HOST: %s:%d\r\n",ip,port);  
    char content_type[] = "Content-Type: application/x-www-form-urlencoded\r\n";  
    char content_len[50];  
    sprintf(content_len,"Content-Length: %d\r\n\r\n",strlen(buf));  
    sprintf(content,"%s%s%s%s%s",content_page,content_host,content_type,content_len,buf);  
    send(sockfd,content,strlen(content),0);  
}
void sendDataGas(uint8_t *buf,int length)
{
    printf("buf:%s\n",buf);
    char page[] = "Gas";  
    char content[4096];  
    char content_page[50];  
    sprintf(content_page,"POST /%s HTTP/1.1\r\n",page);  
    char content_host[50];  
    sprintf(content_host,"HOST: %s:%d\r\n",ip,port);  
    char content_type[] = "Content-Type: application/x-www-form-urlencoded\r\n";  
    char content_len[50];  
    sprintf(content_len,"Content-Length: %d\r\n\r\n",strlen(buf));  
    sprintf(content,"%s%s%s%s%s",content_page,content_host,content_type,content_len,buf);  
    send(sockfd,content,strlen(content),0);  
}

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
	ip  = argv[2];
	port = atoi(argv[3]);
	char buffer[64];
	char httpstring[2048];
	if( argc < 4 ){
		fprintf( stderr,"usage: %s dev_path,ip and post\n",argv[0] );
		exit(1);
	}
	//serial part
    	cssl_t *ser;     
    	cssl_start();
    	ser=cssl_open(argv[1],callback,0,38400,8,0,1);   
    	if (!ser){
        		printf("%s\n",cssl_geterrormsg());
    	}
    	printf("Serial connection successful, DEV = %s\n", argv[1]);
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
	memset( &sockaddr,0,sizeof(sockaddr) );

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons ( atoi (argv[3]) );
	inet_pton(AF_INET,argv[2],&sockaddr.sin_addr.s_addr);
	socklen_t len = sizeof(sockaddr);

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
