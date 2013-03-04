#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE	4096

int main(int argc, char **argv)
{
	int fd;
	//unsigned short port = 8000;
	unsigned short port = 9000;

	//port = (unsigned short) strtol(argv[2], NULL, 10);


	//------------------------------------------------------------------
	// step 1, create socket
	//------------------------------------------------------------------
	//int socket(int domain, int type, int protocol);
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "[%d]Create new TCP socket failed: %s\n", getpid(), strerror(errno));
		exit(1);
	}

	fprintf(stdout, "[%d]New tcp socket created, fd = %d\n", getpid(), fd);

	//------------------------------------------------------------------
	// step 2, connect
	//------------------------------------------------------------------
	struct sockaddr_in remote_ipv4_address;	// IPv4

#if 0
	struct sockaddr_in
	{
		sa_family_t sin_family;
		in_port_t sin_port;
		struct in_addr sin_addr;
		unsigned char sin_zero[8];
	};
#endif

	memset(&remote_ipv4_address, 0, sizeof(remote_ipv4_address));

	remote_ipv4_address.sin_family = AF_INET;	// IPv4
	remote_ipv4_address.sin_port = htons(port);	// network byte order
	// int inet_pton(int af, const char *src, void *dst);
	inet_pton(AF_INET, argv[1], &remote_ipv4_address.sin_addr);

	//int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);
	if (connect(fd, (struct sockaddr *) &remote_ipv4_address, sizeof(remote_ipv4_address)) < 0)
	{
		// failed
		fprintf(stderr, "[%d]Connect to remote server %s:%d failed: %s\n", getpid(), argv[1], port, strerror(errno));
		// FIXME: retry some times!
		close(fd);
		exit(1);
	}

	fprintf(stdout, "[%d]Connected to %s:%d successfully.\n", getpid(), argv[1], port);
	//char buf[BUFFER_SIZE] = "START";
	//char buf[BUFFER_SIZE] = "FULLSIZE";
	//char buf[BUFFER_SIZE] = "CLOSE";
	//char buf[BUFFER_SIZE] = "RESTART";
	//char buf[BUFFER_SIZE] = "ASSIZE|1024|768";
	//char buf[BUFFER_SIZE] = "speed_mult 3\n";
	//char buf[BUFFER_SIZE] = "speed_set 1\n";
#if 1 
	//char buf[BUFFER_SIZE] = "DELETE&vedio/David Garrett- -He s a Pirate(泡豆音乐!Paodo.com).flv";
	//char buf[BUFFER_SIZE] = "get status";
	//char buf[BUFFER_SIZE] = "vedio list";
	//char buf[BUFFER_SIZE] = "pt_step 1";
	char buf[BUFFER_SIZE] = "pt_step -1";
	
	//char buf[BUFFER_SIZE] = "set msg&旁白君来啦，这次为大家带来了精彩的MTV和LIVE，请欣赏～～";
	send(fd,buf,strlen(buf)+1,0);	
	//int size;
	//size = recv(fd,buf,BUFFER_SIZE,0);
	//write(STDOUT_FILENO,buf,size);	
#else
	char buf[BUFFER_SIZE] = "GET&picture1/upload.jpg";
	send(fd,buf,strlen(buf)+1,0);	
	int file = open("upload.jpg",O_RDONLY);
	if(file == -1){
		fprintf(stderr,"open file failed.\n");	
	}
	ssize_t size;
	recv(fd,buf,BUFFER_SIZE,0);
	if(strncmp(buf,"ready",sizeof("ready")) == 0){
		fprintf(stdout,"begin send file.\n");	
	}else{
		close(fd);	
		close(file);
		exit(-1);
	}
		
	while(1){
		size = read(file,buf,BUFFER_SIZE);	
		if(size == -1){
			fprintf(stderr,"read file failed.\n");	
			break;
		}else if(size == 0)
			break;
		send(fd,buf,size,0);
	}
	close(file);
#endif
	//------------------------------------------------------------------
	// step 4, close socket
	//------------------------------------------------------------------
	close(fd);

	return 0;
}

