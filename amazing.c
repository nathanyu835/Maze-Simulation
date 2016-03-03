

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
MazeNode **Amazing;


// ---------------- Private prototypes
//void Traverse(Avatar *curr);
void initializeMaze(int height, int width, int nAvatars);

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
}


//PSEUDOCODE
/*
Create maze based on input
Randomly put in N avatars into the maze
Initialize 2D array of maze size for each Avatar
Find meeting location based on average x and y coordinates of starting location
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




/*
* We will mark the path that each avatar has traveled (update visited in AvataerInfo). If a location is twice visited, we will block it off (dead end) 
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
            printf("index is %d,%d\n", i,j);
            Amazing[i][j].visited = malloc(sizeof(struct MazeNode) + (nAvatars));
            Amazing[i][j].up = 0;
            Amazing[i][j].right = 0;
            Amazing[i][j].down = 0;
            Amazing[i][j].left = 0;
            printf("up is %d\n", Amazing[i][j].up);
        }

    Amazing[0][1].visited[0]++;   
    printf("visited is %d\n", Amazing[0][1].visited[0]); 
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