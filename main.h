#include "list.h"

#define MAX_SIZE 200
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
	int dest; 
	int src; 
	int type;
	char *body;
};

typedef struct pcb PCB;
struct pcb
{
	int pid; 
	int priority; 
	int state;
	PROC_MSG *msg;
};

typedef struct sem SEM;
struct sem
{
	int sid; 
	int value;
	List *slist;
};
