#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>
#include "structures.h"

// JUST NUMBERS TO USE, could be any number except MAX_SIZE
#define MAX_SIZE 5000
// states
#define READY 100
#define RUNNING 101
#define BLOCKED 102
// returnObj
#define RET_QUEUE 200
#define RET_PCB 201
// msg type
#define SEND 300
#define RECEIVE 301
#define REPLY 302

char *translate_type(int type);

char userInputCmd;
char *resultReport;
List *list;

char messageBuf[40];
int processId;
int priority;
int semId;
// int pStatus;
PCB *initProcess;

// place holder for every queue , to be initiated in main()
List *readyQ[3];
List *sendQ; 
List *receiveQ; 
List *messageQ; 
List *semaphoreQ; 
List *runningQ;

// don't need
int debug_msg = 1;
// don't need
int enable_printf = 1;
// NEED THEM
int numOfProcess = 0;
int procAutoIncrement = 0;

// function pointers

// THIS NOT DOING ANYTHING
// DONT NEED THIS GUY
void freemem(void *x)
{
	printf("free mem: %c\n", *((char *)x));
}

// UNDERSTOOD, ready to implement
int compareitem(void *item, void *compareArg)
{
	int *pid1, *pid2;
	pid1 = ((int *)item);
	pid2 = ((int *)compareArg);

	if (*pid1 == *pid2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// functions
// probably don't need this 'enable_printf' as this is always 1
// UNDERSTOOD, ready to implement.
void display(char *str)
{
	if (enable_printf)
	{
		printf("%s\n", str);
	}
}

// just set the current item to the first item in every queue in order to do search p efficierntly
// UNDERSTOOD. ready to implement
void resetQ_ptrs()
{
	List_first(readyQ[0]);
	List_first(readyQ[1]);
	List_first(readyQ[2]);
	List_first(sendQ);
	List_first(receiveQ);
	List_first(runningQ);
	List_first(semaphoreQ);
}

// reset the message
// UNDERSTOOD. ready to implement.
void reset_pm(PROC_MSG *pm)
{
	pm->src = -1;
	pm->dest = -1;
	strcpy(pm->body, "");
	// memset(pm->body, (int) NULL, sizeof pm->body);
}

// function to display message
// UNDERSTOOD. ready to implement.
void display_pm(PROC_MSG *pm)
{

	display("-------");
	display("Message Available:");
	printf("Type: %s\n", translate_type(pm->type));
	printf("From pid: %i - ", pm->src);
	display(pm->body);
	display("-------");
}

// UNDERSTOOD, ready to implement
char *translate_state(int state)
{
	char *str;

	if (state == READY)
	{
		str = "Ready";
	}
	else if (state == RUNNING)
	{
		str = "Running";
	}
	else if (state == BLOCKED)
	{
		str = "Blocked";
	}
	return str;
}

// UNDERSTOOD, ready to implement
char *translate_type(int type)
{
	char *str;

	if (type == SEND)
	{
		str = "Send";
	}
	else if (type == RECEIVE)
	{
		str = "Receive";
	}
	else if (type == REPLY)
	{
		str = "Reply";
	}
	return str;
}

// UNDERSTOOD. ready to implement
void CPU_scheduler()
{
	PCB *p;
	// 3 as we have 3 different ready queue.
	int num = 3;
	int i, status;

	for (i = 0; i < num; i++)
	{ // if ith ready queue is not empty,
		if (List_count(readyQ[i]))
		{
			// get the last item
			p = List_trim(readyQ[i]);
			// add to runningQ
			if (p)
			{
				status = List_append(runningQ, p);
				p->state = RUNNING;
			}

			// display status messages
			if (status == 0)
			{
				display("CPU Schedule SUCCESS");
			}
			else
			{
				display("CPU Schedule FAIL");
			}
			// exit loop
			i = 99;
		}
	}
	// set init proc state
	// if there was no process waiting, just set the init process as running one.
	if (p = List_last(runningQ))
	{
		if (p->pid == 0)
		{
			p->state = RUNNING;
		}
		printf("pid: %i now running. \n", p->pid);

		// we probably want to do this for every process that comes to running process(show the message if the currently running process has message)
		// if (strlen(p->msg->body) != 0)
		// {
		// 	display_pm(p->msg);
		// }

		// reset_pm(p->msg);
	}
}

// find pid from a specified Q. easier than having to go through every single queue to find a specified PCB.
// UNDERSTOOD, ready to implement.
void *findpid_fromQ(int pid, List *list)
{
	int (*comparator)(void *, void *);
	Node *n;

	comparator = &compareitem;
	List_first(list);
	n = List_search(list, comparator, (void *)&pid);
	if (n)
	{
		return n->pItem;
	}
	else
	{
		return NULL;
	}
}

// returnObj = 1 // return Queue
// returnObj = 2 // return PCB
// UNDERSTOOD.
void *findpid(int pid, int returnObj)
{
	int (*comparator)(void *, void *);
	List *list = NULL;
	Node *n = NULL;
	// get the comparator
	comparator = &compareitem;
	// why resetQ? just to search through. not too important
	resetQ_ptrs();
	// if the wanted pcb is there, assign the pcb to n
	if (n = List_search(readyQ[0], comparator, (void *)&pid))
	{
		list = readyQ[0];
	}
	else if (n = List_search(readyQ[1], comparator, (void *)&pid))
	{
		list = readyQ[1];
	}
	else if (n = List_search(readyQ[2], comparator, (void *)&pid))
	{
		list = readyQ[2];
	}
	else if (n = List_search(sendQ, comparator, (void *)&pid))
	{
		list = sendQ;
	}
	else if (n = List_search(receiveQ, comparator, (void *)&pid))
	{
		list = receiveQ;
	}
	else if (n = List_search(runningQ, comparator, (void *)&pid))
	{
		list = runningQ;
	}
	// if we need to search through semaphoreQ,
	else if (List_count(semaphoreQ))
	{
		SEM *s;
		int i, count;
		// get the number of semaphores
		count = List_count(semaphoreQ);
		// don't we wanna do this to List_first?
		List_prev(semaphoreQ);
		for (i = 0; i < count; i++)
		{
			// start from first node of semaphoreQ and s->list
			s = List_next(semaphoreQ);
			// get the first item waiting on the semaphore
			List_first(s->slist);
			// if that pcb is what we are looking for,
			if (n = List_search(s->slist, comparator, (void *)&pid))
			{ // set that semaphore list as returning list
				list = s->slist;
				i = count;
			}
		}
	}
	// I need to know why we need to return either list or the pcb.
	if (returnObj == RET_QUEUE)
	{
		return list;
	}
	if (returnObj == RET_PCB)
	{
		return n->pItem;
	}

	return NULL;
}

// To print all the information about the specified queue.
// UNDERSTOOD.
void display_queue(List *list)
{
	void *ptr;
	char *str;
	int id;

	List_first(list);
	List_prev(list);
	if (list == messageQ)
	{
		str = "dest";
	}
	else if (list == semaphoreQ)
	{
		str = "sid";
	}
	else
	{
		str = "pid";
	}

	printf("%s: ", str);
	while (ptr = List_next(list))
	{
		if (list == semaphoreQ)
		{
			SEM *s;
			s = (SEM *)ptr;
			id = s->sid;
		}
		else
		{
			PCB *p;
			p = (PCB *)ptr;
			id = p->pid;
		}
		printf("%i,", id);
	}
	display("");
}

// commands

bool isValidPriorityNumber(int priority){
	if (priority < 0 || priority > 2)
	{
        return false;
	}
    else{
        true;
    }
}
// maybe we don't need this pm in the parameter
// cuz when we falk, I don't want to keep the message??
// UNDERSTOOD and ready to implement
char *create(int priority)
{
	char *resultReport;
	int listSize;

	//If the user input invalid number, set it to 2
	if (!isValidPriorityNumber(priority))
	{
		priority = 2;
	}

	listSize = List_count(readyQ[priority]);

	numOfProcess++;
	//for process id auto increment 
	procAutoIncrement++;
	// allocate memory for the new PCB
	PCB *p = malloc(sizeof(PCB));
	p->pid = procAutoIncrement;
	p->priority = priority;
	p->state = READY;
	// allocate memory for the messge section in the PCB
	p->msg = (PROC_MSG *)malloc(sizeof(PROC_MSG));
	p->msg->body = (char *)malloc(sizeof(char) * 40);

//heyheyhey
	// add the newly created process to ready Q, when create a pcb, it will automatically go to readyQ
	List_prepend(readyQ[priority], p);
	// check if the list got a new item, and if so, success, if not fail.
	if (List_count(readyQ[priority]) == (listSize + 1))
	{
		printf("SUCCESS: pid%i\n", p->pid);
	}
	else
	{
		resultReport = "PROCESS CREATION FAILED";
	}
	// at the end, return the result.
	return resultReport;
}

// UNDERSTOOD.
char *fork_cmd()
{ // create report, and pointer to the pcb
	char *report;
	PCB *p;
	// get the pointer to the curent item in the running Q
	p = List_last(runningQ);
	// if the current running process is not null,
	if (p)
	{ // create a new pcb and put that in the ready Q
		// report = create(p->priority, p->msg);
		report = create(p->priority);
	}
	// finally return the result report
	return report;
}
// UNDERSTOOD, ready to implement
char *kill(int pid, List *list)
{
	char *report;
	PCB *p;
	int fail = 1;

	if (list)
	{ // this will remove the current item in the specififed list.
		p = List_remove(list);
		fail = 0;
	}

	if (p && !fail)
	{
		// if success, free the memory of the removed pcb.
		display("SUCCESS");
		printf("pid:%i removed\n", p->pid);
        if(p->pid !=0){
		free(p->msg->body);
		free(p->msg);
		free(p);
		p->msg->body = 0;
		p->msg = 0;
		p = 0;
		numOfProcess--;
        }
	}
	else
	{
		display("FAIL");
	}

	return report;
}

// with "e", kill the current running process.
// UNDERSTOOD. ready to implement
char *performExit()
{
	char *report;
	PCB *p;
	// get the currentrunning process
	p = List_last(runningQ);
	// this condition check might be wrong. why numProc == 0?
	//  if numOfProcess ==0, which means we only have init process(kernal)
	// so this 'e' will work to terminate the whole program if no process. try it
	// so i guess when init process is created, we don't do numOfProcess++??
	if (numOfProcess == 0 || p->pid != 0)
	{
		// remove the currently running pcb
		p = List_trim(runningQ);
		printf("pid:%i removed\n", p->pid);
		// free memory
        if(p->pid !=0){
		free(p->msg->body);
		free(p->msg);
		free(p);
		p->msg->body = 0;
		p->msg = 0;
		p = 0;
		numOfProcess--;
		// get another process from readyQ running.
		CPU_scheduler();
        }
        else{
		printf("init core process killed. Terminating the program.\n");
        }
	}
	else
	{
		display("Cannot exit init process.  Other processes waiting.");
	}
	return report;
}
// UNDERSTOOD
void performQuantum()
{
	PCB *p;

	// take item out of runningQ
	p = List_last(runningQ);
	// if the removed process is just a normal pcb (not init process)
	if (p->pid != 0)
	{
		p = List_trim(runningQ);
		// descrease priority
		if (p->priority < 2)
		{
			p->priority++;
		}
		// push the p into the ready queue to the front of the ready Q.
		p->state = READY;
		// place on readyQ
		if (p)
		{
			List_prepend(readyQ[p->priority], p);

			printf("pid: %i placed on ready queue.\n", p->pid);
			// printf("priority: %i\n", p->priority);
		}
	}
	// why do we need this? what happen if we 'q' when no process
	// we don't need this block.
	//  else
	//  {
	//  	// reset init proc state
	//  	p->state = READY;
	//  }

	// run cpu scheduler
	CPU_scheduler();
}

// UNDERSTOOD, ready to implement
void send(int pid, char *msg)
{
	PCB *run, *p;
	PROC_MSG *pm;
	int src;
	int fail = 0;
	// get the currently running process
	run = List_last(runningQ);
	// get the pid of the process in order to include it in the message
	src = run->pid;

	// if the receiver of the message is already waiting in the receive Queue,
	if (p = findpid_fromQ(pid, receiveQ))
	{
		// update the messge of the process
		p->msg->src = src;
		p->msg->dest = pid;
		p->msg->type = SEND;
		// what does this strcpy do? ok just putting it
		strcpy(p->msg->body, msg);
		// take out of receiveQ
		p = List_remove(receiveQ);
		// add to readyQ
		p->state = READY;
		List_prepend(readyQ[p->priority], p);
	} // else if we can find the pcb in somewhere else(which is not blocked)
	else if (findpid(pid, RET_QUEUE))
	{ // allocate memory for the message
		pm = malloc(sizeof pm);
		// set the message to the message
		pm->body = (char *)malloc(sizeof(char) * 40);
		pm->src = src;
		pm->dest = pid;
		pm->type = SEND;
		strcpy(pm->body, msg);
		// put msg on messageQ
		List_insert_after(messageQ, pm);
		run = List_last(runningQ);
		if (run->pid != 0)
		{
			// blk sender
			// by getting the process running,
			p = List_trim(runningQ);
			// and block
			p->state = BLOCKED;
			// and put that into sendQ
			List_insert_after(sendQ, p);
			// and let the last item in the ready queue running
			CPU_scheduler();
		}
	} // if we cann't find the specified pcb, set it as fail
	else
	{
		fail = 1;
	}

	if (!fail)
	{
		display("SEND SUCCESSFUL");
	}
	else
	{
		display("SEND FAIL");
	}
}

// RECEIVE
// UNDERSTOOD
void performReceive()
{
	PROC_MSG *pm;
	PCB *p;
	int dest;
	// get the currently running item
	p = List_last(runningQ);
	dest = p->pid;
	// chk messageQ
	// show msg, src
	// if there's already a message waiting to be received for this guy,
	if (pm = findpid_fromQ(dest, messageQ))
	{ // display the message, and remove the messge from queue
		display_pm(pm);
		List_remove(messageQ);
	}
	// if no message available for this guy, then block
	else
	{
		// put process on receiveQ
		// cpu sched
		if (p->pid != 0)
		{ // take it out of running Q and then put that in receive queue
			p = List_trim(runningQ);
			p->state = BLOCKED;
			List_prepend(receiveQ, p);
			// and let the next one run
			CPU_scheduler();
		}
	}
}
// REPLY
// UNDERSTOOD how it's working.
void reply_cmd(int pid, char *msg)
{
	PCB *run, *p;
	// get the running process cuz he's wanting to reply, so we want his pid
	run = List_last(runningQ);
	// if the current process wants to reply to currently blocked sender,
	if (p = findpid_fromQ(pid, sendQ))
	{
		// copy msg to sender's pcb
		p->msg->dest = pid;
		p->msg->src = run->pid;
		p->msg->type = REPLY;
		strcpy(p->msg->body, msg);

		// unblock the blocked sender in sendQ
		p = List_remove(sendQ);
		p->state = READY;
		// and insert the unblocked sender to the appropriete ready queue.
		List_prepend(readyQ[p->priority], p);

		display("REPLY SUCCESS");
	}
	// I think i need to modify here, cuz
	// a process can reply to a process that are not waiting at all.
	// FIX HERE.
	else
	{
		// fail
		display("REPLY FAIL");
	}
}

//'n' to create a new semaphore.
// UNDERSTOOD, ready to implement
void new_sem(int sid)
{
	// init sem
	SEM *s;
	int fail = 1;
	// if sid 0-5 AND does not already exist;
	if (sid >= 0 && sid <= 4)
	{
		if (!findpid_fromQ(sid, semaphoreQ))
		{
			s = malloc(sizeof(SEM));
			s->sid = sid;
			s->value = 0;
			s->slist = List_create();
			List_insert_after(semaphoreQ, s);

			fail = 0;
		}
	}

	if (!fail)
	{
		display("SUCCESS");
		printf("SEM %i initialized\n", sid);
	}
	else
	{
		display("FAIL");
	}
}

// UNDERSTOOD
void P(int sid)
{
	SEM *s;
	PCB *p;
	int fail = 1;
	int block = 0;

	p = List_last(runningQ);
	s = findpid_fromQ(sid, semaphoreQ);
	if (s && p->pid != 0)
	{
		s->value--;
		if (s->value < 0)
		{
			p = List_trim(runningQ);
			p->state = BLOCKED;
			List_prepend(s->slist, p);
			fail = 0;
			block = 1;
		}
	}

	if (fail)
	{
		display("FAIL");
	}
	else
	{
		display("SUCCESS");
		if (block)
		{
			printf("pid: %i BLOCKED\n", p->pid);
		}
		CPU_scheduler();
	}
}

// UNDERSTOOD
void V(int sid)
{
    SEM *s;
    PCB *p;
    int fail = 1;
    int ready_q = 0;

	

	if (fail)
	{
		display("FAIL");
	}
	else
	{
		display("SUCCESS");
		if (ready_q)
		{
			printf("pid: %i into READY QUEUE\n", p->pid);
		}
	}
}

// UNDERSTOOD
void procinfo(int pid)
{
	PCB *p;
	char *state;

	p = (PCB *)findpid(pid, RET_PCB);
	if (p)
	{

		state = translate_state(p->state);
		display("=========");
		printf("Proc Info for pid: %i\n", p->pid);
		printf("Priority: %i\n", p->priority);
		printf("State: %s\n", state);
		printf("Msg: %s\n", p->msg->body);
		display("=========");
	}
	else
	{
		printf("Cannot get Proc Info");
	}
}

// UNDERSTOOD
void totalinfo()
{
	PCB *p;
	char *state;
	int i, count;

	// runningQ
	p = List_last(runningQ);
	state = translate_state(p->state);
	display("=========");
	display("RUN QUEUE");
	printf("pid: %i\n", p->pid);

	// readyQ
	display("READY QUEUES");
	for (i = 0; i < 3; i++)
	{
		printf("Priority [%i]:\n", i);
		display_queue(readyQ[i]);
	}

	// sendQ
	display("SEND QUEUE");
	display_queue(sendQ);
	display("RECEIVE QUEUE");
	display_queue(receiveQ);
	display("MSG QUEUE");
	display_queue(messageQ);

	display("SEM QUEUES");
	if (count = List_count(semaphoreQ))
	{
		SEM *s;

		List_first(semaphoreQ);
		List_prev(semaphoreQ);
		for (i = 0; i < count; i++)
		{
			s = List_next(semaphoreQ);
			printf("sid%i: ", s->sid);
			display_queue(s->slist);
		}
	}

	// display("P() QUEUE");
	// display("V() QUEUE");
	display("=========");
}

void setInitProcess(){
    initProcess = malloc(sizeof(PCB));
    initProcess->pid = 0;
    initProcess->priority = 0;
    initProcess->state = RUNNING;
    List_insert_after(runningQ, initProcess);
}

void initQueues(){
    runningQ = List_create();
    readyQ[0] = List_create();
    readyQ[1] = List_create();
    readyQ[2] = List_create();
    sendQ = List_create();
    receiveQ = List_create();
    messageQ = List_create();
    semaphoreQ = List_create();
}
void performCreate(){
    printf("Specify Process Priority (0, 1, 2): ");
    scanf("%d", &priority);
    resultReport = create(priority);
}
void performKill(){
    printf("Enter the pid to kill: ");
    scanf("%d", &processId);
    if(processId== 0 && numOfProcess !=0){
        printf("\n You can't kill the init core process.\n");
    }else{
        list = findpid(processId, RET_QUEUE);
        resultReport = kill(processId, list);
        if(list == runningQ){
            CPU_scheduler();
        }
    }
}

void performSend(){
    printf("Enter the pid to send message to: ");
    scanf("%d", &processId);
    printf("Enter message: ");
    scanf(" %[^\n]", messageBuf);
    send(processId, messageBuf);
}

void performReply(){
    printf("Enter the pid to reply to: ");
    scanf("%d", &processId);
    printf("Enter reply message: ");
    scanf(" %[^\n]", messageBuf);
    reply_cmd(processId, messageBuf);
}

void performNewSemaphore(){
    printf("Enter SEM id to create: ");
    scanf("%d", &semId);
    new_sem(semId);
}

void performP(){
    printf("Enter SEM id to apply P: ");
    scanf("%d", &semId);
    P(semId);
}

void performV(){
    printf("Enter SEM id to apply V: ");
    scanf("%d", &semId);
    V(semId);
}

void performProcInfo(){
    printf("Enter process id to see details: ");
    scanf("%d", &processId);
    procinfo(processId);
}

void promptUser(){

    // bool isInitProcessAlive = (List_count(runningQ)>0);
    printf("isInitProcessAlive is %d", List_count(runningQ)>0);
    while(List_count(runningQ)>0){

        printf("Enter command :");
        scanf("%c", &userInputCmd);
        userInputCmd = toupper(userInputCmd);

        if(userInputCmd =='C'){
            performCreate();
            // break;
        }
        else if(userInputCmd =='F'){
            resultReport = fork_cmd();
            // continue;
        }
        else if(userInputCmd =='K'){
            performKill();
            // break;
        }
        else if(userInputCmd =='E'){
            performExit();
            // break;
        }
        else if(userInputCmd =='Q'){
            performQuantum();
            // break;
        }
        else if(userInputCmd =='S'){
            performSend();
            // break;
        }
        else if(userInputCmd =='R'){
            performReceive();
            // break;
        }
        else if(userInputCmd =='Y'){
            performReply();
            // break;
        }
        else if(userInputCmd =='N'){
            performNewSemaphore();
            // break;
        }
        else if(userInputCmd =='P'){
            performP();
            // break;
        }
        else if(userInputCmd =='V'){
            performV();
            // break;
        }
        else if(userInputCmd =='I'){
            performProcInfo();
            // break;
        }
        else if(userInputCmd =='T'){
            totalinfo();
            // break;
        }
        else{
            // break;
        }

    }

}

// UNDERSTOOD
int main(int argc, const char *argv[])
{

    initQueues();
    setInitProcess();


    promptUser();


	return 0;

}