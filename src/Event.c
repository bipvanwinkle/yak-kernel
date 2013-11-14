#include "../include/yakk.h"

extern PriorityQueue readyQueue;

YKEVENT* YKEventCreate(unsigned initialValue) {

	YKEVENT* newEvent;

	newEvent = getNewEvent();
	if (newEvent == null) exit(NEW_EVENT_FAILED);
	
	newEvent->mask = initialValue;
	initializePriorityQueue(&(newEvent->queue));
	
	return newEvent;

}

unsigned YKEventPend(YKEVENT* event, unsigned eventMask, int waitMode) {

	TCB* runningTask;

	if (waitMode == EVENT_WAIT_ANY) {
		YKEnterMutex();		
		if (event->mask & eventMask) { 
			return event->mask;
		} else {
			runningTask = removePriorityQueue(&readyQueue);
			runningTask->state = T_BLOCKED;
			runningTask->tid = eventMask;
			insertPriorityQueue((&(event->queue)), runningTask);
			YKExitMutex();
			asm("int 0x20");
			return event->mask;
		}
	} else if (waitMod == EVENT_WAIT_ALL) {
		YKEnterMutex();		
		if (event->mask & eventMask == eventPask) { 
			return event->mask;
		} else {
			runningTask = removePriorityQueue(&readyQueue);
			runningTask->state = T_BLOCKED;
			runningTask->tid = eventMask;
			insertPriorityQueue((&(event->queue)), runningTask);
			YKExitMutex();
			asm("int 0x20");
			return event->mask;
		}
	} else {
		exit(EVENT_PEND_ERROR);
	}

}

void YKEventSet(YKEVENT* event, unsigned eventMask) {

	TCB* temp;
	TCB*	current;

	YKEnterMutex();
	event->mask = event->mask | eventMask;
	
	current = event->queue.head;
	while (current != null) {
		if (current->tid == event->mask) {
			temp = current;
			current = current->next;
			if (temp->prev != null) {
				temp->prev->next = temp->next;
			} else {
				event->queue.head = temp->next;
			}
			if (temp->next != null) {
				temp->next->prev = temp->prev;
			} else {
				event->queue.tail = temp->prev;
			}
			temp->prev = null;
			temp->next = null;
			event->queue.size--;
			temp->state = T_READY;
			insertPriorityQueue(&(readyQueue), temp);
		} else {
			current = current->next;
		}
	}
	YKExitMutex();
	asm("int 0x20");
	return;

}

void YKEVentReset(YKEVENT* event, unsigned eventMask) {

	event->mask = event->mask & (~eventMask);
	return;

}
