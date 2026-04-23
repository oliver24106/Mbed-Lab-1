#include "mbed.h"
using namespace std::chrono_literals;

/*
Embedded Systems - Lab 1 (Blinky Extension Tasks)
Board: NUCLEO (LED1/LED2/LED3)

How to use:
- Change RUN_TASKS below to 1,2,3, or 4
- Build + flash

Tasks (from lab sheet):
1) Blink all 3 LEDS
2) LED1 @1s, LED2 @0.5s (2x faster), LED3 @2s (0.5x slower)
3) Ping-pong: LED1 -> LED2-> LED3 -> LED2 -> LED1...
4) Blink all together 5 times, then LED1 ON forever (LED2/LED3 OFF)
*/

// --------- Hardware ----------
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

// Pick which task runs (1-4)
#define RUN_TASK 4

// Set all LEDs off
static void all_off() { led1=0; led2=0; led3=0; }

// ---------- Task 1 -----------
//Edit blinky so LED2 + LED3 also blink (same rate)

static void task1_blink_all() {
    all_off();
    while(true) {
        led1 = !led1;
        led2 = !led2;
        led3 = !led3;
        ThisThread::sleep_for(500ms);
    }
}

// ---------- Task 2 -----------
//LED1 toggles every 1s, LED2 every 0.5s, LED3 every 2s
//Timing logic: one Timer, three independent "next toggle" timestamps

static void task2_three_rates() {
    
    all_off();

    Timer t;
    t.start();

    auto next1 = 1s;
    auto next2 = 500ms;
    auto next3 = 2s;

    while (true) {
        auto now = t.elapsed_time();

        if (now >=next1) { led1 = !led1; next1 += 1s;}
        if (now >=next2) { led2 = !led2; next2 += 500ms;}
        if (now >=next3) { led3 = !led3; next3 += 1s;}

        ThisThread::sleep_for(10ms); // prevents busy waiting
    }
}


// ---------- Task 3 -----------
// Ping-pong sequence:
// LED1 -> LED2 -> LED3 -> LED2 -> LED1 -> repeat
// Each step: ON 200ms, OFF 200ms
// Sequence logic: index + direction, reverse direction at ends
static void task3_ping_pong() {
    all_off();

    DigitalOut* leds[] = { &led1, &led2, &led3 };
    int idx = 0;
    int dir = +1;

    while (true) {
        *leds[idx] = 1;
        ThisThread::sleep_for(200ms);

        *leds[idx] = 0;
        ThisThread::sleep_for(200ms);

        idx += dir;
        if (idx == 2) dir = -1; // bounce back from LED3
        if (idx == 0) dir = +1; // bounce forward from LED1
    }
}

// ---------- Task 4 -----------
// Blink all three LEDs together 5 times:
// 200ms on, 200ms off.
// After 5 blinks: LED1 stays ON forever, LED2+LED3 stay OFF.
// Extra logic: two-phase behaviour (blink phase + final hold phase)
static void task4_blink_then_lock() {
    all_off();

    for (int i = 0; i < 5; i++) {
        led1 = 1; led2 = 1; led3 = 1;
        ThisThread::sleep_for(200ms);

        led1 = 0; led2 = 0; led3 = 0;
        ThisThread::sleep_for(200ms);
    }

    // Final state
    led1 = 1; led2 = 0; led3 = 0;

    while (true) {
        ThisThread::sleep_for(1s); // hold forever
    }
}

// ---------- Main -----------
int main() {
#if RUN_TASK == 1
    task1_blink_all();
#elif RUN_TASK == 2
    task2_three_rates();
#elif RUN_TASK == 3
    task3_ping_pong();
#elif RUN_TASK == 4
    task4_blink_then_lock();
#else
    // Invalid selection: turn all off
    all_off();
    while (true) { ThisThread::sleep_for(1s); }
#endif
}