#include "clib.h"

static int tickCounter = 0;
static int i = 0;
static char tick[5] = "TICK ";
static char keypress[10] = "KEYPRESS (";
static char ignored[9] = ") IGNORED";
static char delay[17] = "DELAY KEY PRESSED";
static char complete[14] = "DELAY COMPLETE";
extern int KeyBuffer;

void resetHandler() {

	exit(0);

}

void tickHandler() {

	tickCounter++;
	printNewLine();
	print(tick, 5);
	printInt(tickCounter);
	printNewLine();
	return;

}

void keyboardHandler() {

	if (KeyBuffer == 'd') {
		printNewLine();
		print(delay, 17);
		printNewLine();

		for (i = 0; i < 5000; i++);
	
		printNewLine();
		print(complete, 14);		
		printNewLine();

	} else {
		printNewLine();
		print(keypress, 10);
		printChar((char) KeyBuffer);
		print(ignored, 9);
		printNewLine();
	}

	return;
}