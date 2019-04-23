#include <stdio.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <arpa/inet.h>
#include <signal.h> 
#define PORT 12342

static volatile sig_atomic_t counter = 1;

void sigintHandler(int sig_num) 
{ 
    /* Reset handler to catch SIGINT next time. 
       Refer http://en.cppreference.com/w/c/program/signal */
	//signal(SIGINT, sigintHandler); 
	printf("Closing client side connection \n"); 
	counter=0;
}    
void main(int argc, char const *argv[]) 
{ 
    signal(SIGINT, sigintHandler);
    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char hello[10]; 
    char buffer[1024] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        //return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        //return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        //return -1; 
    } 

    while(counter){
	//printf("Character to be sent from client");
	//scanf("%c",&hello);
	fgets(hello,sizeof(hello),stdin);    	
	send(sock , &hello , sizeof(hello) , 0 ); 
    	valread = read( sock , buffer, 1024); 
    	printf("%s\n",buffer ); 
    }
    //send(sock , &hello , 1 , 0 ); 
    //valread = read( sock , buffer, 1024); 
    //printf("%s\n",buffer );
   if(close(sock)==-1)
	printf("Connection could not be closed appropriately");
   exit(0); 
   //return 0; 
} 

