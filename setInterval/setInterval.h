/**
 * lib minimale pour appeler une fonction à intervalles réguliers, à la façon du setInterval en js
 */

#ifndef SET_INTERVAL_H
#define SET_INTERVAL_H

#ifdef __cplusplus
//extern "C" {
#endif

/**
 * constante pour indiquer une erreur
 */
#define SET_INTERVAL_ERROR ((setIntervalTimer)-1)

/**
 * le nombre max de timer qu'on peut lancer en même temps
 */
#define MAX_INTERVAL 10

/**
 * le type d'objet retourné par setInterval
 */
typedef int setIntervalTimer;

/**
 * prototype des fonctions à appeler
 * void *userData : pointer passed to setInterval call
 * long delta : called function with ms late
 * int missed : number of call missed
 */
typedef void (setIntervalFunction)(void *, long, int);

/**
 * défini un timer qui appellera la fonction "callback" toutes les "ms" millisecondes
 */
setIntervalTimer setInterval(long ms, setIntervalFunction *callback, void *userData);

/**
 * change le délai d'appel du timer passé en argument
 */
setIntervalTimer changeInterval(setIntervalTimer timer, long ms);

/**
 * fonction a appeler en boucle pour effectuer les appels
 */
void setIntervalStep();

#ifdef __cplusplus
//}
#endif

#endif
