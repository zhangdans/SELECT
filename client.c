
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/select.h>
#include<netinet/in.h>
#include<arpa/inet.h>
int main()
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("sock");
		exit(1);
	}
	struct sockaddr_in remote;
	remote.sin_family=AF_INET;
	remote.sin_port=htons(8080);
	inet_aton("127.0.0.1",&remote.sin_addr);
	int ret=connect(sock,(struct sockaddr*)&remote,sizeof(remote));
	if(ret<0)
	{
		perror("connect");
		exit(2);
	}
	
	fd_set _read_set;
	fd_set _write_set;
	int read_fd=0;
	int write_fd=1;
	int max_fd=sock;
	max_fd=sock>read_fd?sock:read_fd;
	while(1)
	{
		FD_ZERO(&_read_set);
		FD_ZERO(&_write_set);

		FD_SET(read_fd,&_read_set);
		FD_SET(sock,&_write_set);
		switch(select(max_fd+1,&_read_set,&_write_set,NULL,NULL))
		{
			case 0:
				printf("time out\n");
				break;
			case -1:
				printf("failed...\n");
				break;
     		default:
				if(FD_ISSET(read_fd,&_read_set))
				{
					char buf[1024];
					memset(buf,'\0',sizeof(buf));
				    int len=read(read_fd,buf,sizeof(buf)-1);
					if(len>0)
					{
						buf[len]='\0';
					}
					if(FD_ISSET(sock,&_write_set))
					{
						send(sock,buf,strlen(buf),0);
					}
				}
				break;
		}
	}
	return 0;
}



		

