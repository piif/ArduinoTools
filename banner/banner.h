#define POLICE "police_5x8.h"

#ifdef __cplusplus
//extern "C" {
#endif

/**
 * initialize iterator with given message
 * ba careful : message is modified !
 * return number of iterations needed
 */
int bannerSetMessage(char *message);

/**
 * return next byte to send to display message
 * loops at end of message
 */
int bannerNext();

/**
 * force to go to given step in banner scenario
 */
void bannerSeek(int charIndex, int byteIndex);

#ifdef __cplusplus
//}
#endif
