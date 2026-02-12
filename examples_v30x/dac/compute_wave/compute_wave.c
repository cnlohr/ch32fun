#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define LEN 2048
#define PI 3.1415926535

void compute_waves(uint32_t *buf, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		double t = (double)i / size;

		double sinVal = (sin(2 * PI * t) + 1.0f) / 2.0f;
		double cosVal = (cos(2 * PI * t) + 1.0f) / 2.0f;
		
		buf[i] = ((uint16_t)(sinVal*0xFFF) << 16) | ((uint16_t)(cosVal*0xFFF));
	}
}

int main()
{
	uint32_t buf[LEN];
	compute_waves(buf, LEN);

	FILE *f = fopen("../wave.h", "w+");

	fprintf(f,
		"#ifndef _WAVE\n"
		"#define _WAVE\n"
		"#define DAC_BUF_SIZE %d\n"
		"uint32_t dac_buf[DAC_BUF_SIZE] = {\n"
	, LEN);
	fprintf(f, "\t");
	for (size_t i = 0; i < LEN; i++)
	{
		if ( (i % 8)==0 && (i != 0))
			fprintf(f, "\n\t");
		fprintf(f, "0x%08X, ", buf[i]);
	}
	fseek(f, -2, SEEK_CUR);
	fprintf(f,
		"\n};\n"
		"#endif // _WAVE\n"
	);	
	fclose(f);
	return 0;
}