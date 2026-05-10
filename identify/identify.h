// Try to identify device kind and model
// By the way, CPU frequency should be defined in F_CPU macro

#define __STR(x) #x
#define _STR(x) __STR(x)

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny25__)
	#define ARDUINO_TINY
	#define DEVICE_KIND "tiny"
	#define DEVICE_KIND_TINY 1
	#define MODEL_NAME  _STR(__AVR_DEVICE_NAME__)
#elif defined(__AVR_ATtiny13A__)
	#define ARDUINO_TINY13
	#define DEVICE_KIND "tiny"
	#define DEVICE_KIND_TINY 1
	#define MODEL_NAME  "attiny13"
#elif defined(__LGT8F__)  // or ARDUINO_AVR_LARDU_328E
	#define LGT8F_UNO
	#define DEVICE_KIND "arduino"
	#define DEVICE_KIND_ARDUINO 1
	#define MODEL_NAME  "LGT8F328"
#elif defined(__AVR_MEGA__)
	#define ARDUINO_UNO
	#define DEVICE_KIND "arduino"
	#define DEVICE_KIND_ARDUINO 1
	#define MODEL_NAME  _STR(__AVR_DEVICE_NAME__)
#elif defined(__AVR_DEVICE_NAME__)
	#define DEVICE_KIND "unknown"
	#define MODEL_NAME  _STR(__AVR_DEVICE_NAME__)
#else
	#define DEVICE_KIND "unknown"
	#define MODEL_NAME  "unknown"
#endif
