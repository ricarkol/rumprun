#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr

#define BUFFER_SIZE 1024

int main(int argc , char *argv[])
{
	int sockfd;
	struct sockaddr_in server;
	char buffer[BUFFER_SIZE];
	int ret;
	
	//Create socket
	sockfd = socket(AF_INET , SOCK_STREAM , 0);
	if (sockfd == -1)
	{
		printf("Could not create socket");
	}
		
	server.sin_addr.s_addr = inet_addr("10.0.0.4");
	server.sin_family = AF_INET;
	server.sin_port = htons( 5000 );

	//Connect to remote server
	if (connect(sockfd , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	//write(sockfd, "asdf", 4); // write(fd, char[]*, len);
	write(sockfd, "GET /\r\n", strlen("GET /\r\n")); // write(fd, char[]*, len);  
	ret = recv(sockfd, buffer, sizeof(buffer), 0);
	//printf("recv()'d %d bytes of data in buf\n", ret);
	//printf(buffer);
	//fflush(stdout);
	//sleep(1);
	close(sockfd);
	printf("bye\n");
	
/*
	write(sockfd, "GET /\r\n", strlen("GET /\r\n")); // write(fd, char[]*, len);  
	bzero(buffer, BUFFER_SIZE);
	
	while(read(sockfd, buffer, BUFFER_SIZE - 1) != 0){
		printf("%s", buffer);
		bzero(buffer, BUFFER_SIZE);
	}
*/
	puts("Connected");
	return 0;
}
