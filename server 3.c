//* server.c - a chat server (and monitor) that uses pipes and sockets

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/ip.h>
#include <getopt.h>
#include <netdb.h>

// constants for pipe FDs
#define WFD 1
#define RFD 0
#define BACKLOG 21

int portno = 6666;
struct addrinfo hints, *servinfo;
int sockfd;
int rv;
struct sockaddr_in their_addr;
socklen_t addr_size;


/*
 * monitor - provides a local chat window
 * @param srfd - server read file descriptor
 * @param swfd - server write file descriptor
 */
void monitor(int srfd, int swfd) {
	// implement me
	char buffer[1000];
	int bytesread = 0;
		
	while(1){
		bytesread = read(srfd, buffer, sizeof(buffer));
		if (bytesread == 0){
			break;
		}
		else if (bytesread == -1){
			perror("Monitor read");
		}
		write(STDOUT_FILENO, buffer, bytesread);
		write(STDOUT_FILENO, "> ", 3);
		bytesread = read(STDIN_FILENO, buffer, sizeof(buffer));				
		write(swfd, buffer, bytesread);
	}
}

/*
 * server - relays chat messages
 * @param mrfd - monitor read file descriptor
 * @param mwfd - monitor write file descriptor
 * @param portno - TCP port number to use for client connections
 */
void server(int mrfd, int mwfd, int portno) {
	// implement me
	memset(&hints, 0, (sizeof(hints)));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//htons for port and htonl for address
	//no ip address while connect
	
	struct sockaddr_in myaddr;

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(portno);
	myaddr.sin_addr.s_addr = INADDR_ANY;
//	printf("port:%d\n",portno);
//		


	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
		perror("server:socket");
		exit(1);
	}

	int val = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));


	if (bind(sockfd, (struct sockaddr*) &myaddr, sizeof(myaddr) )== -1){
		close(sockfd);
		perror("server:bind");	
		exit(1); 
	}

	

//	if (p == NULL) {
//		// whole loop end，to the linked-list end , can't successfully bind bind
//		fprintf(stderr, "failed to bind socket\n");
//		exit(2);
//	}

	// freeaddrinfo(servinfo); // no need this

	if (listen(sockfd, BACKLOG) == -1) {
			perror("listen");
			exit(1);
		}
		

	printf("server: waiting for connections...\n");
	
	addr_size = sizeof their_addr;
	int clientfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

	if (clientfd == -1) {
		perror("accept");
		exit(1);
	}
		
		//Relay server read from client and write to monitor
		// read from monitor and write to client
	char buffer[1000];
	int s_bytesread = 0;
		
	while (1){
		s_bytesread = read(clientfd, buffer, sizeof(buffer));
		
		if (s_bytesread == 0){
			break;
		}
		else if (s_bytesread == -1){
			perror("Server read");
		}
		write(mwfd, buffer, s_bytesread);
		s_bytesread = read(mrfd, buffer, sizeof(buffer));
//		printf("relay received from monitor %s\n",buffer);
//		printf("Fd number %d",mrfd);
		write(clientfd, buffer, s_bytesread);					
	}
	//after loop  need to close file descriptors
	close(clientfd);
	close(sockfd);
	close(mrfd);
	close(mwfd);
}

//getup

int main(int argc, char **argv) {
	// implement me
	int c;
	int hflag = 0, pflag = 0;
		
		while  ((c = getopt(argc, argv, "hp:")) != -1) {
				switch (c) {
					case 'h' :
						printf("usage: ./server [-h] [-p port #]\n");
						printf("-h - this help message\n");
						printf("-p # - the port to use when connecting to the server\n");
						break;
					case 'p' :
						portno  = atoi(optarg);
						printf("Connecting to client on port %d.....\n",portno);
						break;
					}
//					printf("got here\n");

				}
				

		pid_t pid;
		int MSfd[2];
		int SMfd[2];

		if (pipe(MSfd) == -1) {
			perror("pipe problem MS");
			exit(1);
		}
		if (pipe(SMfd) == -1) {
			perror("pipe problem SM");
			exit(1);
		}

		pid = fork();
		if (pid == -1) {
			perror("fork error");
			exit(1);
		} else if (pid == 0) {
			// child process
//			printf("get monitor\n");
//			printf("MSfd , %d ",MSfd[WFD]);//4
//			printf("SMfd , %d ",SMfd[RFD]);//5
//			
			close(MSfd[RFD]);
			close(SMfd[WFD]);
			monitor(SMfd[RFD],MSfd[WFD]);
			close(MSfd[RFD]);
			close(SMfd[WFD]);

		} else {
//			printf("get server\n");
//			
//			printf("MSfd , %d ",MSfd[RFD]);// 3
//			printf("SMfd , %d ",SMfd[WFD]);//6
			// parent process
			close(SMfd[RFD]);
			close(MSfd[WFD]);
			server(MSfd[RFD],SMfd[WFD],portno);
			close(SMfd[WFD]);
			close(MSfd[RFD]);

			wait(NULL);
		}
	

	return 0;
}