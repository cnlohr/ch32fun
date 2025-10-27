/*
Short example to demonstrate how to use external interrupts.
When I tested my circuit I made the following connections:
PA10 ---- LED
PA1  ---- Debounced switch that pulls to ground and a pull up resistor.
You can set any pins for LED and switch by editing their defines.
*/
#include "ch32fun.h"

#define LED_PIN PA10
#define INT_PIN PA1

#define RISING_EDGE 1
#define FALLING_EDGE 0

static inline void blinkInterrupt( void );
// Setup all interrupt handlers the same way so we can later enable any of them in code
__attribute__((interrupt)) void EXTI0_IRQHandler( void ) { blinkInterrupt(); }
__attribute__((interrupt)) void EXTI1_IRQHandler( void ) { blinkInterrupt(); }
__attribute__((interrupt)) void EXTI2_IRQHandler( void ) { blinkInterrupt(); }
__attribute__((interrupt)) void EXTI3_IRQHandler( void ) { blinkInterrupt(); }
__attribute__((interrupt)) void EXTI4_IRQHandler( void ) { blinkInterrupt(); }
__attribute__((interrupt)) void EXTI9_5_IRQHandler( void ) { blinkInterrupt(); }
__attribute__((interrupt)) void EXTI15_10_IRQHandler( void ) { blinkInterrupt(); }

// set state of the LED_PIN
int state = 1;

static void setupEXTI( uint8_t pin, uint8_t rising )
{
	uint8_t port = (uint8_t)(pin / 16); // GPIO pin numbers in ch32fun are defined as an integer. Each port adds 16 to the pin number.
	uint8_t exti = (uint8_t)(pin / 4); // There are 4 AFIO->EXTICR registers. And each is responsible for 4 pins out of 16

	AFIO->EXTICR[exti] |= port; // AFIO->EXTICR cares only about enabling certain port for it's corresponding 4 pins

	if (rising) {
		EXTI->RTENR |= 1 << (pin%16); // enable rising edge trigger
		EXTI->FTENR &= ~(1 << (pin%16)); // disable falling edge trigger
	} else {
		EXTI->RTENR &= ~(1 << (pin%16)); // disable rising edge trigger
		EXTI->FTENR |= 1 << (pin%16); // enable falling edge trigger
	}

	// There are 5 separate interrupts for pins 0-4 and two combined interrupts for pins 5-9 and 10-15
	if( (pin%16) < 5 ) NVIC_EnableIRQ( (IRQn_Type)((uint8_t)EXTI0_IRQn + (pin%16)) );
	else if( (pin%16) < 10 ) NVIC_EnableIRQ( EXTI9_5_IRQn );
	else NVIC_EnableIRQ( EXTI15_10_IRQn );

	// And here we are finally enable the pin number. (We enable port previously in AFIO->EXTICR)
	EXTI->INTENR |= 1 << (pin%16); // Enable EXTIx line
}

int main()
{
	SystemInit();

	/*
	The enables that end up being used in this code are
	GPIOx  (GPIO port enable for our pins)
	AFIO   (alternate function enable)
	AFIOEN (alternate function clock enable)
	*/
	funGpioInitAll();

	// configure LED_PIN for output and set it high
	funPinMode( LED_PIN, GPIO_CFGLR_OUT_10Mhz_PP );
	funDigitalWrite( LED_PIN, FUN_HIGH );

	// configure interrupt pin for floating input
	funPinMode( INT_PIN, GPIO_CFGLR_IN_FLOAT );

	// You can also use pull-up/pull-down
	// funPinMode( INT_PIN, GPIO_CFGLR_IN_PUPD );
	// funDigitalWrite( INT_PIN, FUN_LOW );

	// Set all registers needed for external interrupt on INT_PIN
	setupEXTI( INT_PIN, RISING_EDGE );

	while(1)
	{

	}
}

/*
Interrupt code
	Checks if it was set low or high last
	and toggles it based on that info.
*/
static inline void blinkInterrupt( void )
{
	// verify the interrupt flag occured.
	if( EXTI->INTFR & (1<<(INT_PIN%16)) )
	{
		if( state == 0 )
		{
			funDigitalWrite( LED_PIN, FUN_HIGH );
			state = 1;
		} 
		else
		{
			funDigitalWrite( LED_PIN, FUN_LOW );
			state = 0;
		}
		EXTI->INTFR = 1<<(INT_PIN%16); // clear interrupt flag
	}
}