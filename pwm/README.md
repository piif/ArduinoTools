# pwm.h

Some functions to simplify usage of atmel timers on various micro controlers

# timers summary

There are between 2 and 6 timers depending on ATmega or ATtiny models.

Each of them have specific registers to control various aspects.

All of them share some functionnalities :
 * a counter on 8 or 16 bits
 * several modes to control the way counting from 0 to N
 * output PINs can be controled by current value of counter, compare to other registers
 * prescalers to divide CPU frequency by 2^n
 * interruptions can be called on timer events (counter overflow)

The most complex part are :
 * understand various control modes (Wave Generation Modes)
 * handle differences between each timer of each model ...

Here is some information to try to clarify this :

First of all : Arduino time functions use timer0, thus if you tweak them, `delay` or `micros` functions work no more

| | ATtiny | ATmega328p (Arduino Uno & Nano) | ATmega644A (RepRap) | ATmega32u (Arduino Micro & Fio) | ATmega2560 (Arduino Mega) |
|---|---|---|---|---|
| counter size|
| timer 0 | 8 bits |  8 bits |  8 bits |  8 bits |  8 bits |
| timer 1 | 8 bits | 16 bits | 16 bits | 16 bits | 16 bits |
| timer 2 |   -    |  8 bits |  8 bits |    -    |  8 bits |
| timer 3 |   -    |    -    |(16 bits)| 16 bits | 16 bits |
| timer 4 |   -    |    -    |    -    | 10 bits | 16 bits |
| timer 5 |   -    |    -    |    -    |    -    | 16 bits |
| output comparators|
| timer 0 | A,B | A,B | A,B | A,B   | A,B   |
| timer 1 | A,B | A,B | A,B | A,B,C | A,B,C |
| timer 2 |  -  | A,B | A,B |   -   | A,B   |
| timer 3 |  -  |  -  |(A,B)| A,B,C | A,B,C |
| timer 4 |  -  |  -  |  -  | A,B,D | A,B,C |
| timer 5 |  -  |  -  |  -  |   -   | A,B,C |
| outputs |
| timer 0 | PB0, 1             | PD6, 5 = 6, 5   | PB3, 4 = 3, 4   | PB7,PD0 = 11, 3     | PB7, PG5 = 13, 4 |
| timer 1 | PB1/~PB0, PB4/~PB3 | PB1,2 = 9,10    | PD5, 4 = 13, 12 | PB5,6,7 = 9, 10, 11 | PB5,6,7 = 11,12,13 |
| timer 2 |        -           | PB3,PD3 = 11,3  | PD7, 6 = 15, 14 |         -           | PB4,PH6 = 10,9  |
| timer 3 |        -           |        -        | (PB6, 7 = 6, 7) | PC6 = 5             | PE3,4,5 = 5,2,3 |
| timer 4 |        -           |        -        |        -        | PC7/~PC6 , PB6/~PB5 , PD7/~PD6 = -/5,-/6,-/9 | PH3,4,5 = 6,7,8 |
| timer 5 |        -           |        -        |        -        |         -           | PL3,4,5 = 46,45,44 |
| prescalers |
| timer 0 | 1,8,64,256,1024 | 1,8,64,256,1024        | 1,8,64,256,1024        | 1,8,64,256,1024 | 1,8,64,256,1024 |
| timer 1 | 1 to 16384      | 1,8,64,256,1024        | 1,8,64,256,1024        | 1,8,64,256,1024 | 1,8,64,256,1024 |
| timer 2 |      -          | 1,8,32,64,128,256,1024 | 1,8,64,256,1024        |     -    | 1,8,32,64,128,256,1024 |
| timer 3 |      -          |  -                     |(1,8,32,64,128,256,1024)| 1,8,64,256,1024 | 1,8,64,256,1024 |
| timer 4 |      -          |  -                     |  -                     | 1 to 16384      | 1,8,64,256,1024 |
| timer 5 |      -          |  -                     |  -                     |       -         | 1,8,64,256,1024 |


On Tiny :
 * Timer1 have 2 outputs and their 'not' values

On 32U4 :
 * There's no timer2
 * Timer3 has 3 output comparators, but only 1 related output (on OC3A = PC6 = pin 5)
 * Timer4 has 3* an output and their 'not' values but Arduino Micro have only 'not' ones wired to pins.  
   Third one is called 'D' and not 'C' (?!)

On 644A there are 3 timers but variant 1284A have a forth one (timer3)