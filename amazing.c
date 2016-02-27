

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

/*
We will mark the path that each avatar has traveled (update visited in AvataerInfo). If a location is twice visited, we will block it off (dead end) 
*/


// we will use a 2d array of size DIFFICULTY of Avatar Information for each Avatar
typedef struct AvatarInfo {
    int visited; // stores the number of times a location has been visited
    List *directionsTried; // stores the directions already attempted but failed because of a wall
} AvatarInformation;


// Basic pseudocode for our traversal: Find target location based on average location of Avatars.
// We then find the Manhattan Distance between each avatar and the location, which is the sum of the ABS of the x distance and y distance to location.

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
}