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
int mazePort;
int mazeWidth;
int mazeHeight;

// ---------------- Private prototypes

// ---------------- Public functions


int chooseDir(Avatar *avatar, XYPos *rendezvous) {
	int dir = rand() % 4;
	int ax = avatar->pos->x;
	int ay = avatar->pos->y;
	int rx = rendezvous->x;
	int ry = rendezvous->y;
	if(ax == rx && ay == ry) {
		return M_NULL_MOVE;
	}
	return dir;
}

void AMStartup(int Difficulty)
{
	int sockfd;
	struct sockaddr_in servaddr;

	//create socket for startup
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        	perror("Problem in creating the socket\n");
        	exit(2);
    	}

    	//Setup socket
    	memset(&servaddr, 0, sizeof(servaddr));
    	servaddr.sin_family = AF_INET;
     	servaddr.sin_addr.s_addr= inet_addr(hostIP);
     	servaddr.sin_port =  htons(atoi(AM_SERVER_PORT)); //convert to network order

     	//Connection of the client to the socket 
     	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        	perror("Problem in connecting to the server\n");
        	exit(3);
     	}

     	//Send init message to the server
	AM_Message *AM_INIT_msg = (AM_Message *) calloc(1, sizeof(AM_Message));
	AM_INIT_msg->type = htonl(AM_INIT);
	AM_INIT_msg->init.nAvatars = htonl(nAvatars);
	AM_INIT_msg->init.Difficulty = htonl(Difficulty);

	send(sockfd, AM_INIT_msg, sizeof(AM_Message), 0);

	//Get init response from the server
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
	//Set maze variables
	mazeHeight = ntohl(AM_INIT_resp->init_ok.MazeHeight);
	mazeWidth = ntohl(AM_INIT_resp->init_ok.MazeWidth);
	mazePort = ntohl(AM_INIT_resp->init_ok.MazePort);

	//Create maze
	initializeMaze(mazeHeight, mazeWidth, nAvatars);
	createPerimeter(mazeHeight, mazeWidth);
	AMClient();
}


int main(int argc, char **argv)
{
	hostIP = argv[3];
	nAvatars = atoi(argv[1]);
	rendezvous = (XYPos *) calloc (1, sizeof(XYPos));
	rendezvous->x = -1;
	rendezvous->y = -1;
	AMStartup(atoi(argv[2]));
	return 0;
}