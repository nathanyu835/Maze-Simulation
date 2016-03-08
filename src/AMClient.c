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
FILE *testLog;
int moveCount;

// ---------------- Private prototypes

void drawMaze(MazeNode** maze, Avatar** avatars, int mazeheight, int mazewidth);

// ---------------- Public functions



void* newAvatar(void *newAvatar)
{
	Avatar *avatar = (Avatar *) newAvatar;
	XYPos *pos = (XYPos *) calloc(1, sizeof(XYPos));
	pos->x = -1;
	pos->y = -1;
	avatar->pos = pos;
	avatar->solved = 0;
	free(pos);

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
     	servaddr.sin_addr.s_addr= inet_addr(hostname);
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

	fprintf(testLog, "Avatar %d has been created\n", avatar->AvatarId);

	//Allocate memory for move messages to the server, with constant type and id
	AM_Message *move = (AM_Message *) calloc(1, sizeof(AM_Message));
	move->type = htonl(AM_AVATAR_MOVE);
	move->avatar_move.AvatarId = htonl(avatar->AvatarId);

	//Allocate memory for response messages from the server
	AM_Message *response = (AM_Message *) calloc(1, sizeof(AM_Message));

	int justMoved = 0; //for updating wall
	int firstMove = 1; //for updating initial square
	
	//wait for the first turn so that we can set the target location
	recv(sockfd, response, sizeof(AM_Message), 0);
	if(ntohl(response->type) == AM_AVATAR_TURN) {
		if(ntohl(response->avatar_turn.TurnId) == avatar->AvatarId) {
			//Only avatar 0 will print this
			moveCount++;
			fprintf(testLog, "\nAll avatars successfully initialized\n");
			//calculate target in the maze
			if (rendezvous->x == -1) {
				int xSum = 0;
				int ySum = 0;
				for(int i = 0; i < nAvatars; i++) {
					xSum += ntohl(response->avatar_turn.Pos[i].x);
					ySum += ntohl(response->avatar_turn.Pos[i].y);
				}
				rendezvous->x = xSum/nAvatars;
				rendezvous->y = ySum/nAvatars;
				fprintf(testLog, "The target position is (%d, %d)\n\n", rendezvous->x, rendezvous->y);
			}
			int newX = ntohl(response->avatar_turn.Pos[avatar->AvatarId].x);
			int newY = ntohl(response->avatar_turn.Pos[avatar->AvatarId].y);
			avatar->pos->x = newX;
			avatar->pos->y = newY;
			visitSquare(avatar->pos, avatar->AvatarId);
			
            Amazing[newX][newY].whoLast = avatar->AvatarId;
            //Amazing[newX][newY].lastDir = avatar->face;

            firstMove = 0;
			/***********************************
			Put algorithm here in place of next line
			*************************************/
			//Print initial conditions
			for(int i = 0; i < nAvatars; i++) {
				fprintf(testLog, "The initial position of avatar %d is (%d, %d)\n", i, 
					ntohl(response->avatar_turn.Pos[i].x), ntohl(response->avatar_turn.Pos[i].y));
			}
			int dir = getMove(avatar->pos, avatar->AvatarId);
			fprintf(testLog, "\nTurn %d:\nIt is Avatar %d's turn, attempted move: %d\n", 
						moveCount, avatar->AvatarId, dir);
			//Send move message to the server
			move->avatar_move.Direction = htonl(dir);
			avatar->face = dir;
			send(sockfd, move, sizeof(AM_Message), 0);
			justMoved = 1;
		}
	}
	while(1 == 1) {
		//Wait for message from the server
		recv(sockfd, response, sizeof(AM_Message), 0);
		//Check if it is this avatar's turn to move, if so send move request to server
		int respType = ntohl(response->type);
		if(respType == AM_AVATAR_TURN) {
			if(justMoved == 1) {
				justMoved = 0;
				int newX = ntohl(response->avatar_turn.Pos[avatar->AvatarId].x);
				int newY = ntohl(response->avatar_turn.Pos[avatar->AvatarId].y);
				
                Amazing[newX][newY].whoLast = avatar->AvatarId;
                Amazing[newX][newY].lastDir = avatar->face;

                if (avatar->pos->x == newX && avatar->pos->y == newY) 
				{
					addWall(avatar->pos, avatar->face);
				} else {
					avatar->pos->x = newX;
					avatar->pos->y = newY;
					visitSquare(avatar->pos, avatar->AvatarId);
				}
			}
			if (firstMove == 1) {
				int newX = ntohl(response->avatar_turn.Pos[avatar->AvatarId].x);
				int newY = ntohl(response->avatar_turn.Pos[avatar->AvatarId].y);
				avatar->pos->x = newX;
				avatar->pos->y = newY;
				visitSquare(avatar->pos, avatar->AvatarId);
				firstMove = 0;
			}
			if(ntohl(response->avatar_turn.TurnId) == avatar->AvatarId) {
				usleep(200); //give qanother avatar time to update the maze
				moveCount++;
				justMoved = 1;
				/***********************************
				Put algorithm here in place of next line
				*************************************/
				int dir = getMove(avatar->pos, avatar->AvatarId); 

				//Print info to logfile
				fprintf(testLog, "After the move, the new positions are:\n");
				for(int i = 0; i < nAvatars; i++) {
					int currX = ntohl(response->avatar_turn.Pos[i].x);
					int currY = ntohl(response->avatar_turn.Pos[i].y);
					fprintf(testLog, "The position of avatar %d is (%d, %d)\n", i, currX, currY);
				}
				fprintf(testLog, "\nTurn %d:\nIt is Avatar %d's turn, attempted move: %d\n", 
						moveCount, avatar->AvatarId, dir);
				//sleep to allow time for print statments
				usleep(200);
				move->avatar_move.Direction = htonl(dir);

                avatar->face = dir;
				Amazing[avatar->pos->x][avatar->pos->y].lastDir = dir;
                send(sockfd, move, sizeof(AM_Message), 0);

                drawMaze(Amazing, avatars, mazeHeight, mazeWidth);

			}
			usleep(200); //allow time for print statements to print in the correct order
		} else {
			if(respType == AM_MAZE_SOLVED && avatar->AvatarId == 0) {
				sleep(1); // give a chance for the other avatars to finish any remaining print statements
				fprintf(testLog, "Maze completed in %d moves\n", moveCount);
				terminated = response;
				break;
			}  else if ((respType | AM_AVATAR_OUT_OF_TURN | AM_UNEXPECTED_MSG_TYPE 
						| AM_UNKNOWN_MSG_TYPE) != 0) {
				terminated = response;
				sleep(50); //Do not attempt to make another move
			}
			//Otherwise wait and continue
			sleep(1);
		}

	}
	return NULL;
}

void AMClient()
{
	terminated = NULL;
	moveCount = 0;

	char logPath[200];
	sprintf(logPath,"./testing/testing_log_nAvatars_%d_Difficulty_%d", nAvatars, difficulty);
	printf("logpath is: %s\n", logPath);
	testLog = fopen(logPath, "w+");
	if(testLog == NULL) {
		printf("Error creating test log file: %s\n", logPath);
		return;
	}
	fprintf(testLog, "Testing file for Amazing Project\n");
	fprintf(testLog, "nAvatars: %d\n", nAvatars);
	fprintf(testLog, "Difficulty: %d\n\n", difficulty);

	pthread_t avatarThreads[nAvatars];
	//Create a new thread for each avatar passing it a new Avatar pointer
	int curr_id = 0;
	while(curr_id < nAvatars) {
		Avatar *nextAvatar = (Avatar *) calloc(1, sizeof(Avatar));
		
        nextAvatar->AvatarId = curr_id;
		
        // Make array of avatars
        avatars[curr_id] = (Avatar *)nextAvatar;

        int iret1 = pthread_create(&(avatarThreads[curr_id]), NULL, newAvatar, nextAvatar);
		if(iret1) {
			fprintf(stderr,"Cannot create thread, rc=%d\n", iret1);
			return;
		}
		usleep(50);
		curr_id++;
	}
	while (terminated == NULL) {
		usleep(50);
	}
	//The maze has terminated, check the last message for exit status
	switch (ntohl(terminated->type)) {
		case AM_MAZE_SOLVED:
			sleep(1); //Make sure everything has been written to the logfile before exitting
			break;
		case AM_NO_SUCH_AVATAR | AM_ERROR_MASK:
			fprintf(testLog, "Invalid AvatarId. Terminating program\n");
			break;
		case AM_TOO_MANY_MOVES | AM_ERROR_MASK:
			fprintf(testLog, "Move limit has been reached. Terminating program\n");
			break;
		case AM_SERVER_DISK_QUOTA | AM_ERROR_MASK:
			fprintf(testLog, "Disk quota has been reached. Terminating program\n");
			break;
		case AM_SERVER_OUT_OF_MEM | AM_ERROR_MASK:
			fprintf(testLog, "Disk quota has been reached. Terminating program\n");
			break;
		case AM_UNKNOWN_MSG_TYPE | AM_ERROR_MASK:
			fprintf(testLog, "Unkown message type sent to server. Terminating program\n");
			break;
		case AM_SERVER_TIMEOUT | AM_ERROR_MASK:
			fprintf(testLog, "Disk quota has been reached. Terminating program\n");
			break;
		case AM_UNEXPECTED_MSG_TYPE | AM_ERROR_MASK:
			fprintf(testLog, "Disk quota has been reached. Terminating program\n");
			break;
	}
	fclose(testLog);
}


void drawMaze(MazeNode** maze, Avatar** avatars, int mazeheight, int mazewidth){

    char* palette[11];
    palette[0]="\033[22;31m"; // red
    palette[1]="\033[22;32m"; // green
    palette[2]="\033[22;33m"; // brown
    palette[3]="\033[22;34m"; // blue
    palette[4]="\033[22;35m"; // magenta
    palette[5]="\033[22;36m"; // cyan
    palette[6]="\033[01;31m"; // light red
    palette[7]="\033[01;32m"; // light green
    palette[8]="\033[01;33m"; // yellow
    palette[9]="\033[01;34m"; // light blue
    palette[10]="\033[22;30m"; //black

    printf("\033[2J\033[1;1H");
    for (int height = 0; height <= mazeheight + 1; height++) {
        for (int width = 0; width <= mazewidth + 1; width++) {
            
            // Print border of the maze
            if (width == 0 && height == 0){// || width == mazewidth + 1 || height == mazeheight + 1) {
                printf("%s\u256D", palette[10]);
                continue;
            }
            if (width == mazewidth + 1 && height == 0){
                printf("%s\u2500\u256E", palette[10]);
                continue;
            }
            if (width == 0 && height == mazeheight + 1){
                printf("%s\u2570\u2500", palette[10]);
                continue;
            }
            if (width == mazewidth + 1 && height == mazeheight + 1){
                printf("%s\u256F ", palette[10]);
                continue;
            }
            if(width == 0 || width == mazewidth+1){
                printf("%s\u2502 ", palette[10]);
                continue;
            }
            if(height == 0 || height == mazeheight+1){
                printf("%s\u2500\u2500", palette[10]);
                continue;
            }
            else{

            }

            // Print rendezvous point
            if(width-1==rendezvous->x && height-1==rendezvous->y){
                printf("%s\u25CE ", palette[0]);
                continue;
            }

            // Get current node and skip if unvisited
            MazeNode *currNode = &maze[width-1][height-1];
            if(currNode->whoLast==-1){
                printf("  ");
                continue;
            }

            Avatar *currAv = avatars[currNode->whoLast];
            char *arrow = calloc(7, sizeof(char));

            if(currAv->pos->x == width-1 && currAv->pos->y == height-1){
                arrow = "o";//"\u2605";
            }

            else if (currNode->lastDir == 0) {
                arrow = "\u2190";
            }
            else if (currNode->lastDir == 1) {
                arrow = "\u2191";
            }
            else if (currNode->lastDir == 2) {
                arrow = "\u2193";
            }
            else if (currNode->lastDir == 3) {
                arrow = "\u2192";
            }
            else {
                arrow = "-";
            }

            printf("%s%s ", palette[currNode->whoLast],arrow);

        }
        printf("\n\033[0m");
    }
    fflush(stdout);
    usleep(2000);
}
