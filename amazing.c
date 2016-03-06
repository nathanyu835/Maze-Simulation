

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

// ---------------- Private prototypes
/*void Traverse(Avatar *curr);
void initializeMaze(int height, int width, int nAvatars);
void createPerimeter(int height, int width);
void getRendezvous(Avatar *avatar);
int getManhattan(XYPos *start, XYPos *end);
int getDirection(XYPos *start, XYPos *end);*/

/* ========================================================================== */
/*
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

    //CODE FOR TESTING MAZE INITIALIZATION
    for(int i = 0; i < width; i++)
        for(int j = 0; j < height; j++)
        {
            printf("index is %d,%d: ", i,j); 
            printf("north is: %d, " , Amazing[i][j].north);
            printf("west is: %d, " , Amazing[i][j].west);
            printf("south is: %d, " , Amazing[i][j].south);
            printf("east is: %d\n" , Amazing[i][j].east);
        }
    
}*/

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
        xSum += avatar->pos->x;
        ySum += avatar->pos->y;
    }

    rendezvous->x = xSum/nAvatars;
    rendezvous->y = ySum/nAvatars;
}

//checks for a path that decreases the MD between currPos and renedezvous, prioritizing a decrease in abs(y) over a decrease in abs(x)
//returns the direction of a potential productive move, -1 if one doesn't exist
int isProductive(XYPos *currPos)
{
    //int currMD = getManhattan(currPos, rendezvous);
    int changeX = rendezvous->x - currPos->x;
    int changeY = rendezvous->y - currPos->y;

    if(changeY > 0) //Avatar needs to move south to reach rendezvous
    {
        if(Amazing[currPos->x][currPos->y].south == 0)
            return M_SOUTH;
    }
    else
    {
        if(Amazing[currPos->x][currPos->y].north == 0)
            return M_NORTH;
    }
    if(changeX > 0) //Avatar needs to move east to reach rendezvous
    {
        if(Amazing[currPos->x][currPos->y].east == 0)
            return M_EAST;    
    }
    else
    {
        if(Amazing[currPos->x][currPos->y].west == 0)
            return M_WEST;
    }
    return -1;
}

int getMove(XYPos *curr)
{
    //XYPos *startingPoint;
    //XYPos *currPos;
    //int bestMD = getManhattan(startingPoint, rendezvous); // It stores the closest MD we ever had to rendezvous
    //while(!(currPos->x == rendezvous->x && currPos->y == rendezvous->y))
    //{
        //if(∃ a productive path) // A productive path is the one that makes our MD to dst smaller
        //    Take the productive path;
        int productive = isProductive(curr);
        if(productive != -1)
            return productive;
            //The function calling this method needs to attempt this move
            //if it fails, update the walls and then call the function again until -1 is returned
        else
        {
            int size = 0, count = 0;
            if(Amazing[XYPos->x-1][XYPos->y].visited == 0)
                size++;
            if(Amazing[XYPos->x+1][XYPos->y].visited == 0)
                size++;
            if(Amazing[XYPos->x][XYPos->y-1].visited == 0)
                size++;
            if(Amazing[XYPos->x][XYPos->y+1].visited == 0)
                size++;

            int options[size];
            if(Amazing[XYPos->x-1][XYPos->y].visited == 0)
            {
                options[count] = M_WEST;
                count++;
            }
            if(Amazing[XYPos->x+1][XYPos->y].visited == 0)
            {
                options[count] = M_EAST;
                count++;
            }
            if(Amazing[XYPos->x][XYPos->y-1].visited == 0)
            {
                options[count] = M_NORTH;
                count++;
            }
            if(Amazing[XYPos->x][XYPos->y+1].visited == 0)
            {
                options[count] = M_SOUTH;
                count++;
            }

            return options[rand() % size];
            /*
            bestMD = getManhattan(currPos, rendezvous);
            Imagine a line between cur and dst;
            Take the first path in the left/right of the line; // The left/right selection affects the following hand rule
            while(isProductive(curr) == -1) //getManhattan(currPos, rendezvous) != bestMD || 
            {
                // The opposite of the selected side of the line (right hand rule)
                //int face = getDirection(currPos, rendezvous);
                
                switch(face)
                {
                    case M_EAST:
                    {
                        if(Amazing[currPos->x][currPos->y].south == 0 && Amazing[currPos->x][currPos->y].east == 1)
                        {
                            currPos->y = currPos->y + 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_EAST;
                        }
                        else if (Amazing[currPos->x][currPos->y].west == 0 && Amazing[currPos->x][currPos->y].south == 1)
                        {
                            currPos->x = currPos->x - 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_NORTH;
                        }
                        else if (Amazing[currPos->x][currPos->y].east == 0 && Amazing[currPos->x + 1][currPos->y].north == 1)
                        {
                            currPos->x = currPos->x + 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_SOUTH;
                        }
                        else
                            face = M_SOUTH;
                        break;
                    }
                    case M_SOUTH:
                    {
                        if(Amazing[currPos->x][currPos->y].east == 0 && Amazing[currPos->x][currPos->y].north == 1)
                        {
                            currPos->x = currPos->x + 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_SOUTH;
                        }
                        else if(Amazing[currPos->x][currPos->y].north == 0 && Amazing[currPos->x][currPos->y - 1].west== 1)
                        {
                            currPos->y = currPos->y - 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_WEST;
                        }
                        else if(Amazing[currPos->x][currPos->y].south == 0 && Amazing[currPos->x][currPos->y].east == 1)
                        {
                            currPos->y = currPos->y + 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_EAST;
                        }
                        else
                            face = M_WEST;
                        break;
                    }
                    case M_WEST:
                    {
                        if(Amazing[currPos->x][currPos->y].north == 0 && Amazing[currPos->x][currPos->y].west == 1)
                        {
                            currPos->y = currPos->y - 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_WEST;
                        }
                        else if(Amazing[currPos->x][currPos->y].west == 0 && Amazing[currPos->x][currPos->y + 1].west == 1)
                        {
                            currPos->x = currPos->x - 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_NORTH;
                        }
                        else if(Amazing[currPos->x][currPos->y].east == 0 && Amazing[currPos->x][currPos->y].north == 1)
                        {
                            currPos->x = currPos->x + 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_SOUTH;
                        }
                        else if(Amazing[currPos->x][currPos->y].south == 0 && Amazing[currPos->x][currPos->y].east == 1)
                        {
                            currPos->y = currPos->y + 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_EAST;
                        }
                        else if(Amazing[currPos->x][currPos->y].west == 0 && Amazing[currPos->x][currPos->y + 1].west == 1)
                        {
                            currPos->x = currPos->x - 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_NORTH;
                        }
                        else
                            face = M_NORTH;
                        break;
                    }
                    case M_NORTH:
                    {
                        if(Amazing[currPos->x][currPos->y].west == 0 && Amazing[currPos->x][currPos->y].south == 1)
                        {
                            currPos->x = currPos->x - 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_NORTH;
                        }
                        else if(Amazing[currPos->x][currPos->y].north == 0 && Amazing[currPos->x][currPos->y].east == 1)
                        {
                            currPos->y = currPos->y - 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_WEST;
                        }
                        else if(Amazing[currPos->x][currPos->y].south == 0 && Amazing[currPos->x + 1][currPos->y].south == 1)
                        {
                            currPos->y = currPos->y + 1;
                            //Move avatar to (currPos->x,currPos->y)
                            face = M_EAST;
                        }
                        else
                            face = M_EAST;
                        break;
                    }
                }   
            }
            */
        }
    //}
}

int getDirection(XYPos *start, XYPos *end)
{
    printf("Direction from (%d,%d) to (%d, %d)", start->x, start->y, end->x, end->y);  

    int endX = end->x - start->x;
    int endY = end->y - start->y;

    if (endX == 0)
    {
        if (endY == -1)
            return M_NORTH;
        else if (endY == 1)
            return M_SOUTH;
    }
    else if (endY == 0)
    {
        if (endX == -1)
            return M_WEST;
        else if (endX == 1)
            return M_EAST;
    }
    return M_NULL_MOVE;
}

int getManhattan(XYPos *start, XYPos *end)
{
    int xManhattan = abs(end->x - start->x);
    int yManhattan = abs(end->y - start->y);

    return xManhattan + yManhattan;
}

void addDeadEnd(XYPos *pos, int face)
{
    int x = pos->x;
    int y = pos->y;

    if (face == M_NORTH)
    {
        y = y - 1;
        Amazing[x][y].south = 1;
    }
    else if (face == M_SOUTH)
    {
        y = y + 1;
        Amazing[x][y].north = 1;
    }
    else if (face == M_WEST)
    {
        x = x - 1;
        Amazing[x][y].east = 1;
    }
    else if (face == M_EAST)
    {
        x = x + 1;
        Amazing[x][y].west = 1;
    }
}

void addWall(XYPos *pos, int face)
{
    int x = pos->x;
    int y = pos->y;

    if (face == M_NORTH)
    {
        Amazing[x][y].north = 1;
        Amazing[x][y-1].south = 1;
    }
    else if (face == M_SOUTH)
    {
        Amazing[x][y].south = 1;
        Amazing[x][y+1].north = 1;
    }
    else if (face == M_WEST)
    {
        Amazing[x][y].west = 1;
        Amazing[x-1][y].east = 1;
    }
    else if (face == M_EAST)
    {
        Amazing[x][y].east = 1;
        Amazing[x+1][y].west = 1;
    }
}