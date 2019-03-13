#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<poll.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<sys/un.h>
#include<limits.h>
#include<signal.h>


#define POLL_SIZE 10
///#define SOCKET_PATH "web/server_socket.sock"
#define PORT 12342
#define TRUE 1
#define FALSE 0

int main(){
	int opt = TRUE;
	int numfds=0;	
	struct pollfd sockets_polled[POLL_SIZE];
	memset(sockets_polled,'\0',sizeof(sockets_polled));
	int i=0;
	/*	
	for(i=0;i<POLL_SIZE;i++){
		sockets_polled[i].fd=-1; //polling is ignored for uninitialized sockets
	}*/ //This only works if the polling has to be called just once
	int listening_socket, client_sockfd;
    	int server_len, client_len;
    	struct sockaddr_in server_address;
    	struct sockaddr_in client_address;
	//Initializing a listening socket	
/*	
	unlink(SOCKET_PATH);
    	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
 	
 	
    	server_address.sun_family = AF_INET;
    	strcpy(server_address.sun_path, SOCKET_PATH);
    	server_len = sizeof(server_address);
	 
 	
    	bind(server_sockfd, (struct sockaddr *)&server_address, (socklen_t)server_len);
    	listen(server_sockfd, 6);	 
*/
	if( (listening_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    	{   
        	perror("socket failed");   
        	exit(EXIT_FAILURE);   
    	}   
	if( setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,sizeof(opt)) < 0 ){   
        	perror("setsockopt");   
        	exit(EXIT_FAILURE);   
    	}
	//type of socket created  
    	server_address.sin_family = AF_INET;   
    	server_address.sin_addr.s_addr = INADDR_ANY;   
    	server_address.sin_port = htons( PORT );
	
	//binding the listening socket to port 8888

	if (bind(listening_socket, (struct sockaddr *)&server_address, sizeof(server_address))<0)   
    	{   
        	perror("bind failed");   
        	exit(EXIT_FAILURE);   
    	}
	printf("Listener on port %d \n", PORT);
	
	//listening to max of 5 clients
	if (listen(listening_socket, 5) < 0){   
        	perror("listen");   
        	exit(EXIT_FAILURE);   
    	}
	
	//polling the listening socket
	sockets_polled[0].fd=listening_socket;
	sockets_polled[0].events=POLLIN;
         
 	server_len=sizeof(server_address);
	puts("Waiting for connections ...");
	numfds++;
	while(TRUE){
		int fd_index;
		char ch;
		int act=0;
		int num_read;
		int count;
		printf("Waiting for client (10 total)...\n");
		act=poll(sockets_polled, numfds, -1);
		printf("Activity detected from %d sockets\n",act);
		for(fd_index=0;fd_index<numfds;fd_index++){
			if(sockets_polled[fd_index].revents & POLLIN){
				if(fd_index==0){
					client_len = sizeof(client_address);
                    			client_sockfd = accept(listening_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_len);
					//count=0;
					printf("Connection accepted\n");					
					/*while(count<POLL_SIZE){
						count++;						
						if(sockets_polled[count].fd==-1){
							sockets_polled[count].fd=client_sockfd;
							sockets_polled[count].events=POLLIN;
						}
					}*/
					sockets_polled[numfds].fd=client_sockfd;
					sockets_polled[numfds].events=POLLIN;
					numfds++;
				}
				else{
					ioctl(sockets_polled[fd_index].fd, FIONREAD, &num_read);
					
					if(num_read==0){ //client has hung up
						printf("client has closed the connection\n");
						//close(sockets_polled[fd_index].fd);
						//sockets_polled[fd_index].fd=-1; //so that polling is ignored till next connection
						//sockets_polled[fd_index].events=0;
						if((numfds-1)==fd_index)
							numfds--; //if the last client leaves
						else{
							sockets_polled[fd_index].fd=sockets_polled[numfds-1].fd;
							sockets_polled[fd_index].events=sockets_polled[numfds-1].events;//as new connections are added in the back
							numfds--;
						}
						printf("No. of active clients- %d\n",(numfds-1));
					}
					else{//echoing a character back
						read(sockets_polled[fd_index].fd, &ch, 1);
						printf("Serving a client\n");
						if(ch>='a'&& ch<='z')
							ch=ch-32;
						else if(ch>='A'&& ch<='Z')
							ch=ch+32;
						write(sockets_polled[fd_index].fd, &ch, 1);					
					}
				}			
			}
			/*else if(sockets_polled[fd_index].revents & POLLHUP){
				printf("client has closed the connection\n");
				//close(sockets_polled[fd_index].fd);
				if((numfds-1)==fd_index)
					continue;
				else{
					sockets_polled[fd_index].fd=sockets_polled[numfds-1].fd;
					sockets_polled[fd_index].events=sockets_polled[numfds-1].events;				
				}
				numfds--;
			}
			else if(sockets_polled[fd_index].revents & POLLNVAL){
				printf("client has closed the connection\n");
				//close(sockets_polled[fd_index].fd);
				if((numfds-1)==fd_index)
					continue;
				else{
					sockets_polled[fd_index].fd=sockets_polled[numfds-1].fd;
					sockets_polled[fd_index].events=sockets_polled[numfds-1].events;				
				}
				numfds--;		
		}*/
	}
}
}
