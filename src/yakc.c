#include "clib.h"
#include "yakk.h"
#include "yaku.h"

//User Accessible Variables
unsigned int YKCtxSwCount = 0;
unsigned int YKIdleCount = 0;
unsigned int YKTickNum = 0;

//Kernel Accessible Variables
static unsigned int ISRCallDepth = 0;
static TCB* currentTask = null;
static ReadyQueue readyQueue;
static DelayQueue delayQueue;
static TaskBlock taskBlock;
static int idleTaskStack[IDLETASKSTACKSIZE];

//Error Codes
#define NewTaskFailed 1

char* TCBFullError = "Task Block Full\n\r";

void YKEnterMutex(void) {

	//disable interrupts
	asm("cli");

}

void YKExitMutex(void) {

	//Conditionally enable interrupts
	asm("sti");

}

void YKEnterISR(void) {

	ISRCallDepth++;

}

void YKExitISR(void) {

	ISRCallDepth--;
	if (ISRCallDepth == 0) YKScheduler();

}

void YKInitialize(void) {

	YKEnterMutex();

	//Set up Task Block
	taskBlock.nextFreeTCB == 0;
	//
		
	//Set up Idle Task
	YKNewTask(YKIdleTask, &idleTaskStack[IDLETASKSTACKSIZE], 100);
	//

	YKExitMutex();

}

void YKIdleTask(void) {

	while (1) {
		int i;
		i = 1;
		YKIdleCount++;
	}

}

void YKNewTask(void (*task)(void), void* taskStack, unsigned char priority) {

	TCB* newTask;	
	
	//Obtain a TCB
	newTask = getNewTCB();
	if (newTask == null) exit(NewTaskFailed);

	//Fill TCB
	newTask->tid = 0;
	newTask->priority = priority;
	newTask->stackPointer = taskStack;
	newTask->state = READY;
	newTask->delayCount = 0;
	newTask->next = null;
	newTask->prev = null;

	//Set up Stack
	asm("mov bx, [bp+6]"); //Get address of stack
	asm("mov cx, [bp+4]"); //Get address of function pointer
	asm("mov [bx-20], 0x0200"); //Move flag register onto the stack
	asm("mov [bx-22], 0x0");	
	asm("mov [bx-24], cx");

	//Insert into ready queue
	insertSorted(&readyQueue, newTask); 

}

TCB* getNewTCB(void) {

	TCB* task;
	if (taskBlock.nextFreeTCB < MAX_TASKS + 1) {
                task = &taskBlock.tasks[taskBlock.nextFreeTCB];
		taskBlock.nextFreeTCB++;
		return task;
	} else {
		printf("TCBFullError\n");		
		return null;
	}

}

void scheduler(void) {

	YKEnterMutex();
	TCB* readyTask = dequeue(readyQueue);
	if (readyTask == null) exit (2);
	if (readyTask != currentTask) {
		YKExitMutex();
		dispatcher(readyTask);
		return;
	}
	YKExitMutex();
	return;
}

void YKRun(void) {

	YKMutex();
	scheduler();

}

void YKDelayTask(unsigned int count) {

	if (count == 0) return;

	currentTask->state = BLOCKED;
	currentTask->delayCount = count;
	insert(delayQueue, currentTask);
	asm("int 0x20");
	return;

}

