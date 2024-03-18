#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>
#include "structures.h"
#include "list.h"


char userInputCmd;
char *resultReport;
List *list;

char messageBuf[40];
int processId;
int priority;
int semId;
PCB *initProcess;

// place holder for every queue , to be initiated in main()
List *readyQ[3];
List *sendQ; 
List *receiveQ; 
List *messageQ; 
List *semaphoreQ; 
List *runningQ;

int numOfProcess = 0;
int procAutoIncrement = 0;



// UNDERSTOOD, ready to implement
int compareProcesses(void *item, void *compareArg)
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

// just set the current item to the first item in every queue in order to do search p efficierntly
// UNDERSTOOD. ready to implement
//IMPLEMENTED
void resetQueuesForSearch()
{
	List_first(runningQ);
	List_first(readyQ[0]);
	List_first(readyQ[1]);
	List_first(readyQ[2]);
	List_first(sendQ);
	List_first(receiveQ);
	List_first(semaphoreQ);
}

// reset the message
// UNDERSTOOD. ready to implement.
void resetMessage(PROC_MSG *pm)
{
	pm->src = -1;
	pm->dest = -1;
	strcpy(pm->body, "");
}

// UNDERSTOOD, ready to implement
//IMPLEMENTED
char *getMessageTypeName(int type)
{
	char *returnString;

	if (type == SEND)
	{
		returnString = "Send";
	}
	else if (type == RECEIVE)
	{
		returnString = "Receive";
	}
	else if (type == REPLY)
	{
		returnString = "Reply";
	}
	return returnString;
}

// function to display message
// UNDERSTOOD. ready to implement.
//IMPLEMENNTED
void printMessage(PROC_MSG *pm)
{

	printf("\n-------\n");
	printf("\nMessage Available:\n");
	printf("Type: %s\n", getMessageTypeName(pm->type));
	printf("From pid: %i - ", pm->src);
	printf("\n");
	printf(pm->body);
	printf("\n");
	printf("\n-------\n");
}

// UNDERSTOOD, ready to implement
//IMPLEMENTED
char *getStateName(int state)
{
	char *returnString;

	if (state == READY)
	{
		returnString = "Ready";
	}
	else if (state == RUNNING)
	{
		returnString = "Running";
	}
	else if (state == BLOCKED)
	{
		returnString = "Blocked";
	}
	return returnString;
}


//ORIGINAL
bool isListNotEmpty(List *list){
	if(List_count(list)!=0){
		return true;
	}
	else{
		return false;
	}
}
// UNDERSTOOD. ready to implement
// IMPLEMENTED
void runNextProcess()
{
	if(numOfProcess!=0){
		PCB *p;
		// 3 as we have 3 different ready queue.
		int num = 3;
		int i;

		for (i = 0; i < num; i++)
		{ // if ith ready queue is not empty,
			if (isListNotEmpty(readyQ[i]))
			{
				// get the last item
				p = List_trim(readyQ[i]);
				// add to runningQ
				if (p)
				{
					List_append(runningQ, p);
					p->state = RUNNING;
					printf("runNextProcess SUCCESS");
				}
				else{
					printf("runNextProcess FAIL");
				}
				// exit loop
				i = 99;
			}
		}
		// set init proc state
		// if there was no process waiting, just set the init process as running one.
		// if (p = List_last(runningQ))
		// {
			if (p->pid == 0)
			{
				p->state = RUNNING;
			}
			printf("pid: %i now running. \n", p->pid);

			// we probably want to do this for every process that comes to running process(show the message if the currently running process has message)
			// if (strlen(p->msg->body) != 0)
			// {
			// 	printMessage(p->msg);
			// }

			// resetMessage(p->msg);
		// }

	}
	else{
		printf("Only the initial process is running. cannot quantum.\n");
	}
}

// find pid from a specified Q. easier than having to go through every single queue to find a specified PCB.
// UNDERSTOOD, ready to implement.
//IMPLEMENTED
PCB *findQueuedProcess(int pid, List *list)
{
	int (*comparator)(void *, void *);
	Node *n;

	comparator = &compareProcesses;
	List_first(list);
	n = List_search(list, comparator, (void *)&pid);

	if (n)
	{
		printf("findProcess> N is not NULLLLLL, but pItem might be null");
		if(n->pItem != NULL){
		printf("findProcess>  pItem IS NOT NULL");
		}
		return &n->pItem;
	}
	else
	{
		printf("in findQueuedProcess, n is null");
		return NULL;
	}
}

//ORIGINAL
SEM *findSemaphore(int pid, List *list)
{
	int (*comparator)(void *, void *);
	Node *n = NULL;

	comparator = &compareProcesses;
	List_first(list);
	n = List_search(list, comparator, (void *)&pid);

	if (n)
	{
		printf("findProcess> N is not NULLLLLL, but pItem might be null");
		if(n->pItem != NULL){
		printf("findProcess>  pItem IS NOT NULL");
		}
		// SEM * result = (SEM *) n->pItem;
		// if(result !=NULL){
		// 	printf("result semahpre we got is %d", result->sid);
		// }
		// return result;
		return &n->pItem;
	}
	else
	{
		printf("in findQueuedProcess, n is null");
		return NULL;
	}
}

//IMPLEMENTED
void *findListFromProcess(int processId){

	int (*comparator)(void *, void *);
	List *list = NULL;
	Node *n = NULL;

	if (n = List_search(readyQ[0], comparator, (void *)&processId))
	{
		list = readyQ[0];
	}
	else if (n = List_search(readyQ[1], comparator, (void *)&processId))
	{
		list = readyQ[1];
	}
	else if (n = List_search(readyQ[2], comparator, (void *)&processId))
	{
		list = readyQ[2];
	}
	else if (n = List_search(sendQ, comparator, (void *)&processId))
	{
		list = sendQ;
	}
	else if (n = List_search(receiveQ, comparator, (void *)&processId))
	{
		list = receiveQ;
	}
	else if (n = List_search(runningQ, comparator, (void *)&processId))
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
			if (n = List_search(s->slist, comparator, (void *)&processId))
			{ // set that semaphore list as returning list
				list = s->slist;
				i = 99;
			}
		}
	}
	return list;

}
// returnObj = 1 // return Queue
// returnObj = 2 // return PCB
// UNDERSTOOD.
void *findProcess(int pid)
{
	printf("\nhey 1 findProcess I WILL FI ND PID %d\n", pid);
	int (*comparator)(void *, void *);
	List *list = NULL;
	Node *n;
	// get the comparatoList_searchr
	comparator = &compareProcesses;
	// why resetQ? just to search through. not too important
	resetQueuesForSearch();
	printf("\nhey 2 findProcess\n");
	// if the wanted pcb is there, assign the pcb to n
	if (n = List_search(readyQ[0], comparator, (void *)&pid))
	{
	printf("\nhey 3 findProcess\n");
		printf("found the specified pid in readyQ 0");
		// list = readyQ[0];
	return &n->pItem;
	}
	else if (n = List_search(readyQ[1], comparator, (void *)&pid))
	{
	printf("\nhey 4 findProcess.\n");
	if(n == NULL){
	printf("\nhey 4 findProcess. N IS NULL\n");
	return NULL;		
	}	
	if(n->pItem ==NULL){
	printf("\nhey 4 findProcess. N->pItem IS NULL\n");
	return NULL;		
	}	
	return &n->pItem;
	}
	else if (n = List_search(readyQ[2], comparator, (void *)&pid))
	{
	printf("\nhey 5 findProcess\n");
		printf("found the specified pid in readyQ 2");
		// list = readyQ[2];
	return &n->pItem;
	}
	else if (n = List_search(sendQ, comparator, (void *)&pid))
	{
	printf("\nhey 6 findProcess\n");
		printf("found the specified pid in sendQ");
		// list = sendQ;
	return &n->pItem;
	}
	else if (n = List_search(receiveQ, comparator, (void *)&pid))
	{
	printf("\nhey 7 findProcess\n");
		printf("found the specified pid in receiveQ");
		// list = receiveQ;
	return &n->pItem;
	}
	else if (n = List_search(runningQ, comparator, (void *)&pid))
	{
	printf("\nhey 8 findProcess\n");
		printf("found the specified pid in runningQ");
		// list = runningQ;
	return &n->pItem;
	}
	// if we need to search through semaphoreQ,
	else if (List_count(semaphoreQ))
	{
	printf("\nhey 9 findProcess\n");
		SEM *s;
		int i, count;
		// get the number of semaphores
		count = List_count(semaphoreQ);
		// don't we wanna do this to List_first?
		List_prev(semaphoreQ);
	printf("\nhey 99 findProcess\n");
		for (i = 0; i < count; i++)
		{
			// start from first node of semaphoreQ and s->list
			s = List_next(semaphoreQ);
			// get the first item waiting on the semaphore
			List_first(s->slist);
			// if that pcb is what we are looking for,
			if (n = List_search(s->slist, comparator, (void *)&pid))
			{ // set that semaphore list as returning list
		printf("found the specified pid in semaphoreQ");
				// list = s->slist;
				i = count;
	return &n->pItem;
			}
		}
	printf("\nhey 199 findProcess\n");
	}
	// I need to know why we need to return either list or the pcb.
	printf("\nhey 1999 findProcess\n");
	if(n ==NULL){
	printf("\nhey 1999 n NULLLLLLLL\n");
	}
	//but first of all, why not find pid 2? 
	//N IS NULL
	// if(n->pItem ==NULL){
	// printf("\nhey 1999 n->pItem NULLLLLLLL\n");
	// }
	return NULL;

	// return n->pItem;
}

//ORIGINAL
char *getItemCategory(List *list){
	if (list == messageQ)
	{
		return "destination";
	}
	else if (list == semaphoreQ)
	{
		return "sid";
	}
	else
	{
		return "pid";
	}
}

// To print all the information about the specified queue.
// UNDERSTOOD.
//IMPLEMENTED
void printQueue(List *list)
{
	void *pointer;
	char *str;
	int id;

	List_first(list);
	List_prev(list);
	str = getItemCategory(list);

	printf("%s: ", str);
	while (pointer = List_next(list))
	{
		if (list == semaphoreQ)
		{
			SEM *s;
			s = (SEM *)pointer;
			id = s->sid;
		}
		else
		{
			PCB *p;
			p = (PCB *)pointer;
			id = p->pid;
		}
		printf("%i,", id);
	}
	printf("\n\n");
}

// commands
//ORIGINAL
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
char *forkProcess()
{ // create report, and pointer to the pcb
	char *resultReport;
	PCB *currentProcess;
	// get the pointer to the curent item in the running Q
	// currentProcess = List_last(runningQ);
	// if the current running process is not null,
	if (currentProcess = List_last(runningQ))
	{ // create a new pcb and put that in the ready Q
		// report = create(p->priority, p->msg);
		resultReport = create(currentProcess->priority);
	}
	else{
		printf("FORK FAILED");
		resultReport = "FORK FAILED";
	}
	// finally return the result report
	return resultReport;
}
// UNDERSTOOD, ready to implement
char *killProcess(int pid, List *list)
{
	char *resultReport;
	PCB *p;
	printf("\nhey 1");
	if (list)
	{ // this will remove the current item in the specififed list.and current item is what we want to killProcess
	//cuz after performingi list_search, the current pointer is pointing to the searched item. 
	printf("\nhey 2");
		p = List_remove(list);
		resultReport = "SUCCESS";
		printf("pid:%i removed\n", p->pid);
        if(p->pid !=0){
		free(p->msg->body);
		free(p->msg);
		free(p);
		// p->msg->body = 0;
		// p->msg = 0;
		// p = 0;
		numOfProcess--;
        }
	}
	else{
	printf("\nhey 3");
		resultReport = "FAIL";
	}

	return resultReport;
}

// with "e", killProcess the current running process.
// UNDERSTOOD. ready to implement
//IMPLEMENTED
char *performExit()
{
	char *resultReport;
	PCB *currentProcess;
	// get the currentrunning process
	currentProcess = List_last(runningQ);

	if (currentProcess)
	{
		// remove the currently running pcb
		currentProcess = List_trim(runningQ);
		printf("pid:%i removed\n", currentProcess->pid);
		// free memory
        if(currentProcess->pid !=0){
		free(currentProcess->msg->body);
		free(currentProcess->msg);
		free(currentProcess);
		// currentProcess->msg->body = 0;
		// currentProcess->msg = 0;
		// currentProcess = 0;
		numOfProcess--;
		// get another process from readyQ running.
		if(numOfProcess!=0){
		runNextProcess();
		}
        }
        else{
		printf("init core process killed. Terminating the program.\n");
        }
	}
	else
	{
		printf("Exit the current process FAILED\n");
	}
	return resultReport;
}

void decreasePriority(PCB *p){
	if (p->priority < 2)
	{
		p->priority++;
	}
}
// UNDERSTOOD
// IMPLEMENTED
void performQuantum()
{
	PCB *currentProcess;

	// take item out of runningQ
	currentProcess = List_last(runningQ);
	// if the removed process is just a normal pcb (not init process)
	if (currentProcess->pid != 0)
	{
		currentProcess = List_trim(runningQ);

		decreasePriority(currentProcess);

		// push the p into the ready queue to the front of the ready Q.
		currentProcess->state = READY;
		// place on readyQ
		List_prepend(readyQ[currentProcess->priority], currentProcess);

		printf("pid: %i placed on ready queue.\n", currentProcess->pid);
	}
	// why do we need this? what happen if we 'q' when no process
	// we don't need this block.
	//  else
	//  {
	//  	// reset init proc state
	//  	p->state = READY;
	//  }

	// runningProcess cpu scheduler
	runNextProcess();
}

// UNDERSTOOD, ready to implement
// IMPLEMENTED, but I need to fix the seg fault. 
void sendMessage(int pid, char *msg)
{
	PCB *runningProcess, *p;
	PROC_MSG *pm;
	int srcProcessId;
	// get the currently running process
	runningProcess = List_last(runningQ);
	// get the pid of the process in order to include it in the message
	srcProcessId = runningProcess->pid;

	// if the receiver of the message is already waiting in the receive Queue, the sender don't get blocked
	//this if block when receiver is already waiting, we get seg fault. so let's fix that issue. let's go
	if (p = findQueuedProcess(pid, receiveQ))
	{
		// update the messge of the process
		p->msg->src = srcProcessId;
		p->msg->dest = pid;
		p->msg->type = SEND;
		// what does this strcpy do? ok just putting it
		strcpy(p->msg->body, msg);
		// take out of receiveQ
		p = List_remove(receiveQ);
		// add to readyQ
		p->state = READY;
		List_prepend(readyQ[p->priority], p);
		// printf("SEND SUCCESS");
	} // else if we can find the pcb in somewhere else(which is not blocked)
	else if (findProcess(pid))
	{ // allocate memory for the message
		pm = malloc(sizeof pm);
		// set the message to the message
		pm->body = (char *)malloc(sizeof(char) * 40);
		pm->src = srcProcessId;
		pm->dest = pid;
		pm->type = SEND;
		strcpy(pm->body, msg);
		// put msg on messageQ
		List_insert_after(messageQ, pm);
		runningProcess = List_last(runningQ);
		if (runningProcess->pid != 0)
		{
			// blk sender
			// by getting the process running,
			runningProcess = List_trim(runningQ);
			// and block
			runningProcess->state = BLOCKED;
			// and put that into sendQ
			List_insert_after(sendQ, runningProcess);
			// and let the last item in the ready queue running
			runNextProcess();
		}
		printf("SEND SUCCESS");
	} // if we cann't find the specified pcb, set it as fail
	else
	{
		printf("SEND FAILED");
	}
}

// RECEIVE
// UNDERSTOOD
// IMPLEMENTED
void performReceive()
{
	PROC_MSG *pm;
	PCB *currentProcess;
	int destPid;
	// get the currently running item
	currentProcess = List_last(runningQ);
	destPid = currentProcess->pid;
	// chk messageQ
	// show msg, src
	// if there's already a message waiting to be received for this guy,
	if (pm = findQueuedProcess(destPid, messageQ))
	{ // display the message, and remove the messge from queue
		printMessage(pm);
		List_remove(messageQ);
	}
	// if no message available for this guy, then block
	else
	{
		// put process on receiveQ
		// cpu sched
		if (currentProcess->pid != 0)
		{ // take it out of running Q and then put that in receive queue
			currentProcess = List_trim(runningQ);
			currentProcess->state = BLOCKED;
			List_prepend(receiveQ, currentProcess);
			// and let the next one runningProcess
			runNextProcess();
		}
	}
}
// REPLY
// UNDERSTOOD how it's working.
void replyMessage(int pid, char *msg)
{
	PCB *runningProcess, *p;
	// get the running process cuz he's wanting to reply, so we want his pid
	runningProcess = List_last(runningQ);
	// if the current process wants to reply to currently blocked sender,
	if (p = findQueuedProcess(pid, sendQ))
	{
		// copy msg to sender's pcb
		p->msg->dest = pid;
		p->msg->src = runningProcess->pid;
		p->msg->type = REPLY;
		strcpy(p->msg->body, msg);

		// unblock the blocked sender in sendQ
		p = List_remove(sendQ);
		p->state = READY;
		// and insert the unblocked sender to the appropriete ready queue.
		List_prepend(readyQ[p->priority], p);

		printf("\nREPLY SUCCESS\n");
	}
	// I think i need to modify here, cuz
	// a process can reply to a process that are not waiting at all.
	// FIX HERE.
	else
	{
		p = findProcess(pid);
		if(p!=NULL){
		p->msg->dest = pid;
		p->msg->src = runningProcess->pid;
		p->msg->type = REPLY;
		strcpy(p->msg->body, msg);
		}
		else{
		// fail
		printf("\nREPLY FAIL\n");
		}
	}
}

//ORIGINAL
bool isValidSemNumber(int sid){
	if (sid >= 0 && sid <= 4){
		return true;
	}
	else{
		return false;
	}
}

//'n' to create a new semaphore.
// UNDERSTOOD, ready to implement
//IMEPELEMTNED
void createSemaphore(int sid)
{
	// init sem
	SEM *s;
	// if sid 0-5 AND does not already exist;
	if (isValidSemNumber(sid))
	{
		if (!findSemaphore(sid, semaphoreQ))
		{
			s = malloc(sizeof(SEM));
			s->sid = sid;
			s->value = 1;
			s->slist = List_create();
			List_insert_after(semaphoreQ, s);
			printf("SEM initialization SUCCESS");

		}
		else{
			printf("SEM initialization FAIL");
		}
	}
	else{
			printf("SEM initialization FAIL");
	}
}

// UNDERSTOOD
//IMPLEMENTED, ready to fix seg fault. 
void P(int sid)
{
	SEM *s;
	s = malloc(sizeof(SEM));
	PCB *p;
	int block = 0;

	p = List_last(runningQ);
	printf("\n hey1 p%d", p->pid);
	s = findSemaphore(sid, semaphoreQ);
	if(s==NULL){
		printf("SSSSSSS IS NULLLLL");
	}
	
	// printf("\n hey2 after s is assignied. check if s is good p%d", s->sid);
	//S IS NOT NULL
	if (s !=NULL && p->pid != 0)
	{
		printf("\n s is not null and p pid is not null");
		s->value--;            
		if (s->value <= 0)
		{
			printf("\n hey4");
			p = List_trim(runningQ);
			p->state = BLOCKED;
			List_prepend(s->slist, p);
			block = 1;
			printf("P SUCCESS");
			runNextProcess();	
			printf("\n hey5");
		}
	}
	else{
		printf("\n hey6");
		printf("P FAIILED");
	}
}

// UNDERSTOOD
//IMPLEMENTED, ready to fix seg fault. 
void V(int sid)
{
    SEM *s;
    PCB *p;
    int fail = 1;
    int ready_q = 0;

	s = findQueuedProcess(sid, semaphoreQ);
	if(s){
		s->value++;
		if(s->value <=1){
			if(p = List_trim(s->slist)){
				List_prepend(readyQ[p->priority], p);
				p->state = READY;
				ready_q = 1;
			}
		}
		fail = 0;
	}
	if (fail)
	{
		printf("\nFAIL\n");
	}
	else
	{
		printf("\nSUCCESS\n");
		if (ready_q)
		{
			printf("pid: %i into READY QUEUE\n", p->pid);
		}
	}
}

// UNDERSTOOD
//IMPLEMENTED
void procinfo(int pid)
{
	PCB *process;
	char *state;

	process = findProcess(pid);//i think it's becaues of the way findpid return stuff. cuz p is there. 
	// printf("\n in proc info after findProcess, this is what i got %d", p->pid);
	// P IS NOT NULL;m but accessing anythingi regarding p is null. 
	if (process != NULL)
	{
		state = getStateName(process->state);
		printf("\nPROCESS INFORMATION\n");
		printf("\n______________________\n");
		printf("Process Id: %i\n", process->pid);
		printf("______________________\n");
		printf("Priority: %i\n", process->priority);
		printf("______________________\n");
		printf("State: %s\n", state);
		printf("______________________\n");
		printf("Message: %s\n", process->msg->body);
		printf("\n_________________\n");
	}
	else
	{
		printf("PROCESS INFO ACQUISITION FAILED");
	}
}

// UNDERSTOOD
//IMPLEMENTED
void totalinfo()
{
	PCB *p;
	// char *state;
	int i, count;

	// runningQ
	p = List_last(runningQ);
	// state = getStateName(p->state);
	printf("\n_______________\n");
	printf("\nRUNNING QUEUE\n");
	printf("pid: %i\n", p->pid);

	// readyQ
	printf("\nREADY QUEUES\n");
	for (i = 0; i < 3; i++)
	{
		printf("Priority [%i]:\n", i);
		printQueue(readyQ[i]);
	}

	// MESSAGE Q
	printf("\nSEND QUEUE\n");
	printQueue(sendQ);
	printf("\nRECEIVE QUEUE\n");
	printQueue(receiveQ);
	printf("\nMSG QUEUE\n");
	printQueue(messageQ);

	printf("\nSEM QUEUES\n");
	if(isListNotEmpty(semaphoreQ))
	{
		count = List_count(semaphoreQ);
		SEM *s;
		List_first(semaphoreQ);
		List_prev(semaphoreQ);
		for (i = 0; i < count; i++)
		{
			s = List_next(semaphoreQ);
			printf("sid%i: ", s->sid);
			printQueue(s->slist);
		}
	}

	printf("\n_______________\n");
}

//ORIGINAL
void setInitProcess(){
    initProcess = malloc(sizeof(PCB));
    initProcess->pid = 0;
    initProcess->priority = 0;
    initProcess->state = RUNNING;
    List_insert_after(runningQ, initProcess);
}

//ORIGINAL
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

//ORIGINAL
void performCreate(){
    printf("Specify Process Priority (0, 1, 2): ");
    scanf("%d", &priority);
    resultReport = create(priority);
}

//ORIGINAL
void performKill(){
    printf("Enter the pid to killProcess: ");
    scanf("%d", &processId);
    if(processId== 0 && numOfProcess !=0){
        printf("\n You can't killProcess the init core process.\n");
    }else{
		//instead of passing RET_QUEUE, just get the PCB returned, and 
		//write a function to get which queue the PCB is in. 
		PCB *p;
    printf("before findProcess in performKILL\n\n ");
        p = (PCB *) findProcess(processId);
    printf("after findProcess in performKILL\n\n ");
		list = findListFromProcess(processId);
		//pass processId, and get the name of the list.
        resultReport = killProcess(processId, list);
        if(list == runningQ){
            runNextProcess();
        }
    }
}

//ORIGINAL
void performSend(){
    printf("Enter the pid to sendMessage message to: ");
    scanf("%d", &processId);
    printf("Enter message: ");
    scanf(" %[^\n]", messageBuf);
    sendMessage(processId, messageBuf);
}

//ORIGINAL
void performReply(){
    printf("Enter the pid to reply to: ");
    scanf("%d", &processId);
    printf("Enter reply message: ");
    scanf(" %[^\n]", messageBuf);
    replyMessage(processId, messageBuf);
}

//ORIGINAL
void performNewSemaphore(){
    printf("Enter SEM id to create: ");
    scanf("%d", &semId);
    createSemaphore(semId);
}

//ORIGINAL
void performP(){
    printf("Enter SEM id to apply P: ");
    scanf("%d", &semId);
    P(semId);
}

//ORIGINAL
void performV(){
    printf("Enter SEM id to apply V: ");
    scanf("%d", &semId);
    V(semId);
}

//ORIGINAL
void performProcInfo(){
    printf("Enter process id to see details: ");
    scanf("%d", &processId);
    procinfo(processId);
}

//ORIGINAL
void promptUser(){

    // bool isInitProcessAlive = (List_count(runningQ)>0);
    printf("isInitProcessAlive is %d", List_count(runningQ)>0);
    while(List_count(runningQ)>0){

        printf("Enter command :");
        scanf("%c", &userInputCmd);
        userInputCmd = toupper(userInputCmd);

        if(userInputCmd =='C'){
            performCreate(); //DONE
            // break;
        }
        else if(userInputCmd =='F'){
            resultReport = forkProcess();//DONE
            // continue;
        }
        else if(userInputCmd =='K'){
            performKill();//DONEDONEDONE
            // break;
        }
        else if(userInputCmd =='E'){
            performExit();//DONE
            // break;
        }
        else if(userInputCmd =='Q'){
            performQuantum();//DONE
            // break;
        }
        else if(userInputCmd =='S'){
            performSend();//DONE
            // break;
        }
        else if(userInputCmd =='R'){
            performReceive();//DONE
            // break;
        }
        else if(userInputCmd =='Y'){
            performReply();//NEED TO FIX stuff
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