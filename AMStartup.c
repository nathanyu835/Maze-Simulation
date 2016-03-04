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

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables
char *hostIP;
int mazePort;
int mazeWidth;
int mazeHeight;
int nAvatars;
AM_Message *terminated;

// ---------------- Private prototypes

// ---------------- Public functions


void* newAvatar(void *avatar_id)
{
	int AvatarId = *((int *) avatar_id);
	printf("Creating avatar %d\n", AvatarId);

	//create a new socket for this avatar
	int sockfd;
	struct sockaddr_in servaddr;

	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        	perror("Problem in creating the socket");
        	exit(2);
    	}

    	//Setup socket
    	memset(&servaddr, 0, sizeof(servaddr));
    	servaddr.sin_family = AF_INET;
     	servaddr.sin_addr.s_addr= inet_addr(hostIP);
     	servaddr.sin_port =  htons(mazePort); //convert to network order

     	//Connection of the client to the socket 
     	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        	perror("Problem in connecting to the server");
        	exit(3);
     	}

     	//Allocate send and free the ready message to the server
	AM_Message *ready_msg = (AM_Message *) calloc(1, sizeof(AM_Message));
	ready_msg->type = htonl(AM_AVATAR_READY);
	ready_msg->avatar_ready.AvatarId = htonl(AvatarId);
	send(sockfd, ready_msg, sizeof(AM_Message), 0);
	free(ready_msg);

	//Allocate memory for move messages to the server, with constant type and id
	AM_Message *move = (AM_Message *) calloc(1, sizeof(AM_Message));
	move->type = htonl(AM_AVATAR_MOVE);
	move->avatar_move.AvatarId = htonl(AvatarId);

	//Allocate memory for response messages from the server
	AM_Message *response = (AM_Message *) calloc(1, sizeof(AM_Message));

	//wait for the first turn so that we can set the target location
	recv(sockfd, response, sizeof(AM_Message), 0);
	if(ntohl(response->type) == AM_AVATAR_TURN) {
		if (target->x == -1 && AvatarId == ntohl(response->avatar_turn.TurnId)) {
			int xSum = 0;
			int ySum = 0;
			for(int i = 0; i < nAvatars; i++) {
				xSum += ntohl(response->avatar_turn.Pos[i].x);
				ySum += ntohl(response->avatar_turn.Pos[i].y);
			}
			target->x = xSum/nAvatars;
			target->y = ySum/nAvatars;
			printf("The target position is (%d, %d)\n", target->x, target->y);
		}
		int dir = rand() % 4;
		printf("After the move, the new positions are:\n");
		for(int i = 0; i < nAvatars; i++) {
			printf("The position of avatar %d is (%d, %d)\n", i, ntohl(response->avatar_turn.Pos[i].x), ntohl(response->avatar_turn.Pos[i].y));
		}
		printf("It is Avatar %d's turn, attempted move: %d\n", AvatarId, dir);
		move->avatar_move.Direction = htonl(dir);
		send(sockfd, move, sizeof(AM_Message), 0);
	}
	while(1 == 1) {
		//Wait for message from the server
		recv(sockfd, response, sizeof(AM_Message), 0);
		//Check if it is this avatar's turn to move, if so send move request to server
		if(ntohl(response->type) == AM_AVATAR_TURN) {
			if(ntohl(response->avatar_turn.TurnId) == AvatarId) {
				int dir = rand() % 4;
				printf("After the move, the new positions are:\n");
				for(int i = 0; i < nAvatars; i++) {
					printf("The position of avatar %d is (%d, %d)\n", i, ntohl(response->avatar_turn.Pos[i].x), ntohl(response->avatar_turn.Pos[i].y));
				}
				printf("It is Avatar %d's turn, attempted move: %d\n", AvatarId, dir);
				move->avatar_move.Direction = htonl(dir);
				send(sockfd, move, sizeof(AM_Message), 0);
			}
			//sleep(1);
		} else {
			sleep(1);
			if(ntohl(response->type) == AM_MAZE_SOLVED &&  AvatarId == 0) {
				sleep(1);
				printf("Maze completed, avatar %d exiting\n", AvatarId);
				terminated = response;
				break;
			} else {
				printf("And the other avatars, with message type: %d\n", ntohl(response->type));
				break;
			}
		}

	}
	return NULL;
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
	//mazeNode maze[mazeWidth][mazeHeight] = mazeInit() 

	int curr_id = 0;
	pthread_t avatars[nAvatars];
	while(curr_id < nAvatars) {
		int iret1 = pthread_create(&(avatars[curr_id]), NULL, newAvatar, &curr_id);
		if(iret1) {
			fprintf(stderr,"Cannot create thread, rc=%d\n", iret1);
			return;
		}
		for(int i = 0; i<1000000; i++);
		curr_id++;
	}

	while (terminated == NULL) {
		sleep(1);
	}

}


int main(int argc, char **argv)
{
	hostIP = argv[3];
	nAvatars = atoi(argv[1]);
	target = (XYPos *) calloc (1, sizeof(XYPos));
	target->x = -1;
	target->y = -1;
	terminated = NULL;
	AMStartup(atoi(argv[2]));
	return 0;
}