#pragma once

#include "LPC11xx.h"

#define TIMER0 0
#define TIMER1 1

#ifdef __cplusplus
extern "C" {
#endif
// Interrupt handler
void TIMER32_0_IRQHandler(void);
void TIMER32_1_IRQHandler(void);
void TIMER16_0_IRQHandler(void);
void TIMER16_1_IRQHandler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

//32bit timer
class Timer32 {
private:
	uint32_t timer_num;
	LPC_TMR_TypeDef* address;
	uint32_t prescaler;

public:
	Timer32(uint32_t set_timer_num);
	void enableTimer(){address->TCR = 1;}
	void disableTimer(){address->TCR = 0;}
	void resetTimer();						//reset timer counter
	void clearInterrupt(){address->IR = 1;} //clear interrupt flag
	void setPrescaler(uint32_t scale){prescaler = scale; address->PR  = prescaler - 1;}
	void setTimer_ms(void (*fp)(void), uint32_t ms);
	void setTimer_us(void (*fp)(void), uint32_t us);
	void setTimer_counter(void (*fp)(void), uint32_t setCount);
	void wait_ms(uint32_t ms);
	void wait_us(uint32_t us);
};

//16bit timer
class Timer16 {
private:
	uint32_t timer_num;
	LPC_TMR_TypeDef* address;
	uint16_t prescaler;

public:
	Timer16(uint32_t set_timer_num);
	void enableTimer(){address->TCR = 1;}
	void disableTimer(){address->TCR = 0;}
	void resetTimer();
	void setPrescaler(uint16_t scale){prescaler = scale; address->PR  = prescaler - 1;}
	void setTimer_us(void (*fp)(void), uint32_t us);
	void setTimer_counter(void (*fp)(void), uint32_t setCount);
	void wait_us(uint32_t us);
};

// wait using SystickTimer
//void wait_ms(uint32_t waitTicks);
//void wait_us(uint32_t waitTicks);

// wait using nop (not accurate)
void wait_cycle(uint32_t cycle);
void wait_ms(uint32_t ms);
void wait_us(uint32_t us);
