/* ========================================================================== */
/* File: AMStartup.c
 *
 * Project name: CS50 Amazing Project
 * Component name: AMStartup
 *
 * Author: Ian DeLaney
 * Date: Wed, Mar 2016
 *
 * Init sends the AM_INIT message to the server and then beings threads for 
 * each avatar. Then waits until the process is done.
 *
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdlib.h>
#include <stdio.h>			//I/O functionality
#include <sys/types.h>			//Additional types
#include <sys/socket.h>			//socket functionality
#include <netinet/in.h>			//net functionality
#include <string.h>			//string functionality
#include <arpa/inet.h>			//net functionality
#include <pthread.h>			//Thread functionality

// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables
int avatar_id;

// ---------------- Private prototypes

// ---------------- Public functions


void* newAvatar(void *ptr)
{
	printf("Creating avatar %d\n", avatar_id);
	return NULL;
}

void AMStartup(int nAvatars, int Difficulty, char *hostIP)
{
	int sockfd;
	struct sockaddr_in servaddr;
	//create socket for startup
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        	perror("Problem in creating the socket");
        	exit(2);
    	}

    	//Setup socket
    	memset(&servaddr, 0, sizeof(servaddr));
    	servaddr.sin_family = AF_INET;
     	servaddr.sin_addr.s_addr= inet_addr(hostIP);
     	servaddr.sin_port =  htons(atoi(AM_SERVER_PORT)); //convert to network order

     	//Connection of the client to the socket 
     	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        	perror("Problem in connecting to the server");
        	exit(3);
     	}

	AM_Message *AM_INIT_msg = (AM_Message *) calloc(1, sizeof(AM_Message));
	AM_INIT_msg->type = htonl(AM_INIT);
	AM_INIT_msg->init.nAvatars = htonl(nAvatars);
	AM_INIT_msg->init.Difficulty = htonl(Difficulty);

	send(sockfd, AM_INIT_msg, sizeof(AM_Message), 0);

	AM_Message *AM_INIT_resp = (AM_Message *) calloc(1, sizeof(AM_Message));
	recv(sockfd, AM_INIT_resp, sizeof(AM_Message), 0);
	if (ntohl(AM_INIT_resp->type) != AM_INIT_OK) {
		printf("Initialization failed with type %u\n", ntohl(AM_INIT_resp->type));
		return;
	}
	if (ntohl(AM_INIT_resp->type) == AM_INIT_OK) {
		printf("Message type is %u\n", ntohl(AM_INIT_resp->type));
		printf("MazePort is %u\n", ntohl(AM_INIT_resp->init_ok.MazePort));
		printf("Maze width is %u\n", ntohl(AM_INIT_resp->init_ok.MazeWidth));
		printf("Maze height is %u\n", ntohl(AM_INIT_resp->init_ok.MazeHeight));

	}
	uint32_t mazeHeight = ntohl(AM_INIT_resp->init_ok.MazeHeight);
	uint32_t mazeWidth = ntohl(AM_INIT_resp->init_ok.MazeWidth);
	uint32_t mazePort = ntohl(AM_INIT_resp->init_ok.MazePort);

	avatar_id = 0;
	while(avatar_id < nAvatars) {
		pthread_t t1;
		int iret1 = pthread_create(&t1, NULL, newAvatar, NULL);
		if(iret1) {
			fprintf(stderr,"Cannot create thread, rc=%d\n", iret1);
			return;
		}
		for(int i = 0; i < 1500000; i++) {
			;
		}
		avatar_id++;
	}
}


int main(int argc, char **argv)
{
	AMStartup(atoi(argv[1]), atoi(argv[2]), argv[3]);
	return 0;
}