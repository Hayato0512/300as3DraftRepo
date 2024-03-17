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

// place holder for every queue , to be initiated in main()
List *readyQ[3];
List *sendQ, *recvQ, *msgQ, *semQ, *runQ;

// don't need
int debug_msg = 1;
// don't need
int enable_printf = 1;
// NEED THEM
int numproc = 0;
int proc_counter = 0;

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
	List_first(recvQ);
	List_first(runQ);
	List_first(semQ);
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
			// add to runQ
			if (p)
			{
				status = List_append(runQ, p);
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
	if (p = List_last(runQ))
	{
		if (p->pid == 0)
		{
			p->state = RUNNING;
		}
		printf("pid: %i now running. \n", p->pid);

		// we probably want to do this for every process that comes to running process(show the message if the currently running process has message)
		if (strlen(p->msg->body) != 0)
		{
			display_pm(p->msg);
		}

		reset_pm(p->msg);
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
	else if (n = List_search(recvQ, comparator, (void *)&pid))
	{
		list = recvQ;
	}
	else if (n = List_search(runQ, comparator, (void *)&pid))
	{
		list = runQ;
	}
	// if we need to search through semQ,
	else if (List_count(semQ))
	{
		SEM *s;
		int i, count;
		// get the number of semaphores
		count = List_count(semQ);
		// don't we wanna do this to List_first?
		List_prev(semQ);
		for (i = 0; i < count; i++)
		{
			// start from first node of semQ and s->list
			s = List_next(semQ);
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
	if (list == msgQ)
	{
		str = "dest";
	}
	else if (list == semQ)
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
		if (list == semQ)
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

// maybe we don't need this pm in the parameter
// cuz when we falk, I don't want to keep the message??
// UNDERSTOOD and ready to implement
char *create(int priority, PROC_MSG *pm)
{
	char *report;
	char *buf;
	int size;
	// PROC_MSG *pm;

	if (priority < 0 || priority > 2)
	{
		priority = 2;
	}

	size = List_count(readyQ[priority]);

	// what is the difference between numproc and proc_counter??
	// numproc is just the number of pcb at the moment
	// proc_counter is used to assign auto-incremented id to a new pcb.
	// we wanna use both as killing a process will numProc -- but we want to keep increment for the new pcb id
	numproc++;
	proc_counter++;
	// allocate memory for the new PCB,
	PCB *p = malloc(sizeof(PCB));
	p->pid = proc_counter;
	p->priority = priority;
	p->state = READY;
	// allocate memory for the messge section in the PCB
	p->msg = (PROC_MSG *)malloc(sizeof(PROC_MSG));
	p->msg->body = (char *)malloc(sizeof(char) * 40);

	// I don't think we need this cuz we don't wanna inherit the message when folking
	if (pm != NULL)
	{
		p->msg->dest = pm->dest;
		p->msg->src = pm->src;
		p->msg->type = pm->type;
		strcpy(p->msg->body, pm->body);
	}
	else
	{
		reset_pm(p->msg);
	}
	// add the newly created process to ready Q, when create a pcb, it will automatically go to readyQ
	List_prepend(readyQ[priority], p);
	// check if the list got a new item, and if so, success, if not fail.
	if (List_count(readyQ[priority]) == size + 1)
	{
		buf = "SUCCESS: pid";

		// sprintf (report, "%s%i", buf, p->pid);

		printf("SUCCESS: pid%i\n", p->pid);
	}
	else
	{
		report = "FAIL";
		printf("FAIL\n");
	}
	// at the end, return the result.
	return report;
}

// UNDERSTOOD.
char *fork_cmd()
{ // create report, and pointer to the pcb
	char *report;
	PCB *p;
	// get the pointer to the curent item in the running Q
	p = List_last(runQ);
	// if the current running process is not null,
	if (p)
	{ // create a new pcb and put that in the ready Q
		report = create(p->priority, p->msg);
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
		free(p->msg->body);
		free(p->msg);
		free(p);
		p->msg->body = 0;
		p->msg = 0;
		p = 0;
		numproc--;
	}
	else
	{
		display("FAIL");
	}

	return report;
}

// with "e", kill the current running process.
// UNDERSTOOD. ready to implement
char *exit_curr()
{
	char *report;
	PCB *p;
	// get the currentrunning process
	p = List_last(runQ);
	// this condition check might be wrong. why numProc == 0?
	//  if numproc ==0, which means we only have init process(kernal)
	// so this 'e' will work to terminate the whole program if no process. try it
	// so i guess when init process is created, we don't do numproc++??
	if (numproc == 0 || p->pid != 0)
	{
		// remove the currently running pcb
		p = List_trim(runQ);
		printf("pid:%i removed\n", p->pid);
		// free memory
		free(p->msg->body);
		free(p->msg);
		free(p);
		p->msg->body = 0;
		p->msg = 0;
		p = 0;
		numproc--;
		// get another process from readyQ running.
		CPU_scheduler();
	}
	else
	{
		display("Cannot exit init process.  Other processes waiting.");
	}
	return report;
}
// UNDERSTOOD
void quantum()
{
	PCB *p;

	// take item out of runQ
	p = List_last(runQ);
	// if the removed process is just a normal pcb (not init process)
	if (p->pid != 0)
	{
		p = List_trim(runQ);
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
	run = List_last(runQ);
	// get the pid of the process in order to include it in the message
	src = run->pid;

	// if the receiver of the message is already waiting in the receive Queue,
	if (p = findpid_fromQ(pid, recvQ))
	{
		// update the messge of the process
		p->msg->src = src;
		p->msg->dest = pid;
		p->msg->type = SEND;
		// what does this strcpy do? ok just putting it
		strcpy(p->msg->body, msg);
		// take out of recvQ
		p = List_remove(recvQ);
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
		// put msg on msgQ
		List_insert_after(msgQ, pm);
		run = List_last(runQ);
		if (run->pid != 0)
		{
			// blk sender
			// by getting the process running,
			p = List_trim(runQ);
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
void rx()
{
	PROC_MSG *pm;
	PCB *p;
	int dest;
	// get the currently running item
	p = List_last(runQ);
	dest = p->pid;
	// chk msgQ
	// show msg, src
	// if there's already a message waiting to be received for this guy,
	if (pm = findpid_fromQ(dest, msgQ))
	{ // display the message, and remove the messge from queue
		display_pm(pm);
		List_remove(msgQ);
	}
	// if no message available for this guy, then block
	else
	{
		// put process on recvQ
		// cpu sched
		if (p->pid != 0)
		{ // take it out of running Q and then put that in receive queue
			p = List_trim(runQ);
			p->state = BLOCKED;
			List_prepend(recvQ, p);
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
	run = List_last(runQ);
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
		if (!findpid_fromQ(sid, semQ))
		{
			s = malloc(sizeof(SEM));
			s->sid = sid;
			s->value = 0;
			s->slist = List_create();
			List_insert_after(semQ, s);

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

	p = List_last(runQ);
	s = findpid_fromQ(sid, semQ);
	if (s && p->pid != 0)
	{
		s->value--;
		if (s->value < 0)
		{
			p = List_trim(runQ);
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

	s = findpid_fromQ(sid, semQ);
	if (s)
	{
		s->value++;
		if (s->value <= 0)
		{
			if (p = List_trim(s->slist))
			{
				List_prepend(readyQ[p->priority], p);
				p->state = READY;
				ready_q = 1;
			}
		}
		fail = 0;
	}

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

	// runQ
	p = List_last(runQ);
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
	display_queue(recvQ);
	display("MSG QUEUE");
	display_queue(msgQ);

	display("SEM QUEUES");
	if (count = List_count(semQ))
	{
		SEM *s;

		List_first(semQ);
		List_prev(semQ);
		for (i = 0; i < count; i++)
		{
			s = List_next(semQ);
			printf("sid%i: ", s->sid);
			display_queue(s->slist);
		}
	}

	// display("P() QUEUE");
	// display("V() QUEUE");
	display("=========");
}

// UNDERSTOOD
int main(int argc, const char *argv[])
{
	char *str;
	char buf[40];
	int pid, priority, sid, status;

	// void (*itemFree)(void*);
	// int (*comparator)(void*, void*);

	char cmd;
	// char param;
	char *report;
	PCB *init;
	List *list;

	// create lists

	runQ = List_create();
	sendQ = List_create();
	recvQ = List_create();
	readyQ[0] = List_create();
	readyQ[1] = List_create();
	readyQ[2] = List_create();
	msgQ = List_create();
	semQ = List_create();

	// init init process
	init = malloc(sizeof(PCB));
	init->pid = 0;
	init->priority = 0;
	init->state = RUNNING;

	init->msg = (PROC_MSG *)malloc(sizeof(PROC_MSG));
	init->msg->body = (char *)malloc(sizeof(char) * 40);

	reset_pm(init->msg);

	// strlen(init->msg->body);

	List_insert_after(runQ, init);

	// str = hi();
	while (List_count(runQ))
	{

		printf("Enter Command: ");
		scanf("%c", &cmd);
		cmd = toupper(cmd);

		switch (cmd)
		{
		// create
		case 'C':
			printf("Enter Priority (0, 1, 2): ");
			scanf("%d", &priority);
			report = create(priority, NULL);
			break;
		// fork
		case 'F':
			report = fork_cmd();
			break;
		case 'K':
			status = 0;
			printf("Enter pid: ");
			scanf("%d", &pid);
			if (pid == 0 && numproc != 0)
			{
				display("Cannot kill init process.");
				break;
			}
			// this kill will kill the current process. why do we need to know the list?
			// ok, we wanna know it because if the user kill the process in the running Q, we
			// want to start the next process.
			// but if the pcb is in the other queue, we don't do CPU scheduling.
			// so that is why.
			list = findpid(pid, RET_QUEUE);
			report = kill(pid, list);
			if (list && list == runQ)
			{
				CPU_scheduler();
			}

			break;
		case 'E':
			exit_curr();
			break;
		case 'Q':
			quantum();
			break;
		case 'S':
			printf("Enter pid: ");
			scanf("%i", &pid);
			printf("Enter message: ");
			// scanf(" %s", buf);
			scanf(" %[^\n]", buf);
			send(pid, buf);
			break;
		case 'R':
			rx();
			break;
		case 'Y':
			printf("Enter pid: ");
			scanf("%i", &pid);
			printf("Enter reply: ");
			scanf(" %[^\n]", buf);
			reply_cmd(pid, buf);
			break;
		case 'N':
			printf("Enter SEM id: ");
			scanf("%i", &sid);
			new_sem(sid);
			break;
		case 'P':
			printf("Enter SEM id: ");
			scanf("%i", &sid);
			P(sid);
			break;
		case 'V':
			printf("Enter SEM id: ");
			scanf("%i", &sid);
			V(sid);
			break;
		case 'I':
			printf("Enter pid: ");
			scanf("%i", &pid);
			procinfo(pid);
			break;
		case 'T':
			totalinfo();
			break;

		default:
			break;
		}
		scanf("%c", &cmd);
		memset(&cmd, (int)NULL, sizeof cmd);

		// printf("%s\n", report);
	}

	return 0;
}
