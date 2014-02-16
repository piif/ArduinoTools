/**
 * minimal lib to react to input data from serial
 */
#ifndef SERIAL_INPUT_H
#define SERIAL_INPUT_H 1

#include <Arduino.h>

#define SERIAL_INPUT_MAX_LEN 50

typedef void (*destFuncByte)(const byte value);
typedef void (*destFuncInt)(const int value);
typedef void (*destFuncString)(const char *s);

typedef struct inputItem {
	// char to react to
	char prefix;
	// b = read byte , i = read int , s = read string
	// B = call func with byte arg , I or f = call func with int arg , S = call func with string arg
	char type;
	// function to call or pointer into which to write input
	void *destination;
} InputItem;


/**
 * function to call to register a list of commands
 */
int registerInput(int nbItems, InputItem *items);

/**
 * function to call to handle serial input
 */
void handleInput();

#endif
