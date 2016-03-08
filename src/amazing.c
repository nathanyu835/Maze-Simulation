

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

/* This method initializes out maze Amazing, which will be a 2D array of MazeNodes. For each MazeNode, the four walls will be initially
 * set to 0, and will be updated to 1 as walls are discovered. We also initialize a visited list for each MazeNode that will keep track
 * of how many times each avatar has visited a location in the maze. This method also calls createPerimeter */
void initializeMaze(int height, int width, int nAvatars)
{
    MazeNode *temp = (MazeNode*)calloc(width*height, sizeof(MazeNode));
    Amazing = (MazeNode**)calloc(width, sizeof(MazeNode*));
    for (int i = 0; i < width; i++)
        Amazing[i] = temp + i*height;

    for(int i = 0; i < width; i++)
        for(int j = 0; j < height; j++)
        {
            Amazing[i][j].visited = malloc(sizeof(struct MazeNode) + (nAvatars));
            for(int k = 0; k < nAvatars; k++)
            {
                Amazing[i][j].visited[k] = 0;
            }
            Amazing[i][j].north = 0;
            Amazing[i][j].east = 0;
            Amazing[i][j].south = 0;
            Amazing[i][j].west = 0;
            Amazing[i][j].whoLast = -1;
            Amazing[i][j].lastDir = -1;
        }

    createPerimeter(height, width);
}

/* This method creates the walls on the wedge of the maze utilizing the height and width of the maze */
void createPerimeter(int height, int width)
{
    for(int i = 0; i < width; i++)
    {
        Amazing[i][0].north = 1;
        Amazing[i][height-1].south = 1;
    }

    for(int j = 0; j < height; j++)
    {
        Amazing[0][j].west = 1;
        Amazing[width-1][j].east = 1;
    }
}

/* This method frees all visited lists in MazeNodes, MazeNodes and the Amazing maze */
void freeMaze()
{
    for(int i = 0; i < mazeWidth; i++)
        for(int j = 0; j < mazeHeight; j++) {
            free(Amazing[i][j].visited);
        }
    free(*Amazing);
    free(Amazing);
}

/* This method  */
void visitSquare(XYPos *currPos, int i)
{
    Amazing[currPos->x][currPos->y].visited[i]++;
}

/* This method checks for a path that decreases the MD between currPos and renedezvous, prioritizing a decrease in abs(y) over a decrease in abs(x).
 * If no moves decreade the MD, this method prioritizes traversing MazeNodes that have not yet been visited.It returns the direction of a potential 
 * productive move, -1 if one doesn't exist. */
int isProductive(XYPos *currPos, int i)
{
    //int currMD = getManhattan(currPos, rendezvous);
    int changeX = rendezvous->x - currPos->x;
    int changeY = rendezvous->y - currPos->y;

    if(changeY > 0) //Avatar needs to move south to reach rendezvous
    {
        if(Amazing[currPos->x][currPos->y].south == 0 && Amazing[currPos->x][currPos->y+1].visited[i] == 0)
            return M_SOUTH;
    }
    if(changeY < 0)
    {
        if(Amazing[currPos->x][currPos->y].north == 0 && Amazing[currPos->x][currPos->y-1].visited[i] == 0)
            return M_NORTH;
    }
    if(changeX > 0) //Avatar needs to move east to reach rendezvous
    {
        if(Amazing[currPos->x][currPos->y].east == 0 && Amazing[currPos->x+1][currPos->y].visited[i] == 0)
            return M_EAST;    
    }
    if(changeX < 0)
    {
        if(Amazing[currPos->x][currPos->y].west == 0 && Amazing[currPos->x-1][currPos->y].visited[i] == 0)
            return M_WEST;
    }
    if(Amazing[currPos->x][currPos->y].south == 0 && Amazing[currPos->x][currPos->y+1].visited[i] == 0)
        return M_SOUTH;
    if(Amazing[currPos->x][currPos->y].north == 0 && Amazing[currPos->x][currPos->y-1].visited[i] == 0)
        return M_NORTH;
    if(Amazing[currPos->x][currPos->y].east == 0 && Amazing[currPos->x+1][currPos->y].visited[i] == 0)
        return M_EAST;
    if(Amazing[currPos->x][currPos->y].west == 0 && Amazing[currPos->x-1][currPos->y].visited[i] == 0)
        return M_WEST;
    return -1;
}

/* This method is what is called to return a direction to the avatar making a move. First we check for a productive path.
 * If one doesn't exist, we know that every possibile move have been visited once by the avatar. We then arbitrarily choose
 * a direction to traverse, and the we block off the twice visited MazeNode by calling addDeadEnd.*/
int getMove(XYPos *currPos, int i)
{
    if(currPos->x == rendezvous->x && currPos->y == rendezvous->y)
        return M_NULL_MOVE;
    int productive = isProductive(currPos, i);
    if(productive != -1)
        return productive;

    if(Amazing[currPos->x][currPos->y].west == 0)
    {
        addDeadEnd(currPos, M_WEST);
        return M_WEST;
    }
    if(Amazing[currPos->x][currPos->y].east == 0)
    {
        addDeadEnd(currPos, M_EAST);
        return M_EAST;
    }
    if(Amazing[currPos->x][currPos->y].north == 0)
    {
        addDeadEnd(currPos, M_NORTH);
        return M_NORTH;
    }
    if(Amazing[currPos->x][currPos->y].south == 0)
    {
        addDeadEnd(currPos, M_SOUTH);
        return M_SOUTH;
    }
    printf("Error, stuck in a box\n");
    return 0;
}

// This method returns the direction an avatar moved by taking in its previous location and its current location
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

// This method calculates the MD between two nodes
int getManhattan(XYPos *start, XYPos *end)
{
    int xManhattan = abs(end->x - start->x);
    int yManhattan = abs(end->y - start->y);

    return xManhattan + yManhattan;
}

/* This method will add a one-way wall for MazeNodes that are twice visited. */
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

/* This method will update the Amazing maze by adding a wall into the maze whenever onne is discovered */
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
