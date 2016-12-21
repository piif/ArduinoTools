/**
 * lib minimale pour appeler une fonction à intervalles réguliers,
 *  à la façon du setInterval en js
 */

#include "setInterval.h"

#include <Arduino.h>
#ifdef WITHOUT_MILLIS_FUNCTIONS
#include "myMillis/myMillis.h"
#define millis myMillis
#endif

#ifndef NULL
#define NULL (0)
#endif

typedef struct _setIntervalStruct {
	long ms; // 0 si cette entrée est libre, sinon nombre de ms à attendre entre 2 appels
	setIntervalFunction *callback;
	void *userData;
	long next;
} setIntervalStruct;
setIntervalStruct setIntervalTable[MAX_INTERVAL];

/**
 * défini un timer qui appellera la fonction "callback" toutes les "ms" millisecondes
 */
setIntervalTimer setInterval(long ms, setIntervalFunction *callback, void *userData) {
	int i;
	// PAUSED allowed, but not FREE while creating
	if (ms == 0 || ms < -1) {
		return SET_INTERVAL_ERROR;
	}

	for (i = 0; i < MAX_INTERVAL; i++) {
		if (setIntervalTable[i].ms != SET_INTERVAL_FREE) {
			continue;
		}
		// on a trouvé un emplacement libre
		setIntervalTable[i].ms = ms;
		setIntervalTable[i].callback = callback;
		setIntervalTable[i].userData = userData;
		if (ms != SET_INTERVAL_PAUSED) {
			setIntervalTable[i].next = millis() + ms;
		}

		return (setIntervalTimer) i;
	}
	return SET_INTERVAL_ERROR;
}

/**
 * change le délai d'appel du timer passé en argument
 */
setIntervalTimer changeInterval(setIntervalTimer timer, long ms) {
	if (timer < 0  || timer >= MAX_INTERVAL || ms < -1) {
		return SET_INTERVAL_ERROR;
	}
	if (ms > 0) {
		if (setIntervalTable[timer].ms == SET_INTERVAL_PAUSED) {
			// reactivate a stopped timer
			setIntervalTable[timer].next = millis() + ms;
		} else {
			setIntervalTable[timer].next += ms - setIntervalTable[timer].ms;
		}
	}
	setIntervalTable[timer].ms = ms;
	return timer;
}
setIntervalTimer changeInterval(setIntervalTimer timer, long ms, setIntervalFunction *callback, void *userData) {
	setIntervalTimer r = changeInterval(timer, ms);
	if (r != SET_INTERVAL_ERROR) {
		setIntervalTable[timer].callback = callback;
		setIntervalTable[timer].userData = userData;
	}
	return r;
}

void setIntervalStep() {
	int i, missed;
	long delta;

	for (i = 0; i < MAX_INTERVAL; i++) {
		setIntervalStruct *t = &(setIntervalTable[i]);
		if (t->ms <= 0) {
			continue;
		}
		if ((unsigned long) t->next >= millis()) {
			continue;
		}
		delta = millis() - t->next;
		missed = 0;
		t->next += t->ms;
		while (delta > t->ms) {
			missed++;
			t->next += t->ms;
			delta -= t->ms;
		}

		(*(t->callback))(t->userData, delta, missed);
	}
}

