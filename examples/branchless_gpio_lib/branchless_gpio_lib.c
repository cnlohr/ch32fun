// 2023-06-26 recallmenot

#define DEMO_GPIO_blink					1
#define DEMO_GPIO_blink_port				0
#define DEMO_GPIO_out					0
#define DEMO_GPIO_in_btn				0
#define DEMO_ADC_bargraph				0
#define DEMO_PWM_dayrider				0

#if ((DEMO_GPIO_blink + DEMO_GPIO_blink_port + DEMO_GPIO_out + DEMO_GPIO_in_btn + DEMO_ADC_bargraph + DEMO_PWM_dayrider) > 1 \
  || (DEMO_GPIO_blink + DEMO_GPIO_blink_port + DEMO_GPIO_out + DEMO_GPIO_in_btn + DEMO_ADC_bargraph + DEMO_PWM_dayrider) < 1)
#error "please enable ONE of the demos by setting it to 1 and the others to 0"
#endif

#include "ch32fun.h"

#include "ch32v003_GPIO_branchless.h"

#include <stdio.h>



int main() {
	SystemInit();

#if DEMO_GPIO_blink == 1
	GPIO_port_enable(GPIO_port_C);
	GPIO_port_enable(GPIO_port_D);
	// GPIO D0 Push-Pull
	GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 0), GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
	// GPIO D4 Push-Pull
	// P function suffix allows to specify port and pin in one parameter
	GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 4), GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
	// GPIO C0 Push-Pull
	GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_C, 0), GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
#elif DEMO_GPIO_blink_port == 1
	GPIO_port_enable(GPIO_port_C);
	GPIO_port_pinMode(GPIO_port_C, GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
#elif DEMO_GPIO_out == 1
	GPIO_port_enable(GPIO_port_C);
	GPIO_port_enable(GPIO_port_D);
	// GPIO D4 Push-Pull
	GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 4), GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
	// GPIO C0 - C7 Push-Pull
	GPIO_port_pinMode(GPIO_port_C, GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
	/* faster & lighter than
	for (int i = 0; i <= 7; i++) {
		GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_C, i), GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
	}
	*/
#elif DEMO_GPIO_in_btn == 1
	GPIO_port_enable(GPIO_port_C);
	GPIO_port_enable(GPIO_port_D);
	// GPIO D3 Push-Pull
	GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 3), GPIO_pinMode_I_pullUp, GPIO_Speed_In);
	// GPIO C0 - C7 Push-Pull
	GPIO_port_pinMode(GPIO_port_C, GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
#elif DEMO_ADC_bargraph == 1
	GPIO_port_enable(GPIO_port_C);
	GPIO_port_enable(GPIO_port_D);
	// GPIO D4 Push-Pull
	GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 4), GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
	// GPIO D6 analog in
	GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 6), GPIO_pinMode_I_analog, GPIO_Speed_In);
	// GPIO C0 - C7 Push-Pull
	GPIO_port_pinMode(GPIO_port_C, GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
	GPIO_ADCinit();
#elif DEMO_PWM_dayrider == 1
	//SetupUART( UART_BRR );
	GPIO_port_enable(GPIO_port_C);
	GPIO_port_enable(GPIO_port_D);
	// GPIO D4 Push-Pull
	GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 4), GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
	// GPIO D6 analog in
	GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 6), GPIO_pinMode_I_analog, GPIO_Speed_In);
	// GPIO C0 - C7 Push-Pull
	GPIO_port_pinMode(GPIO_port_C, GPIO_pinMode_O_pushPullMux, GPIO_Speed_10MHz);
	GPIO_tim2_map(GPIO_tim2_output_set_1__C5_C2_D2_C1);
	GPIO_tim2_init();
	GPIO_tim2_enableCH(4);
	GPIO_tim2_enableCH(2);
	GPIO_tim2_enableCH(1);
	GPIO_tim1_map(GPIO_tim1_output_set_0__D2_A1_C3_C4__D0_A2_D1);
	GPIO_tim1_init();
	GPIO_tim1_enableCH(3);
	GPIO_tim1_enableCH(4);
#endif
	


	while (1) {
#if DEMO_GPIO_blink == 1
		GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 0), high);
		// P function suffix allows to specify port and pin in one parameter
		GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 4), high);
		GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_C, 0), high);
		Delay_Ms( 250 );
		GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 0), low);
		// P function suffix allows to specify port and pin in one parameter
		GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 4), low);
		GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_C, 0), low);
		Delay_Ms( 250 );
#elif DEMO_GPIO_blink_port == 1
		GPIO_port_digitalWrite(GPIO_port_C, 0b11111111);
		Delay_Ms( 250 );
		GPIO_port_digitalWrite(GPIO_port_C, 0b10101010);
		Delay_Ms( 250 );
		GPIO_port_digitalWrite(GPIO_port_C, 0b00000000);
		Delay_Ms( 250 );
		GPIO_port_digitalWrite(GPIO_port_C, 0b01010101);
		Delay_Ms( 250 );
#elif DEMO_GPIO_out == 1
		GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 4), low);
		Delay_Ms(1000);
		GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 4), high);
		Delay_Ms(250);
		for (int i = 0; i <= 7; i++) {
			GPIO_digitalWrite_hi(GPIOv_from_PORT_PIN(GPIO_port_C, i));
			Delay_Ms(50);
		}
		for (int i = 7; i >= 0; i--) {
			GPIO_digitalWrite_lo(GPIOv_from_PORT_PIN(GPIO_port_C, i));
			Delay_Ms(50);
		}
#elif DEMO_GPIO_in_btn == 1
		uint8_t button_is_pressed = !GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_D, 3));
		static uint8_t leds_to_turn_on;
		if (button_is_pressed && leds_to_turn_on < 8) {
			leds_to_turn_on++;
		}
		else if (!button_is_pressed && leds_to_turn_on > 0) {
			leds_to_turn_on--;
		}
		uint8_t led_i = 0;
		for (int i = 0; i<= 7; i++) {
			if (led_i < leds_to_turn_on) {
				GPIO_digitalWrite_hi(GPIOv_from_PORT_PIN(GPIO_port_C, i));
			}
			else {
				GPIO_digitalWrite_lo(GPIOv_from_PORT_PIN(GPIO_port_C, i));
			}
			led_i++;
		}
		Delay_Ms(50);
#elif DEMO_ADC_bargraph == 1
		GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 4), high);
		uint16_t analog_result = GPIO_analogRead(GPIO_Ain6_D6);
		analog_result = analog_result > 10 ?
				(analog_result < ((1 << 10) - 42) ? analog_result + 42 : (1 << 10))
			: 0;
		uint8_t leds_to_turn_on = analog_result >> 7;
		uint8_t led_i = 0;
		for (int i = 0; i<= 7; i++) {
			if (led_i < leds_to_turn_on) {
				GPIO_digitalWrite_hi(GPIOv_from_PORT_PIN(GPIO_port_C, i));
			}
			else {
				GPIO_digitalWrite_lo(GPIOv_from_PORT_PIN(GPIO_port_C, i));
			}
			led_i++;
		}
		GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 4), low);
		Delay_Ms(50);
#elif DEMO_PWM_dayrider == 1
		#define NUMLEDS		5
		#define TICKS_NEXT	149			// lower = faster scanning
		#define TICK_i		2143			// lower = faster fade-out
		GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 4), high);
		static uint8_t led_focus = 0;
		static uint8_t direction = 0;

		static uint16_t led_PW[NUMLEDS];

		static uint32_t tick;

		for (uint8_t i = 0; i < NUMLEDS; i++) {
			if ((i == led_focus) && ((tick % TICKS_NEXT) == 0)) {
				led_PW[i] = 1023;
				//printf("focus %u, tick %u, direction %u\r\n", led_focus, tick, direction);
				if (direction == 0) {
					led_focus++;
					if (led_focus >= (NUMLEDS - 1)) {
						direction = 1;
					}
				}
				else {
					led_focus--;
					if (led_focus == 0) {
						direction = 0;
					}
				}
				tick++;
			}
			else {
				led_PW[i] = (led_PW[i] > 2) ? (led_PW[i] - 3) : 0;
			}
		}
		GPIO_tim2_analogWrite(4, led_PW[0]);
		GPIO_tim2_analogWrite(2, led_PW[1]);
		GPIO_tim1_analogWrite(3, led_PW[2]);
		GPIO_tim1_analogWrite(4, led_PW[3]);
		GPIO_tim2_analogWrite(1, led_PW[4]);
		GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 4), low);
		tick++;
		Delay_Us(TICK_i);
#endif
	}
}
