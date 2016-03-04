

/* ========================================================================== */
/* File: amazing.c - Amazing Project
 *
 * Author: Nathan Yu, Ian Delaney, Stephanie Guo
 * Date: February 27, 2015
 *
 * Input:
 *
 * Command line options: 
 *
 * Output:
 *
 * Error Conditions:
 *
 * Special Considerations:
 *
 */
/* ========================================================================== */
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdlib.h>
#include <stdio.h> 
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables
int nAvatars, difficulty;
char *hostname;
XYPos *rendezvous;

// ---------------- Private prototypes
//void Traverse(Avatar *curr);
void initializeMaze(int height, int width, int nAvatars);
void createPerimeter(int height, int width);

/* ========================================================================== */

int main(int argc, char *argv[])
{
    // check command line arguments
    if (argc != 4)
    {
        printf("Usage: %s requires 3 arguments: [NUM_AVATARS] [DIFFICULTY] [HOSTNAME]\n", argv[0]);
        exit(1);
    }

    hostname = calloc(strlen(argv[3]) + 1, sizeof(char));
    strcpy(hostname, argv[3]);
    nAvatars = atoi(argv[1]);
    difficulty = atoi(argv[2]);
    int height = difficulty;
    int width = difficulty;

    initializeMaze(height, width, nAvatars);

    // CODE FOR TESTING MAZE INITIALIZATION
    for(int i = 0; i < width; i++)
        for(int j = 0; j < height; j++)
        {
            printf("index is %d,%d: ", i,j); 
            printf("north is: %d, " , Amazing[i][j].north);
            printf("west is: %d, " , Amazing[i][j].west);
            printf("south is: %d, " , Amazing[i][j].south);
            printf("east is: %d\n" , Amazing[i][j].east);
        }
    


}


//PSEUDOCODE
/*
while(!sameLocation())
{
    Traverse(); //use int and directionsTried to optimize run time
    Avatar->Location->AvatarInfo->int++; //block of dead ends (int = 2)
    if(!direction)
        add direction to AvatarInfo->directionsTried
}

int sameLocation()
{
    for(int i = 0; i < AvatarCount; i++)
    {
        if(Avatar->location != (Avatar+1)->Location)
            return 0;
    }
    return 1;
}
*/




// Basic pseudocode for our traversal: Find target location based on average location of Avatars.
// We then find the Manhattan Distance between each avatar and the location, which is the sum of the ABS of the x distance and y distance to location.

void initializeMaze(int height, int width, int nAvatars)
{
    MazeNode *temp = (MazeNode*)calloc(width * height, sizeof(MazeNode));
    Amazing = (MazeNode**)calloc(width, sizeof(MazeNode*));
    for (int i = 0; i < width; i++)
        Amazing[i] = temp + i*height;
    for(int i = 0; i < width; i++)
        for(int j = 0; j < height; j++)
        {
            Amazing[i][j].visited = malloc(sizeof(struct MazeNode) + (nAvatars));
            Amazing[i][j].north = 0;
            Amazing[i][j].east = 0;
            Amazing[i][j].south = 0;
            Amazing[i][j].west = 0;
        }

    createPerimeter(height, width);
}

void createPerimeter(int height, int width)
{
    for(int i = 0; i < height; i++)
    {
        Amazing[i][0].north = 1;
        Amazing[i][width-1].south = 1;
    }

    for(int j = 0; j < width; j++)
    {
        Amazing[0][j].west = 1;
        Amazing[height-1][j].east = 1;
    }
}

void getRendezvous(Avatar *avatar)
{
    int xSum = 0, ySum = 0;

    for(int i = 0; i < nAvatars; i++)
    {
        xSum += avatar->pos.x;
        ySum += avatar->pos.y;
    }

    rendezvous->x = xSum/nAvatars;
    rendezvous->y = ySum/nAvatars;
}

/*
void Traverse()
{
    Point src, dst; // Source and destination coordinates
    // cur also indicates the coordinates of the current location
    int MD_best = MD(src, dst); // It stores the closest MD we ever had to dst
    // A productive path is the one that makes our MD to dst smaller
    while(cur != dst)
    {
        if(∃ a productive path)
            Take the productive path;
        else
        {
            MD_best = MD(cur, dst);
            Imagine a line between cur and dst;
            Take the first path in the left/right of the line; // The left/right selection affects the following hand rule
            while(MD(cur, dst) != MD_best || !∃ a productive path)
                Follow the right-hand/left-hand rule; // The opposite of the selected side of the line
        }
    }
}*/