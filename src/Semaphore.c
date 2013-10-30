#include "../include/yakk.h"
#include "../include/clib.h"
#include "../include/PriorityQueue.h"

extern PriorityQueue readyQueue;

YKSEM* YKSemCreate(int initialValue) {
	YKSEM* newSemaphore;

	newSemaphore = getNewSem();
	if (newSemaphore == null) exit(NEW_SEM_FAILED);

	initializePriorityQueue(&(newSemaphore->queue));
	newSemaphore->value = initialValue;

	return newSemaphore;

}

void YKSemPend(YKSEM* semaphore) {

	TCB* runningTask;

	YKEnterMutex();
	if (semaphore->value < 1) {
		semaphore->value--;
		runningTask = removePriorityQueue(&readyQueue);
		runningTask->state = T_BLOCKED;
		insertPriorityQueue((&(semaphore->queue)), runningTask);
		asm("int 0x20");
		return;

	} else {
		semaphore->value--;
		YKExitMutex();
		return;
	}

}

void YKSemPost(YKSEM* semaphore) {
	
	TCB* readyTask;

	YKEnterMutex();
	semaphore->value++;
	readyTask = removePriorityQueue(&(semaphore->queue));
	if (readyTask == null) return;
	readyTask->state = T_READY;
	insertPriorityQueue(&readyQueue, readyTask);
	YKExitMutex();
	asm("int 0x20");
	return;

}
