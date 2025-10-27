/* Simple demo to compare custom mcpy instruction vs standard memcpy.
Currently only known processors to support this instruction are CH570/2 and CH585. 
More info: https://www.cnblogs.com/JayWellsBlog/p/18640330
https://gist.github.com/ArcaneNibble/77f7e819b5b892f51a777902adb0e7c4
https://github.com/openwch/ch585/blob/main/EVT/EXAM/SRC/StdPeriphDriver/CH58x_sys.c#L605
https://github.com/openwch/ch570/blob/main/EVT/EXAM/SRC/RVMSIS/core_riscv.h#L643 */

#include "ch32fun.h"
#include <stdio.h>

#define BUFFER_SIZE 256

__attribute__((aligned(4))) uint8_t buffer1[BUFFER_SIZE];
__attribute__((aligned(4))) uint8_t buffer2[BUFFER_SIZE];

static inline void mcpy_raw(void *dst, void *start, void *end) {
	__asm__ volatile (".insn r 0x0f, 0x7, 0, x0, %3, %0, %1"
	      : "+r"(start), "+r"(dst)
	      :  "r"(0), "r"(end)
	      : "memory");
}

void * fast_memcpy(void *dst, void *src, uint32_t size)
{
	uint32_t * end = src + size;
	mcpy_raw(dst, src, (void *)end);

	return dst;
}

int main()
{
	SystemInit();

	printf("start\n\n");
	uint32_t systick_timer, systick_timer2;
	memset(buffer1, 0xAA, BUFFER_SIZE);
	memset(buffer2, 0, BUFFER_SIZE);
	Delay_Ms(200);
	printf("buffer1:\n");
	for (uint8_t i = 0; i < 255; i ++) {
		printf("%02x ", buffer1[i]);
	}
	printf("\n\n");
	printf("buffer2:\n");
	for (uint8_t i = 0; i < 255; i ++) {
		printf("%02x ", buffer2[i]);
	}
	printf("\n");
	Delay_Ms(200);
	systick_timer = SysTick->CNT;
	memcpy(buffer2, buffer1, BUFFER_SIZE);
	systick_timer2 = SysTick->CNT;
	printf("\n-----------------------------------------------\n");
	printf("Copied %ld bytes using memcpy in %ld cycles\n", BUFFER_SIZE, systick_timer2 - systick_timer);
	printf("-----------------------------------------------\n");
	printf("buffer1:\n");
	for (uint8_t i = 0; i < 255; i ++) {
		printf("%02x ", buffer1[i]);
	}
	printf("\n\n");
	printf("buffer2:\n");
	for (uint8_t i = 0; i < 255; i ++) {
		printf("%02x ", buffer2[i]);
	}
	printf("\n");
	Delay_Ms(200);
	memset(buffer1, 0xAB, BUFFER_SIZE);
	memset(buffer2, 0, BUFFER_SIZE);
	Delay_Ms(200);
	systick_timer = SysTick->CNT;
	fast_memcpy(buffer2, buffer1, 256);
	systick_timer2 = SysTick->CNT;
	printf("\n-----------------------------------------------\n");
	printf("Copied %ld bytes using mcpy in %ld cycles\n", BUFFER_SIZE, systick_timer2 - systick_timer);
	printf("-----------------------------------------------\n");
	
	Delay_Ms(200);
	printf("buffer1:\n");
	for (uint8_t i = 0; i < 255; i ++) {
		printf("%02x ", buffer1[i]);
	}
	printf("\n\n");
	printf("buffer2:\n");
	for (uint8_t i = 0; i < 255; i ++) {
		printf("%02x ", buffer2[i]);
	}
	printf("\n");

	while(1)
	{
		Delay_Ms(100);
	}
}
