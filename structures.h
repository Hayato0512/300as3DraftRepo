/*
 *  structures.h
 *  A3
 *
 *  Created by kp on 21/03/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "list.h"

// JUST NUMBERS TO USE, could be any number except MAX_SIZE
#define MAX_SIZE 5000
// states
#define READY 1
#define RUNNING 2
#define BLOCKED 3

// msg type
#define SEND 4
#define RECEIVE 5
#define REPLY 6

typedef struct proc_msg PROC_MSG;
struct proc_msg
{
	int dest, src, type;
	char *body;
};

typedef struct pcb PCB;
struct pcb
{
	int pid, priority, state;
	PROC_MSG *msg;
};

typedef struct sem SEM;
struct sem
{
	int sid, value;
	List *slist;
};
