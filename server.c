#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/select.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define MAX_NUM 30
int start_up()
{
	int listen_sock=socket(AF_INET,SOCK_STREAM,0);
	if(listen_sock<0)
	{
		perror("socket");
		exit(1);
	}
	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(8080);
	local.sin_addr.s_addr=htonl(INADDR_ANY);
	int ret=bind(listen_sock,(struct sockaddr*)&local,sizeof(local));
	if(ret<0)
	{
		perror("bind");
		exit(2);
	}
	ret=listen(listen_sock,5);
	if(ret<0)
	{
		perror("listen");
		exit(3);
	}
	return listen_sock;
}
int main()
{
	int listen_sock=start_up();
    struct sockaddr_in remote;
	socklen_t len=sizeof(remote);

	fd_set _read_set;
	int read_fd=0;
	int write_fd=1;
	int array[MAX_NUM];
	int max_fd=listen_sock;
	int i=0;
	for(;i<MAX_NUM;i++)
	{
		array[i]=-1;
	}
	array[0]=listen_sock;

	while(1)
	{
		FD_ZERO(&_read_set);
		for(i=0;i<MAX_NUM;i++)
		{
			if(array[i]>-1)
			{
				FD_SET(array[ i],&_read_set);
				if(array[i]>max_fd)
				{
					max_fd=array[i];
				}
			}
		}
		switch(select(max_fd+1,&_read_set,NULL,NULL,NULL))
		{
			case 0:
				printf("time  out...");
				break;
			case -1:
				printf("failed...");
				break;
			default:
				for(i=0; i<MAX_NUM;i++)
				{
					if(array[ i]!=-1&&array[i]==listen_sock&&FD_ISSET(array[i],&_read_set))
					{
						int new_sock=accept(array[i],(struct sockaddr*)&remote,&len);
						if(new_sock<0)
						{
							perror("accept");  
							exit(2);
						}
						//get new request
						int j=0;
						for(;j<MAX_NUM;j++)
						{
							if (array[j]==-1) 
							{
							 	array[j]=new_sock;
								if(array[j]>max_fd)
								{
									max_fd=array[j];
								}
								break;
							}
							if(j+1==MAX_NUM)
							{
								printf("buf has full...\n");
								close(new_sock);
							}
						}
					}
					else if(array[i]!=-1&&array[i]>-1&&FD_ISSET(array[i],&_read_set))
					{// get data sock
						char buf[1024];
						memset(buf,'\0',sizeof(buf)-1);
						int len=read(array[i],buf,sizeof(buf)-1);
						if(len<0)
						{
							perror("read");
							exit(3);

						}
						else if(len==0)
						{ 
							printf("client close...\n");
							close(array[i]);
							array[i]=-1;
							break;
						}
						else
						{
							buf[len]='\0';
							printf("client say:%s\n",buf);
						}
					}
				}
		}
	}
	close(listen_sock);
	return 0;
}




					




