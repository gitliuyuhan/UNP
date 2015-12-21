/*======================================================
    > File Name: myclient.c
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2015年03月20日 星期五 12时36分21秒
 =======================================================*/

#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<assert.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<libgen.h>

#define  BUFFER_SIZE    512

int main(int argc,char** argv)
{
	if(argc <= 2)
	{
		printf("usage: %s ip_address port_number\n",basename(argv[0]));
		return 1;
	}
	const char*   ip = argv[1];
	int   port = atoi(argv[2]);

	struct sockaddr_in    server_address;
	bzero(&server_address,sizeof(server_address));
	server_address.sin_family = AF_INET;
	inet_pton(AF_INET,ip,&server_address.sin_addr);
	server_address.sin_port = htons(port);
    //创建socket
	int sock = socket(PF_INET,SOCK_STREAM,0);
    //连接服务器
	if(connect(sock,(struct sockaddr*)&server_address,sizeof(server_address)) == -1)
	{
        printf("connect failed\n");
        close(sock);
        exit(1);
	}

    char     buf[BUFFER_SIZE] = "name:Mary; passwd:1234567";
    //发送登录信息
    int ret = send(sock,buf,strlen(buf),0);
    if(ret < 0)
    {
        printf("send fail!\n");
        close(sock);
        exit(1);
    }
    printf("已发送登录信息\n");

    //设置超时时间
    struct timeval timeout;
    timeout.tv_sec  = 3;
    timeout.tv_usec = 0;
    socklen_t  len = sizeof(timeout);
    setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,&timeout,len);

    //接收服务器回应，循环尝试接收3次
    memset(buf,0,sizeof(buf));
    int  i;
    for(i=0; ;i++)
    {
        ret = recv(sock,buf,BUFFER_SIZE,0);
        if(ret ==0)
        {
            printf("服务器断开连接\n");    
            break;   
        }
        else if(ret == -1 )
        {
            //非阻塞返回EAGAIN
            if( errno == EWOULDBLOCK )
            {
                printf("第 %d 次超时\n",i+1);
                if(i+1 == 3)
                {    
                    printf("已经超时 3 次\n");  
                    break;  
                }
                continue;  
            }
            else
            {    
                printf("出现错误\n");  
                break; 
            }
        }
        else
        {       
            printf("成功收到服务器返回的数据:%s\n",buf); 
            break; 
        }
    }
        
	close(sock);
	return 0;
}

