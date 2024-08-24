/**
 * minimal lib to react to input data from serial
 */
#ifndef SERIAL_INPUT_H
#define SERIAL_INPUT_H 1

#include <Arduino.h>
#include <Stream.h>

#define SERIAL_INPUT_MAX_LEN 50

typedef int (*destFuncByte)(const byte value, Stream &channel);
typedef int (*destFuncInt)(const int value, Stream &channel);
typedef int (*destFuncString)(const char *s, Stream &channel);

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
int handleInput(Stream &channel);

/**
 * same function with argument to specify if input must be echo'ed back
 */
int handleInput(Stream &channel, bool echo);

/**
 * function to call to handle serial input
 */
int handleInput();

/**
 * same function with argument to specify if input must be echo'ed back
 */
int handleInput(bool echo);

#endif
