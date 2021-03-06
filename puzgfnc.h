#ifndef PUZG_H
#define PUZG_H
#include <avr/wdt.h>
//#define OSNOVNAYA 2
//#define GENERATOR 3

#define OSNOVNAYA 3
#define GENERATOR 2

#define OSNOVNAYA_SWCH A5
#define GENERATOR_ON A4
#define GENERATOR_OFF A3
#define GENERATOR_SWCH A2

/*#define OSNOVNAYA_SWCH 4
#define GENERATOR_ON 5
#define GENERATOR_OFF 6
#define GENERATOR_SWCH 7
*/


#define SET_OSNOVNAYA               0
#define SET_GENERATORA              1
#define ZAPUSK_GENERATORA           2
#define OSHIBKA_ZAPUSKA             3
#define OSTANOV_GENERATORA          4
#define PROPALA_OSNOVNAYA_SET       5
#define FORCE_GENERATOR             6
#define FORCE_OSNOVNAYA             7
#define FORCE_NOPOWER               8
#define GLOBAL_ERROR                9
#define FORCE_GENERATOR_START       10
#define FORCE_OSNOVNAYA_START       11
#define FORCE_NOPOWER_START         12
#define UNDEF_STATE                 255


//#define START_NUM                    10
#define FORCE_OSNOVNAYA_SHTD_CNT     3
#define FORCE_GENERATOR_START_CNT    5
//#define OSTANOV_GENERATORA1_CNT      30
//#define OSTANOV_GENERATORA2_CNT      60
//#define DIZEL_STARTER_TIME           3000 //ms
//#define OSNOVNAYA_GENERATOR_SWH_DELAY  10000 //ms
extern uint16_t OSTANOV_GENERATORA1_CNT;
extern uint16_t OSTANOV_GENERATORA2_CNT;
extern unsigned int DIZEL_STARTER_TIME; //ms
extern uint8_t START_NUM;
extern unsigned int OSNOVNAYA_GENERATOR_SWH_DELAY;
extern uint8_t starts_try_num;

extern uint8_t curr_state;
void ProcessFunc();
void pins_init();

#endif 
