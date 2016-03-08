

/* ========================================================================== */
/* File: amazing.c - Amazing Project
 *
 * Author: Nathan Yu, Ian Delaney, Stephanie Guo
 * Date: February 27, 2015
 *
 * Input: [NUM_AVATARS] [NUM_DIFFICULTY] [HOSTNAME]
 *
 * Output: graphical representation of maze as avatars traverse it - printed to stdout
 *         logfiles in testing directory of root directory
 *
 * Error Conditions: invalid arguments at command line (see README)
 *
 * Special Considerations: amazing must be run with the flume.cs.dartmouth.edu server
 *                         using the [HOSTNAME] value 129.170.214.115
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

/*
 * initializeMaze - creates and initializes the maze Amazing
 *                  sets walls to 0, and and updates to 1 as they are discovered.
 *                  creates a 'visited' list for each MazeNode that will keep track of
 *                      how many times each avatar has visited a location in the maze.
 *                  calls createPerimeter
 *
 * @height: height of the maze
 * @width: width of the maze
 * @nAvatars: number of avatars in the maze
 *
 */
void initializeMaze(int height, int width, int nAvatars)
{
    MazeNode *temp = (MazeNode*)calloc(width*height, sizeof(MazeNode));
    
    // Amazing is a 2D array of MazeNodes
    Amazing = (MazeNode**)calloc(width, sizeof(MazeNode));
    for (int i = 0; i < width; i++)
        Amazing[i] = temp + i*height;

    // Populate Amazing with unvisited MazeNodes with blank walls
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
    // Set the walls of each MazeNode in Amazing
    createPerimeter(height, width);
}

/*
 * createPerimeter - creates the walls on the edge of the maze
 *                   using the height and width of the maze 
 *
 * @height: height of the maze
 * @width: width of the maze
 *
 */
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

/* 
 * freeMaze - frees all visited lists in MazeNodes, MazeNodes and the Amazing maze
 *
 * */
void freeMaze()
{
    for(int i = 0; i < mazeWidth; i++)
        for(int j = 0; j < mazeHeight; j++) {
            free(Amazing[i][j].visited);
            //free(&Amazing[i][j]);
        }
    //for (int i = 0; i < mazeWidth; i++)
    free(*Amazing);
    free(Amazing);
}

/*
 * visitSquare - increases counter on visits to the given MazeNode
 * 
 * @currPos: XYPos of the given MazeNode
 * @i: avatar id of the avatar whose visits counter is being incremented
 *
 */
void visitSquare(XYPos *currPos, int i)
{
    Amazing[currPos->x][currPos->y].visited[i]++;
}

/*
 * isProductive - checks for a path that decreases the MD between currPos and rendezvous,
 *                prioritizing a decrease in abs(y) over a decrease in abs(x).
 *                If no moves decreade the MD, this method prioritizes traversing MazeNodes
 *                that have not yet been visited.
 *
 * @currPos: XYPos of the given MazeNode
 * @i: avatar id of the avatar currently moving
 *
 * Returns: int direction of a potential productive move, -1 if one doesn't exist.
 *
 */
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
    {
        return M_SOUTH;
    }
    if(Amazing[currPos->x][currPos->y].north == 0 && Amazing[currPos->x][currPos->y-1].visited[i] == 0)
        return M_NORTH;
    if(Amazing[currPos->x][currPos->y].east == 0 && Amazing[currPos->x+1][currPos->y].visited[i] == 0)
        return M_EAST;
    if(Amazing[currPos->x][currPos->y].west == 0 && Amazing[currPos->x-1][currPos->y].visited[i] == 0)
        return M_WEST;
    return -1;
}

/*
 * getMove - Determines direction that avatar moves in by checking for a productive path.
 *           If one doesn't exist, we know that every possible move have been visited once
 *           by the avatar. We then arbitrarily choose a direction to traverse, and block off
 *           the twice visited MazeNode by calling addDeadEnd.
 *
 * @currPos: XYPos of the given MazeNode
 * @i: avatar id of the avatar currently moving
 *
 * Return: int direction 0, 1, 2, 3, 8 for avatar to move in
 */
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

/*
 * getDirection - finds direction an avatar moved by taking
 *                its previous location and its current location
 * @start: XYPos indicating the starting position of the avatar
 * @end: XYPos indicating the ending position of the avatar
 *
 * Return: int direction that the avatar has just moved in
 *
 */
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

/* 
 * getManhattan - calculates the MD (Manhattan Distance) between two nodes
 * 
 * @start: XYPos indicating the starting position of the avatar
 * @end: XYPos indicating the ending position of the avatar
 *
 * Return: int representing MD between two MazeNodes
 *
 */
int getManhattan(XYPos *start, XYPos *end)
{
    int xManhattan = abs(end->x - start->x);
    int yManhattan = abs(end->y - start->y);

    return xManhattan + yManhattan;
}

/*
 * addDeadEnd - adds a one-way wall for MazeNodes that are twice visited
 *
 * @pos: XYPos that holds position of the given MazeNode
 * @face: direction that the current avatar is facing
 *
 */
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

/*
 * addWall - updates the Amazing maze by adding new walls into the maze
 *
 * @pos: XYPos of MazeNode containing the current wall
 * @face: int direction faced by the current avatar
 *
 */
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
