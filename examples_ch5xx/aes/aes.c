#include "ch32fun.h"
#include <stdio.h>

#define AES_BASE ((uint32_t)0x4000c300)

typedef struct {
	volatile uint32_t CFG;
	volatile uint32_t STA; // Completely (?) useless
	volatile uint32_t some_reg1; // Don't know yet what are these 4 for
	volatile uint32_t some_reg2;
	volatile uint32_t some_reg3;
	volatile uint32_t some_reg4;
	volatile uint32_t data[4];
	volatile uint32_t key[4];
} AES_Type;

#define AES ((AES_Type *) AES_BASE)

void doAES(uint32_t * key, uint32_t * in, uint32_t * out, uint8_t enc) {
	
	AES->CFG = 0x100; // Don't know what it does, maybe enables the peripheral or clock for it
	AES->CFG = enc?0:2; // Type of operation
	AES->key[0] = key[0];
	AES->key[1] = key[1];
	AES->key[2] = key[2];
	AES->key[3] = key[3];
	AES->data[0] = in[0];
	AES->data[1] = in[1];
	AES->data[2] = in[2];
	AES->data[3] = in[3];
	// AES->STA &= 0xfffffffd;
	// AES->STA |= 1;
	AES->CFG |= 1; // Start process
  
  while(AES->CFG & 1); // Wait for it to finish
  
	out[0] = AES->data[0];
	out[1] = AES->data[1];
	out[2] = AES->data[2];
	out[3] = AES->data[3];
}

int main()
{
	SystemInit();

	printf("ch32fun HW AES example\n");
	printf("----------------------\n\n");

	char key[16] = "ch32 is very fun";
	char plain_text[16] = "ch5xx can do aes";
	uint8_t secret_message[16] = {0xD7,0xF6, 0x79, 0x38, 0x60, 0x2A, 0xCC, 0x2F, 0x50, 0xF7, 0x2A, 0x8B, 0x2B, 0x04, 0x31, 0x52};
	uint8_t output_data[16];

	printf("128 bit key:\n ");
	for (int i = 0; i < 16; i++) {
		printf("%c", key[i]);
	}
	printf("\n\n");
	printf("key in HEX:\n ");
	for (int i = 0; i < 16; i++) {
		printf("%02X", key[i]);
	}
	printf("\n\n");

	doAES(key, plain_text, output_data, 1);
	printf("normal text:\n ");
	for (int i = 0; i < 16; i++) {
		printf("%c", plain_text[i]);
	}
	printf("\n\n");
	printf("encrypted text:\n ");
	for (int i = 0; i < 16; i++) {
		printf("%02X", output_data[i]);
	}
	printf("\n\n");

	printf("secret message:\n ");
	for (int i = 0; i < 16; i++) {
		printf("%02X", secret_message[i]);
	}
	printf("\n\n");

	doAES(key, secret_message, output_data, 0);
	printf("decrypted message:\n ");
	for (int i = 0; i < 16; i++) {
		printf("%c", output_data[i]);
	}
	printf("\n");

	while(1)
	{
		Delay_Ms(1);
	}
}
