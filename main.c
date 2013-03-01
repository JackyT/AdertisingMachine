#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 256
#define CM_START "START"
#define CM_CLOSE "CLOSE"
#define CM_RESTART "RESTART"
#define CM_FULLSIZE "FULLSIZE"
#define CM_ASSIZE "ASSIZE"
#define CM_DEL "DELETE"
#define CM_GET "GET"

volatile int qt_pid;
volatile int remote_status;

static void start_remote(char *cm,char *width,char *height,char *ip);
static int close_remote(void);
static int getfile(char *path,int socket_fd);
static void deletefile(char *path,int socket_fd);

//--------------------------------------------------------------------
// Main function
//--------------------------------------------------------------------

int main(int argc, char **argv)
{
	if(argc != 2){
		fprintf(stdout,"Note:./server <IP>\n");	
		return 0;
	}else{
		fprintf(stdout,"default set: port 8000,backlog 10\n");	
	}
	remote_status = 0;
	int listening_socket;
	unsigned short port = 8000;
	int backlog = 10;
	start_remote(CM_START,NULL,NULL,argv[1]);

	//------------------------------------------------------------------
	// step 1, create socket
	//------------------------------------------------------------------
	//int socket(int domain, int type, int protocol);
	if ((listening_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		// failed
		fprintf(stderr, "[%d]Create new TCP socket failed: %s\n", getpid(), strerror(errno));
		exit(1);
	}

	fprintf(stdout, "[%d]New TCP socket created, listening_socket = %d.\n", getpid(), listening_socket);

	//------------------------------------------------------------------
	// Set SO_REUSEADDR & SO_REUSEPORT options
	//------------------------------------------------------------------
	int optval;

	optval = 1;

	//int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
	setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

#ifdef SO_REUSEPORT
	optval = 1;
	setsockopt(listening_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
#endif

	//------------------------------------------------------------------
	// step 2, bind
	//------------------------------------------------------------------
	struct sockaddr_in local_ipv4_address;	// IPv4

#if 0
	struct sockaddr_in
	{
		sa_family_t sin_family;
		in_port_t sin_port;
		struct in_addr sin_addr;
		unsigned char sin_zero[8];
	};
#endif

	memset(&local_ipv4_address, 0, sizeof(local_ipv4_address));

	local_ipv4_address.sin_family = AF_INET;	// IPv4
	local_ipv4_address.sin_port = htons(port);

	//local_ipv4_address.sin_addr.s_addr = INADDR_ANY;
	// int inet_pton(int af, const char *src, void *dst);
	inet_pton(AF_INET, argv[1], &local_ipv4_address.sin_addr);

	//int bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen);
	if (bind(listening_socket, (struct sockaddr *) &local_ipv4_address, sizeof(local_ipv4_address)) < 0)
	{
		fprintf(stderr, "[%d]Bind to %s:%d failed: %s\n", getpid(), argv[1], port, strerror(errno));
		close(listening_socket);
		exit(1);
	}

	fprintf(stdout, "[%d]Bound to %s:%d successfully.\n", getpid(), argv[1], port);

	//------------------------------------------------------------------
	// step 3, listen
	//------------------------------------------------------------------
	//int listen(int sockfd, int backlog);
	if (listen(listening_socket, backlog) < 0)
	{
		// failed
		fprintf(stderr, "[%d]Listen on %s:%d failed: %s\n", getpid(), argv[1], port, strerror(errno));
		close(listening_socket);
		exit(1);
	}

	fprintf(stdout, "[%d]Listen on %s:%d successfully.\n", getpid(), argv[1], port);
	fprintf(stdout, "[%d]Waiting for incomming connection ...\n", getpid());


	//------------------------------------------------------------------
	// Main loop
	//------------------------------------------------------------------
	int new_connected_socket;
	struct sockaddr_in peer_ipv4_address;
	socklen_t peer_ipv4_address_length;
	char peer_ipv4_address_string[] = "ddd.ddd.ddd.ddd";
	char buffer[BUFFER_SIZE];
	while(1)
	{
		// step 4, accept, create a new_connected_socket, original socket still listen
		// value-result parameter
		peer_ipv4_address_length = sizeof(peer_ipv4_address);
		if ((new_connected_socket = accept(listening_socket, (struct sockaddr *) &peer_ipv4_address, &peer_ipv4_address_length)) < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			fprintf(stderr, "[%d]accept() failed: %s\n", getpid(), strerror(errno));
			break;
		}
		else
		{
			inet_ntop(AF_INET, &peer_ipv4_address.sin_addr, peer_ipv4_address_string, sizeof(peer_ipv4_address_string));
			fprintf(stdout, "[%d]Accepted a new connection %d from %s:%d.\n", getpid(), new_connected_socket, peer_ipv4_address_string, ntohs(peer_ipv4_address.sin_port));

			ssize_t size = recv(new_connected_socket,buffer,sizeof(buffer),0);
			if(size == -1){
				fprintf(stderr,"recv failed.%s\n",strerror(errno));	
				close(new_connected_socket);
				break;
			}
			fprintf(stdout,"server:%s\n",buffer);
			if(strncmp(buffer,CM_START,sizeof(CM_START)) == 0){
				if(remote_status == 0)
					start_remote(buffer,NULL,NULL,argv[1]);	
			}else if(strncmp(buffer,CM_FULLSIZE,sizeof(CM_FULLSIZE)) == 0){
				if(remote_status == 1)
					close_remote();	
				start_remote(buffer,NULL,NULL,argv[1]);
			}else if(strncmp(buffer,CM_CLOSE,sizeof(CM_CLOSE)) == 0){
				if(remote_status == 1)
					close_remote();
			}else if(strncmp(buffer,CM_RESTART,sizeof(CM_RESTART)) == 0){
				if(remote_status == 1)
					close_remote();
				start_remote(CM_START,NULL,NULL,argv[1]);
			}else{
				char *w = strchr(buffer,'|');
				if(w != NULL){
					*w++ = '\0';
					if(strncmp(buffer,CM_ASSIZE,sizeof(CM_ASSIZE)) == 0){
						char *h = strchr(w,'|');	
						*h++ = '\0';
						if(remote_status == 1)
							close_remote();
						start_remote(buffer,w,h,argv[1]);
					}
				}else{
					char *p = strchr(buffer,'&');
					if(p != NULL){
						*p++ = '\0';
						if(strncmp(buffer,CM_GET,sizeof(CM_GET)) == 0){
							int ret = getfile(p,new_connected_socket);
							if(ret == -1)
								fprintf(stderr,"get file failed.\n");	
						}else if(strncmp(buffer,CM_DEL,sizeof(CM_DEL)) == 0)
							    deletefile(p,new_connected_socket);
					}
				}
			}
			//write(STDOUT_FILENO,buffer,size);
			close(new_connected_socket);
		}
	}
	close(listening_socket);
	return 0;
}
void deletefile(char *path,int socket_fd)
{
	pid_t pid;
	if((pid = fork()) == -1){
		fprintf(stderr,"fork failed,%s\n",strerror(errno));	
		return;
	}else if(pid == 0){
		chdir("./remote");	
		if(unlink(path) == -1){
			fprintf(stderr,"unlink file %s failed,%s\n",path,strerror(errno));	
			exit(-1);
		}
		exit(0);
	}
	fprintf(stdout,"deleting file:%s...\n",path);
	wait(NULL);
	fprintf(stdout,"Done\n");
}
int getfile(char *path,int socket_fd)
{
	pid_t pid;
	if((pid = fork()) == -1){
		fprintf(stderr,"fork failed,%s\n",strerror(errno));	
		return -1;
	}else if(pid == 0){
		chdir("./remote");	
		int fd = open(path,O_RDWR | O_CREAT | O_TRUNC,0664);	
		if(fd == -1){
			fprintf(stderr,"open file %s failed\n",path);	
		}else{
			ssize_t size;
			char buffer[1024];
			send(socket_fd,"ready",sizeof("ready"),0);
			while(1){
				size = recv(socket_fd,buffer,sizeof(buffer),0);
				if(size == -1){
					if(errno == EINTR)
						continue;
					fprintf(stderr,"recv file failed.\n");	
					return -1;
				}else if(size == 0)
					break;
				write(fd,buffer,size);
			}
			close(fd);
		}
	}
	fprintf(stdout,"geting file %s...\n",path);
	wait(NULL);
	fprintf(stdout,"download file succeed.\n");
	return 0;
}
void start_remote(char *cm,char *width,char *height,char *ip)
{
	pid_t pid;
	if((pid = fork()) == -1){
		fprintf(stderr,"fork failed,%s\n",strerror(errno));	
		exit(-1);
	}else if(pid == 0){ //child process
		chdir("./remote");
		if(strncmp(cm,CM_START,sizeof(CM_START)) == 0){ //CM_START
			execl("remote","./remote",ip,NULL);
			fprintf(stderr,"execl failed,%s\n",strerror(errno));	
			exit(-2);
		}else if(strncmp(cm,CM_FULLSIZE,sizeof(CM_FULLSIZE)) == 0){
			execl("remote","./remote","FULL",ip,NULL);
			fprintf(stderr,"execl failed,%s\n",strerror(errno));	
			exit(-2);
		}else if(strncmp(cm,CM_ASSIZE,sizeof(CM_ASSIZE)) == 0){
			execl("remote","./remote",width,height,ip,NULL);
			fprintf(stderr,"execl failed,%s\n",strerror(errno));	
			exit(-2);
		}
	}
	qt_pid = pid;
	remote_status = 1;
}
int close_remote(void)
{
	if(remote_status == 0 || qt_pid == 0)
		return 0;
	if(kill(qt_pid,SIGKILL) == -1){
		fprintf(stderr,"kill process:%d failed,%s\n",qt_pid,strerror(errno));	
		return -1;
	}
	remote_status = 0;
	return 0;
}
