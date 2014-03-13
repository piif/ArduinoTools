#define NOT_IN_MAIN 1
#include "ArduinoTools.h"

#ifndef _VECTOR_SIZE
#define NB_INTERRUPT _VECTORS_SIZE / 4
#else
#define NB_INTERRUPT _VECTORS_SIZE / _VECTOR_SIZE
#endif

extern InterruptHandler interruptHandler[];

InterruptHandler setInterruptHandler(short interruptNumber, InterruptHandler handler) {
	InterruptHandler before = interruptHandler[interruptNumber];
	interruptHandler[interruptNumber] = handler;
	return before;
}
