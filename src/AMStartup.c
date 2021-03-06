/* ========================================================================== */
/* File: AMStartup.c
 *
 * Project name: CS50 Amazing Project
 * Component name: AMStartup
 *
 * Author: Ian DeLaney, Nathan Yu, Stephanie Guo
 * Date: Wed, Mar 2016
 *
 * Init sends the AM_INIT message to the server and then calls AMClient toe run
 * the rest of the maze and then ends.
 *
 */
/* ========================================================================== */

#define _DEFAULT_SOURCE			//For usleep

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdlib.h>
#include <unistd.h>			//sleep functionality
#include <stdio.h>			//I/O functionality
#include <sys/types.h>			//Additional types
#include <sys/socket.h>			//socket functionality
#include <netinet/in.h>			//net functionality
#include <string.h>			//string functionality
#include <arpa/inet.h>			//net functionality
#include <pthread.h>			//Thread functionality

// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"
#include "AMClient.h"


// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

// ---------------- Public functions

/*
 * AMStartup connects with the server via an AM_INIT message
 * if it receives and AM_INIT_OK message it calls AMClient and begins the maze
 * Otherwise it prints an error and exits 
 */
void AMStartup()
{
	int sockfd;
	struct sockaddr_in servaddr;

	//create socket for startup
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        	perror("Problem in creating the socket\n");
        	exit(2);
        	return;
    	}

    	//Setup socket
    	memset(&servaddr, 0, sizeof(servaddr));
    	servaddr.sin_family = AF_INET;
     	servaddr.sin_addr.s_addr= inet_addr(hostname);
     	servaddr.sin_port =  htons(atoi(AM_SERVER_PORT)); //convert to network order

     	//Connection of the client to the socket 
     	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        	perror("Problem in connecting to the server\n");
        	exit(3);
        	return;
     	}

     	//Send init message to the server
	AM_Message *AM_INIT_msg = (AM_Message *) calloc(1, sizeof(AM_Message));
	AM_INIT_msg->type = htonl(AM_INIT);
	AM_INIT_msg->init.nAvatars = htonl(nAvatars);
	AM_INIT_msg->init.Difficulty = htonl(difficulty);

	send(sockfd, AM_INIT_msg, sizeof(AM_Message), 0);
	free(AM_INIT_msg);

	//Get init response from the server
	AM_Message *AM_INIT_resp = (AM_Message *) calloc(1, sizeof(AM_Message));
	recv(sockfd, AM_INIT_resp, sizeof(AM_Message), 0);
	if (ntohl(AM_INIT_resp->type) != AM_INIT_OK) {
		printf("Initialization failed with type %u\n", ntohl(AM_INIT_resp->type)-AM_ERROR_MASK);
		return;
	}
	//Set maze variables
	mazeHeight = ntohl(AM_INIT_resp->init_ok.MazeHeight);
	mazeWidth = ntohl(AM_INIT_resp->init_ok.MazeWidth);
	mazePort = ntohl(AM_INIT_resp->init_ok.MazePort);
	free(AM_INIT_resp);

	//Create maze
	initializeMaze(mazeHeight, mazeWidth, nAvatars);
	printf("Maze initialized!\n");
	AMClient();
}


int main(int argc, char **argv)
{
	//Check args
	if (argc != 4)
    {
        printf("Usage: %s requires 3 arguments: [NUM_AVATARS] [DIFFICULTY] [HOSTNAME]\n", argv[0]);
    	exit(1);
    }    

    //Check HOSTNAME validity
    if(strcmp("129.170.214.115", argv[3])!=0)
    {
        printf("Invalid HOSTNAME! Hostname must refer to flume.cs.dartmouth.edu\n");
        exit(1);
    }

    //Put args into global variables
	nAvatars = atoi(argv[1]);
    difficulty = atoi(argv[2]);
	hostname = calloc(strlen(argv[3]) + 1, sizeof(char));
    strcpy(hostname, argv[3]);

    //Allocate rendezvous
	rendezvous = (XYPos *) calloc (1, sizeof(XYPos));
	rendezvous->x = -1;
	rendezvous->y = -1;
	
	//Allocate avatars	
    avatars = (Avatar **)calloc(nAvatars, sizeof(Avatar));
	AMStartup();
	return 0;
}
