// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
	
#define MAX 80	
#define PORT	 8080
#define MAXLINE 1024
	
	
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

struct logs result(struct logs first, struct logs second){
	struct logs answ = second;
	answ.size = first.size;
	for(int i = 0; i<answ.size; i++){
		strncat(answ.list[i], first.list[i], strlen(first.list[i]));
		strncat(answ.IPs[i], first.IPs[i], strlen(first.IPs[i]));
		answ.ports[i] = first.ports[i];
		answ.onl[i] = first.onl[i];
	}
	return answ;
}

struct logs operate(struct logs sub, int cap){
	struct logs op = sub;
	if(op.command == 1){
		printf("%s has logged in to the server.\n", op.user);
		if(cap == 0){
			strncat(op.list[0], op.user, strlen(op.user));
			strncat(op.IPs[0], op.IP, strlen(op.IP));
			op.ports[0] = op.port;
			op.onl[0] = op.sr;
		}
		else{
			strncat(op.list[cap], op.user, strlen(op.user));
			strncat(op.IPs[cap], op.IP, strlen(op.IP));
			op.ports[cap] = op.port;
			op.onl[cap] = op.sr;
		}
		op.size += 1;
		strcpy(op.user, "You have logged onto the server.");
	}else if(op.command == 2){
		printf("%s has asked who is on.\n", op.user);
		printf("sending %s the list.\n", op.user);
	}else if(op.command == 3){
		printf("%s has logged out of the server.\n", op.user);
		for(int i = 0; i<cap; i++){
			if(strncmp(op.user, op.list[i], strlen(op.user)) == 0){
				for(int j = i; j<cap; j++){
					if(j==cap-1){
						bzero(op.list[j], MAX);
						bzero(op.IPs[j], MAX);
					}else{
						bzero(op.list[j], MAX);
						bzero(op.IPs[j], MAX);
						strncat(op.list[j], op.list[j+1], strlen(op.list[j+1]));
						strncat(op.IPs[j], op.IPs[j+1], strlen(op.IPs[j+1]));
						op.ports[j] = op.ports[j+1];
						op.onl[j] = op.onl[j+1];
					}
				}
			}
		}
		op.size -= 1;
	}else if(op.command == 4){
		bzero(op.user, MAX);
		bzero(op.IP, MAX);
		strncat(op.user, op.list[op.id-1], strlen(op.list[op.id-1]));
		strncat(op.IP, op.IPs[op.id-1], strlen(op.IPs[op.id-1]));
		op.port = op.ports[op.id-1];
		op.sr = op.onl[op.id-1];
	}else if(op.command == 5){
		printf("address lookup recived from %s.\n", op.user);
		bzero(op.IP, MAX);
		bzero(op.user, MAX);
		strncat(op.user, op.list[op.id-1], strlen(op.list[op.id-1]));
		strncat(op.IP, op.IPs[op.id-1], strlen(op.IPs[op.id-1]));
	}
	
	return op;
}

// Driver code
int main() {
	int sockfd, size = 0;
	struct logs test, next;
	char buffer[MAXLINE];
	
	strcpy(buffer, "");
	//char *hello = "Hello from server";
	struct sockaddr_in servaddr, cliaddr;
		
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");//INADDR_ANY;
	servaddr.sin_port = htons(20002);
		
	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
		
	int len, n;
	
	len = sizeof(cliaddr); //len is value/result
	
	n = recvfrom(sockfd, (struct logs*)&test, sizeof(test),
				0, ( struct sockaddr *) &cliaddr,
				&len);
	test = operate(test, size);
	size = test.size;
	sendto(sockfd, (struct logs*)&test, sizeof(test),
		0, (const struct sockaddr *) &cliaddr,
			len);
	
	for(;;){
		recvfrom(sockfd, (struct logs*)&next, sizeof(next), 0, ( struct sockaddr *) &cliaddr, &len);
		next = result(test, next);
		next = operate(next, size);
		size = next.size;
		sendto(sockfd, (struct logs*)&next, sizeof(next), 0, (const struct sockaddr *) &cliaddr, len);
		test = next;
	}
	
	return 0;
}
