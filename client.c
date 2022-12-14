#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include<sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SA struct sockaddr
#define MAX 80	
#define PORT	 8080
#define MAXLINE 1024

void func(int connfd, char name[MAX])
{
	char buff[MAX];
	char input[MAX];
	char test[MAX];
	int go = 0;
	bzero(input, sizeof(input));
	bzero(buff, sizeof(buff));
	strcpy(test, ": ");
	read(connfd, buff, sizeof(buff));
	printf("%s\n", buff);
	bzero(buff, sizeof(buff));
	scanf("%s", input);
	strncat(buff, input, strlen(input));
	write(connfd, buff, sizeof(buff));
	if (strncmp("no", buff, 2) == 0){
		go = 1;
		printf("declined chat\n");
	}
	// infinite loop for chat
	for (;;) {
			if (go != 0) {
				break;
			}
			bzero(input, sizeof(input));
			bzero(buff, sizeof(buff));
			
			// read the message and copy it in buffer
			read(connfd, buff, sizeof(buff));
			
			// if msg contains "quit" then exit achat.
			if (strncmp(buff, "quit", 4) == 0) {
				printf("Exiting...\n");
				bzero(buff, sizeof(buff));
				break;
			}
			// print buffer which contains the client contents
			printf("%s\n", buff);
			bzero(buff, MAX);
			
			printf("%s : ", name);
			scanf("%s", input);
			if (strncmp(input, "quit", 4) == 0) {
				printf("Exiting...\n");
				bzero(buff, sizeof(buff));
				strcpy(buff, "quit");
				write(connfd, buff, sizeof(buff));
				break;
			}
			strncat(buff, name, strlen(name));
			strncat(buff, test, 2);
			strncat(buff, input, strlen(input));
			// and send that buffer to client
			write(connfd, buff, sizeof(buff));
		
	}
}

void funcTo(int sockfd, char name[MAX])
{
	char input[MAX];
	char buff[MAX];
	char test[MAX];
	int go = 0;
	bzero(test, sizeof(test));
	strcpy(test, ": ");
	bzero(buff, sizeof(buff));
	strcpy(buff, "\ntalk request recived");
	write(sockfd, buff, sizeof(buff));
	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff));
	if(strncmp(buff, "no", 2)==0){
		go = 1;
		printf("chat declined\n");
	}
	for (;;) {
		if(go != 0){
			break;
		}
		bzero(input, sizeof(input));
		bzero(buff, sizeof(buff));
		printf("%s : ", name);
		scanf("%s", input);
		if (strncmp(input, "quit", 4) == 0) {
			printf("Exiting...\n");
			bzero(buff, sizeof(buff));
			strcpy(buff, "quit");
			write(sockfd, buff, sizeof(buff));
			break;
		}
		strncat(buff, name, strlen(name));
		strncat(buff, test, 2);
		strncat(buff, input, strlen(input));
		write(sockfd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		if (strncmp(buff, "quit", 4) == 0) {
			printf("Exiting...\n");
			bzero(buff, sizeof(buff));
			break;
		}
		printf("%s\n", buff);
	}
}

void DieWithError(char *errorMessage)
{
	perror(errorMessage);
	exit(1);
}

struct logs{
	char user[MAX];
	int id;
	int command;
	int port;
	char IP[MAX];
	int size;
	char list[10][MAX];
	int ports[10];
	char IPs[10][MAX];
	struct sockaddr_in	 sr;
	struct sockaddr_in onl[10];
};	
 void pl(struct logs bh){
	int i;
	for(i=0;i<bh.size;i++){
		printf("%d:%s\n",i+1,bh.list[i]);
	}
 }

void login(int soc, struct logs lgs, struct sockaddr_in add ){
	int len = sizeof(add);
	lgs.command = 1;
	sendto(soc, (struct logs*)&lgs, sizeof(lgs), 0, (struct sockaddr *) &add, len);
	printf("sent %s\n", lgs.user);
	recvfrom(soc, (struct logs*)&lgs, sizeof(lgs), 0, (struct sockaddr *) &add, &len);
	printf("%s\n", lgs.user);
}

void logout(int soc, struct logs lgs, struct sockaddr_in add){
	int len = sizeof(add);
	lgs.command = 3;
	sendto(soc, (struct logs*)&lgs, sizeof(lgs), 0, (struct sockaddr *) &add, len);
	recvfrom(soc, (struct logs*)&lgs, sizeof(lgs), 0, (struct sockaddr *) &add, &len);
}

struct logs find(int soc, struct logs lgs, struct sockaddr_in add){
	int len = sizeof(add);
	char input[10];
	struct logs op = lgs;
	bzero(input, 10);
	op.command = 4;
	printf("Please enter the number of the id you wish to connect to: ");
	scanf("%s",input);
	op.id = atoi(input);
	sendto(soc, (struct logs*)&op, sizeof(op), 0, (struct sockaddr *) &add, len);
	recvfrom(soc, (struct logs*)&op, sizeof(op), 0, (struct sockaddr *) &add, &len);
	printf("connecting to %s\n", op.user);
	return op;
}

void lookup(int soc, struct logs lgs, struct sockaddr_in add){
	int len = sizeof(add);
	char input[10];
	struct logs op = lgs;
	bzero(input, 10);
	op.command = 5;
	printf("Please enter the usernames number you wish to lookup: ");
	scanf("%s",input);
	op.id = atoi(input);
	sendto(soc, (struct logs*)&op, sizeof(op), 0, (struct sockaddr *) &add, len);
	recvfrom(soc, (struct logs*)&op, sizeof(op), 0, (struct sockaddr *) &add, &len);
	printf("%s IP address is %s\n", op.user, op.IP);
}

void getlist(int soc, struct logs lgs, struct sockaddr_in add ){
	int len = sizeof(add);
	lgs.command = 2;
	sendto(soc, (struct logs*)&lgs, sizeof(lgs), 0, (struct sockaddr *) &add, len);
	recvfrom(soc, (struct logs*)&lgs, sizeof(lgs), 0, (struct sockaddr *) &add, &len);
	pl(lgs);
}

// Driver code
int main() {
	char input[MAX];
	char nme[MAX];
	char servIP[MAX];
	struct logs test;
	int sockfd, socktcp, connfd, sockmd;
	struct sockaddr_in	 servaddr, cli, tcp, sender;
	
	
	// socket for tcp
	socktcp = socket(AF_INET, SOCK_STREAM, 0);
	if (socktcp == -1){
		DieWithError("Tcp failed");
	}
	sockmd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockmd == -1){
		DieWithError("Tcp failed");
	}
	
	// socket for udp
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DieWithError("socket creation failed");
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&tcp, 0, sizeof(tcp));
	memset(&test.sr, 0, sizeof(test.sr));
	memset(&sender, 0, sizeof(sender));
	
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(20002);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	
	int n, len, lentcp, key = 0;
	len = sizeof(servaddr);
	
	
	bzero(test.user, MAX);
	bzero(nme, MAX);
	memset(&test, 0, sizeof(test));
	bzero(input, MAX);
	bzero(servIP, MAX);
	bzero(test.IP, MAX);
	
	// Get the user id
	printf("Enter Your id: ");
	scanf("%s",input);
	strncat(test.user, input, strlen(input));
	strncat(nme, test.user, strlen(test.user));
	
	// Get the IP Address
	printf("Enter Your IP Address: ");
	bzero(input, MAX);
	scanf("%s",input);
	strncat(servIP, input, strlen(input));
	strncat(test.IP, input, strlen(input));
	
	// get port
	printf("Enter Port number: ");
	scanf("%s",input);
	n = atoi(input);
	test.port = n;
	
	// Filling server information for tcp
	tcp.sin_family = AF_INET;
	tcp.sin_port = htons(n);
	tcp.sin_addr.s_addr = inet_addr(servIP);
	
	if ((bind(socktcp, (SA*)&tcp, sizeof(tcp))) != 0){
		DieWithError("Tcp Socket failed");
	}
	
	test.sr = tcp;
	login(sockfd, test, servaddr);
	
	if (fork() == 0) {
		for (;;){
			if(key != 1){
			bzero(input, MAX);
			printf("Enter a command: ");
			scanf("%s",input);
			// if msg contains "Exit" then server exit and chat ended.
			if (strncmp("help", input, 4) == 0) {
				printf("commands are:\nhelp - gives list of commands\n");
				printf("who - list all users using server\n");
				printf("quit - logout of server and exit program\n");
				printf("talk - talk with someone on the server\n");
				printf("lookup - check someones address\n");
				printf("accept - accept a chat\n");
			}else if(strncmp("who", input, 3) == 0){
				getlist(sockfd, test, servaddr);
			}else if(strncmp("quit", input, 4) == 0){
				logout(sockfd, test, servaddr);
				close(sockfd);
				key = 13;
				exit(1);
				return 0;
			}else if(strncmp("talk", input, 7) == 0){
				test = find(sockfd, test, servaddr);
				sender.sin_family = AF_INET;
				sender.sin_port = htons(test.port);
				sender.sin_addr.s_addr = inet_addr(test.IP);
				if (connect(sockmd, (SA*)&sender, sizeof(sender)) != 0) {
					DieWithError("connect failed");
				}
				funcTo(sockmd, nme);
			}else if(strncmp("accept", input, 6) == 0){
				key = 1;
			}else if(strncmp("lookup", input, 6) == 0){
				lookup(sockfd, test, servaddr);
			}else{
				printf("incorrect input try help.\n");
			}
			}else{
				
			}
		}
	}
	else {
		
		for(;;){
			
			// Now server is ready to listen and verification
			if ((listen(socktcp, 5)) != 0) {
				DieWithError("Listen failed...\n");
			}
			lentcp = sizeof(cli);
			// Accept the data packet from client and verification
			connfd = accept(socktcp, (SA*)&cli, &lentcp);
			if (connfd < 0){
				DieWithError("server accept failed...\n");
			}
			
			if (key == 13){
				exit(1);
			}
			// Function for chatting between client and server
			func(connfd, test.user);
			key = 0;
		}
	}
	
	
	
	
	close(sockfd);
	return 0;
}
