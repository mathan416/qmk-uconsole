#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define TIMER_DIFF_16(a,b) ((uint16_t)((a)-(b)))
extern uint16_t mock_now;
static inline uint16_t timer_read(void) { return mock_now; }
typedef struct { int8_t x,y,h,v; uint8_t buttons; } report_mouse_t;
typedef struct { int dummy; } keyrecord_t;
static inline bool process_record_user(uint16_t x, keyrecord_t *r) { (void)x;(void)r;return true; }
#define PAL_LINE(p,n) (n)
#define GPIOC 0
#define PAL_MODE_INPUT_PULLUP 0
#define PAL_EVENT_MODE_BOTH_EDGES 0
static inline void palSetLineMode(int a,int b) {(void)a;(void)b;}
static inline void palEnableLineEvent(int a,int b) {(void)a;(void)b;}
static inline void palSetLineCallback(int a,void (*b)(void*),void*c) {(void)a;(void)b;(void)c;}
static inline void chSysLock(void) {}
static inline void chSysUnlock(void) {}
static inline void suspend_power_down_user(void) {}
static inline void suspend_wakeup_init_user(void) {}
