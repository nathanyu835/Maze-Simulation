/* ========================================================================== */
/* File: amazing.h
 *
 * Project name: CS50 Amazing Project
 * Component name: amazing header
 *
 * Primary Author: Charles C. Palmer
 * Date Created: Mon Jan 2 2012
 *
 * Last Update by: Ira Ray Jenkins - Sun May 11, 2014
 *
 * This file contains the common defines for the Amazing Project.
 *
 */
/* ========================================================================== */
#ifndef AMCLIENT_H
#define AMCLIENT_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include <stdint.h>                          // uint32_t
#include "amazing.h"

// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros

void* newAvatar(void *newAvatar);
void AMClient();
int chooseDir(Avatar *avatar, XYPos *rendezvous);

 #endif // AMCLIENT.H