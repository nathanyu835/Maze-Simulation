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
AM_Message *terminated;

// ---------------- Private prototypes

// ---------------- Public functions



void* newAvatar(void *newAvatar)
{
	Avatar *avatar = (Avatar *) newAvatar;
	XYPos *pos = (XYPos *) calloc(1, sizeof(XYPos));
	pos->x = -1;
	pos->y = -1;
	avatar->pos = pos;
	printf("Creating avatar %d\n", avatar->AvatarId);

	//create a new socket for this avatar
	int sockfd;
	struct sockaddr_in servaddr;

	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        	perror("Problem in creating the socket\n");
        	exit(2);
    	}

    	avatar->fd = sockfd;
    	//Setup socket
    	memset(&servaddr, 0, sizeof(servaddr));
    	servaddr.sin_family = AF_INET;
     	servaddr.sin_addr.s_addr= inet_addr(hostIP);
     	servaddr.sin_port =  htons(mazePort); //convert to network order

     	//Connection of the client to the socket 
     	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        	perror("Problem in connecting to the server\n");
        	exit(3);
     	}

     	//Allocate send and free the ready message to the server
	AM_Message *ready_msg = (AM_Message *) calloc(1, sizeof(AM_Message));
	ready_msg->type = htonl(AM_AVATAR_READY);
	ready_msg->avatar_ready.AvatarId = htonl(avatar->AvatarId);
	send(sockfd, ready_msg, sizeof(AM_Message), 0);
	free(ready_msg);

	//Allocate memory for move messages to the server, with constant type and id
	AM_Message *move = (AM_Message *) calloc(1, sizeof(AM_Message));
	move->type = htonl(AM_AVATAR_MOVE);
	move->avatar_move.AvatarId = htonl(avatar->AvatarId);

	//Allocate memory for response messages from the server
	AM_Message *response = (AM_Message *) calloc(1, sizeof(AM_Message));

	//wait for the first turn so that we can set the target location
	recv(sockfd, response, sizeof(AM_Message), 0);
	if(ntohl(response->type) == AM_AVATAR_TURN) {
		if (rendezvous->x == -1 && avatar->AvatarId == ntohl(response->avatar_turn.TurnId)) {
			int xSum = 0;
			int ySum = 0;
			for(int i = 0; i < nAvatars; i++) {
				xSum += ntohl(response->avatar_turn.Pos[i].x);
				ySum += ntohl(response->avatar_turn.Pos[i].y);
			}
			rendezvous->x = xSum/nAvatars;
			rendezvous->y = ySum/nAvatars;
			printf("The target position is (%d, %d)\n", rendezvous->x, rendezvous->y);
		}
		int dir = chooseDir(avatar, rendezvous);
		printf("After the move, the new positions are:\n");
		for(int i = 0; i < nAvatars; i++) {
			printf("The position of avatar %d is (%d, %d)\n", i, ntohl(response->avatar_turn.Pos[i].x), 
								ntohl(response->avatar_turn.Pos[i].y));
		}
		printf("It is Avatar %d's turn, attempted move: %d\n", avatar->AvatarId, dir);
		move->avatar_move.Direction = htonl(dir);
		send(sockfd, move, sizeof(AM_Message), 0);
	}
	while(1 == 1) {
		//Wait for message from the server
		recv(sockfd, response, sizeof(AM_Message), 0);
		//Check if it is this avatar's turn to move, if so send move request to server
		int respType = ntohl(response->type);
		if(respType == AM_AVATAR_TURN) {
			if(ntohl(response->avatar_turn.TurnId) == avatar->AvatarId) {
				//Update avatar position
				avatar->pos->x = ntohl(response->avatar_turn.Pos[avatar->AvatarId].x);
				avatar->pos->y = ntohl(response->avatar_turn.Pos[avatar->AvatarId].y);
				//Call algorithm to choose the next move
				int dir = chooseDir(avatar, rendezvous);
				printf("After the move, the new positions are:\n");
				for(int i = 0; i < nAvatars; i++) {
					printf("The position of avatar %d is (%d, %d)\n", i, ntohl(response->avatar_turn.Pos[i].x), 
										ntohl(response->avatar_turn.Pos[i].y));
				}
				printf("It is Avatar %d's turn, attempted move: %d towards (%d, %d)\n", avatar->AvatarId, dir, rendezvous->x, rendezvous->y);
				usleep(10000); //sleep to allow time for print statments
				move->avatar_move.Direction = htonl(dir);
				send(sockfd, move, sizeof(AM_Message), 0);
			}
			usleep(10000); //allow time for print statements to print in the correct order
		} else {
			if(respType == AM_MAZE_SOLVED && avatar->AvatarId == 0) {
				sleep(1); // give a chance for the other avatars to finish any remaining print statements
				printf("Maze completed, avatar %d exiting\n", avatar->AvatarId);
				terminated = response;
				break;
			}  else if (respType == AM_AVATAR_OUT_OF_TURN) {
				usleep(10000); //wait then continue
			}
		}

	}
	return NULL;
}



void AMClient()
{
	terminated = NULL;
	int curr_id = 0;
	pthread_t avatars[nAvatars];
	while(curr_id < nAvatars) {
		Avatar *nextAvatar = (Avatar *) calloc(1, sizeof(Avatar));
		nextAvatar->AvatarId = curr_id;
		int iret1 = pthread_create(&(avatars[curr_id]), NULL, newAvatar, nextAvatar);
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

