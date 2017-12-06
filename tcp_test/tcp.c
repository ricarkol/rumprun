#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr

#define BUFFER_SIZE 1024

int main(int argc , char *argv[])
{
	int socket_desc;
	struct sockaddr_in server;
	char buffer[BUFFER_SIZE];
	int ret;
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
		
	server.sin_addr.s_addr = inet_addr("10.0.0.4");
	server.sin_family = AF_INET;
	server.sin_port = htons( 80 );

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	//write(socket_desc, "asdf", 4); // write(fd, char[]*, len);
	write(socket_desc, "GET /\r\n", strlen("GET /\r\n")); // write(fd, char[]*, len);  
	//sleep(1);
	//ret = read(socket_desc, buffer, 99);
	printf("bye\n");
	
/*
	write(socket_desc, "GET /\r\n", strlen("GET /\r\n")); // write(fd, char[]*, len);  
	bzero(buffer, BUFFER_SIZE);
	
	while(read(socket_desc, buffer, BUFFER_SIZE - 1) != 0){
		printf("%s", buffer);
		bzero(buffer, BUFFER_SIZE);
	}
*/
	puts("Connected");
	return 0;
}
