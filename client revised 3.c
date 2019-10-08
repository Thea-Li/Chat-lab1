	//client skel
	//./The client program is very similar to the monitor program, except that the client uses a TCP/IP socket to connect to the server, which only requires one (bidirectional)le descriptor to communicate to the server. The client must create a new TCP/IP socket(), and then call connect() to attach it to the server's socket. The socket must bind to the localhost address, and use the same default TCP port as specied in the server. To resolve the localhost name into an IP address, you should use the gethostbyname() function. The TCP port number should also be able to be specied by using the same "-p" ag as the server.
	//From there, the code should be nearly identical to the monitor process. The reference implementation of the client contains only a main() function, and is about 90 lines of code.

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <errno.h>
	#include <string.h>
	#include <netdb.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>


	
	#define MAXDATASIZE 100 // max number of bytes


	int main(int argc, char **argv){
		int sockfd;
		char buf[MAXDATASIZE];
		struct addrinfo hints, *servinfo;
		int rv;
		int portno = 6666;
		
		
				
		//struct sockaddr_in myaddr;

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_INET; 
		hints.ai_socktype = SOCK_STREAM;
		//finsish this part :  memcpy(&hints.sin_addr.s_addr)
		
		// no need myaddr 
		//struct sockaddr_in myaddr;


		//myaddr.sin_family = AF_INET;
		//myaddr.sin_port = htons(portno);
		//myaddr.sin_addr.s_addr = INADDR_ANY;


		//getopt
		int c;
		int hflag = 0, pflag = 0;
		

		while  ((c = getopt(argc, argv, "hp:")) != -1) { // "h:p:"
			switch (c) {
				case 'h' :
					//hostname = strdup(optarg)
					printf("usage: ./server [-h] [-p port #]\n");
					printf("-h - this help message\n");
					printf("-p # - the port to use when connecting to the server\n");
					break;
				case 'p' :
				if (portno){
					portno  = atoi(optarg);
					printf("Connecting to server.....\n");
					break;
					}
					
		}
				
		if ((rv = getaddrinfo("senna.rhodes.edu", optarg, &hints, &servinfo)) != 0) {
			exit(1);
		}

		if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) { // < 0
			perror("client:socket");
			exit(1);
		}
		
//		if ((rv = gethostbyname(hostname)) == NULL){
//			perror("gethostbyname");
//			exit(1);
//		}

				
		if(connect(sockfd, servinfo->ai_addr, servinfo -> ai_addrlen)== -1){

			close(sockfd);
			perror("client:connect");
			exit(1);
		}


//		if (p == NULL) {
//			fprintf(stderr, "client:failed to connect\n");
//			exit(2);
//		}

		//CLient read from standard in  and write to realy server,
		// read from relay server and write to standard out
		//STDIN_FILENO
		//STDOUT_FILENO
	


		while(1){
			int c_bytesread;
			
			c_bytesread = read(STDIN_FILENO, buf, sizeof(buf));
			
			if (c_bytesread == -1){
				perror("console read ");
				exit(1);
			}else if (c_bytesread == 0){
				break;
				//break if read returns 0;
			}
			
			write(sockfd, buf, c_bytesread);
			
			c_bytesread = read(sockfd, buf, sizeof(buf));
			if (c_bytesread == -1){
				perror("server read ");
				exit(1);
			}else if (c_bytesread == 0){
				break;
				//break if read returns 0;
			}
			
			write(STDOUT_FILENO, buf, c_bytesread);	
			
			}


		freeaddrinfo(servinfo); // release servinfo space

		printf("hanging up \n");
		close(sockfd);
		
		
		return 0;
		}
		//output from read
		//close if nothing
		//close the unused pipe before use


	}
