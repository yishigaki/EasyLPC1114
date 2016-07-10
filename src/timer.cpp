#include "timer.h"

volatile uint32_t tickCount;

// pointers to interrupt handlers
void (*fptr_isr_timer32_0)(void);
void (*fptr_isr_timer32_1)(void);
void (*fptr_isr_timer16_0)(void);
void (*fptr_isr_timer16_1)(void);

void TIMER32_0_IRQHandler(void)
{
  fptr_isr_timer32_0();
}

void TIMER32_1_IRQHandler(void)
{
  fptr_isr_timer32_1();
}

void TIMER16_0_IRQHandler(void)
{
  fptr_isr_timer16_0();
}

void TIMER16_1_IRQHandler(void)
{
  fptr_isr_timer16_1();
}

void SysTick_Handler(void) {
  tickCount++;
}

Timer32::Timer32(uint32_t set_timer_num)
{
	timer_num = set_timer_num;
	if(timer_num == TIMER0){
		address = LPC_TMR32B0;
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9);  //provide system clock

		NVIC_EnableIRQ(TIMER_32_0_IRQn);  //Enable the TIMER0 Interrupt

	}else if(timer_num == TIMER1){
		address = LPC_TMR32B1;
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10); //provide system clock

		NVIC_EnableIRQ(TIMER_32_1_IRQn);  //Enable the TIMER1 Interrupt

	}else{					//exception
		address = LPC_TMR32B0;
	}

	prescaler = 1;		//default: x1
	address->PR = 0x00;  //prescaler
}

void Timer32::resetTimer()
{
	uint32_t regVal;
	regVal = address->TCR;
	regVal |= 0x02;
	address->TCR = regVal;
}

void Timer32::setTimer_ms(void (*fp)(void), uint32_t ms)
{
	if(timer_num == 0) fptr_isr_timer32_0 = fp;
	else fptr_isr_timer32_1 = fp;

	uint32_t count;
	count = ms * ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV) / 1000 / prescaler);

	address->TCR = 0x02;		//reset timer
	address->MR0 = count;
	address->MCR = 3;	// Interrupt and Reset on MR0
}

void Timer32::setTimer_us(void (*fp)(void), uint32_t us)
{
	if(timer_num == 0) fptr_isr_timer32_0 = fp;
	else fptr_isr_timer32_1 = fp;

	uint32_t count;
	count = us * ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV) / 1000000 / prescaler);

	address->TCR = 0x02;		//reset timer
	address->MR0 = count;
	address->MCR = 3;	// Interrupt and Reset on MR0
}

void Timer32::setTimer_counter(void (*fp)(void), uint32_t setCount)
{
	if(timer_num == 0) fptr_isr_timer32_0 = fp;
	else fptr_isr_timer32_1 = fp;

	address->TCR = 0x02;		//reset timer
	address->MR0 = setCount;
	address->MCR = 3;	// Interrupt and Reset on MR0
}

void Timer32::wait_ms(uint32_t ms)
{
	uint32_t count;
	count = ms * ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV) / 1000 / prescaler);

	address->TCR = 0x02;		//reset timer
	address->MR0 = count;
	address->IR  = 0xff;		//reset all interrupts
	address->MCR = 0x04;		//stop timer on match
	address->TCR = 0x01;		//start timer

	while (address->TCR & 0x01);  //wait until delay time has elapsed
}

void Timer32::wait_us(uint32_t us)
{
	uint32_t count;
	count = us * ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV) / 1000000 / prescaler);

	address->TCR = 0x02;		//reset timer
	address->MR0 = count;
	address->IR  = 0xff;		//reset all interrupts
	address->MCR = 0x04;		//stop timer on match
	address->TCR = 0x01;		//start timer

	while (address->TCR & 0x01);  //wait until delay time has elapsed
}



Timer16::Timer16(uint32_t set_timer_num)
{
	timer_num = set_timer_num;
	if(timer_num == TIMER0){
		address = LPC_TMR16B0;
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);  //provide system clock

		NVIC_EnableIRQ(TIMER_16_0_IRQn);  //Enable the TIMER0 Interrupt

	}else if(timer_num == TIMER1){
		address = LPC_TMR16B1;
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8); //provide system clock

		NVIC_EnableIRQ(TIMER_16_1_IRQn);  //Enable the TIMER1 Interrupt

	}else{					//exception
		address = LPC_TMR16B0;
	}

	prescaler = 1;		//default: x1
	address->PR  = 0x00;  //prescaler

}

void Timer16::resetTimer()
{
	uint32_t regVal;
	regVal = address->TCR;
	regVal |= 0x02;
	address->TCR = regVal;
}

void Timer16::setTimer_us(void (*fp)(void), uint32_t us)
{
	if(timer_num == 0) fptr_isr_timer16_0 = fp;
	else fptr_isr_timer16_1 = fp;

	uint32_t count;
	count = us * ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV) / 1000000 / prescaler);

	address->TCR = 0x02;		//reset timer
	address->MR0 = count;
	address->MCR = 3;	// Interrupt and Reset on MR0
}

void Timer16::setTimer_counter(void (*fp)(void), uint32_t setCount)
{
	if(timer_num == 0) fptr_isr_timer16_0 = fp;
	else fptr_isr_timer16_1 = fp;

	address->TCR = 0x02;		//reset timer
	address->MR0 = setCount;
	address->MCR = 3;	// Interrupt and Reset on MR0
}

void Timer16::wait_us(uint32_t us)
{
	uint32_t count;
	count = us * ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV) / 1000000 / prescaler);

	address->TCR = 0x02;		//reset timer
	address->MR0 = count;
	address->IR  = 0xff;		//reset all interrupts
	address->MCR = 0x04;		//stop timer on match
	address->TCR = 0x01;		//start timer

	while (address->TCR & 0x01);  //wait until delay time has elapsed
}

// wait using SystickTimer
/*void wait_ms(uint32_t waitTicks){
	tickCount = 0;
	SysTick_Config(SystemCoreClock / 1000);
	while (tickCount < waitTicks);
	SysTick->CTRL  &= 0xFFFFFFFD;  //disable interrupt
}

void wait_us(uint32_t waitTicks){
	tickCount = 0;
	SysTick_Config(SystemCoreClock / 1000000);
	while (tickCount < waitTicks);
	SysTick->CTRL  &= 0xFFFFFFFD;  //disable interrupt
}
*/
void wait_cycle(uint32_t cycle)
{
	for(uint32_t i=0; i<cycle; i++){
		__asm volatile("nop");
	}
}

// -- wait using nop (not accurate) --
void wait_ms(uint32_t ms)
{
#if defined(DEBUG)
	uint32_t adj = 25;
#else
	uint32_t adj = 15;
#endif
	uint32_t count = ms * ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV) / 1000 / adj);
	volatile uint32_t i;
	for(i=0; i<count; i++){
		//__asm volatile("nop");
	}
}

void wait_us(uint32_t us)
{
#if defined(DEBUG)
	uint32_t adj = 25;
#else
	uint32_t adj = 15;
#endif
	uint32_t count = us * ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV) / 1000000 / adj);
	volatile uint32_t i;
	for(i=0; i<count; i++){
		//__asm volatile("nop");
	}
}

