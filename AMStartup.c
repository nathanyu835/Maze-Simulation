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

// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

// ---------------- Public functions


/*void* newAvatar(int mazePort, int mazeWidth, int mazeHeight, int avatarId)
{

}*/

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
	AM_INIT_msg->type = AM_INIT;
	AM_INIT_msg->init.nAvatars = nAvatars;
	AM_INIT_msg->init.Difficulty = Difficulty;

	send(sockfd, AM_INIT_msg, sizeof(AM_Message), 0);

	AM_Message *AM_INIT_resp = (AM_Message *) calloc(1, sizeof(AM_Message));
	recv(sockfd, AM_INIT_resp, sizeof(AM_Message), 0);
	printf("%u\n", ntohl((uint32_t)AM_INIT_resp->type));

	//if(AM_init_failed)
		//exit with error
	//if(AM_INIT_OK) {
		//int mazePort = mazePort;
		//int mazeWidth = mazeWidth;
		//int mazeHeight = mazeHeight;
	//}
	//while(avatarId < nAvatars) {
	//	newAvatar(mazePort, mazeWidth, mazeHeight, avatarId);
	//	avatarId++;
	//}

}


int main(int argc, char **argv)
{
	AMStartup(atoi(argv[1]), atoi(argv[2]), argv[3]);
	return 0;
}