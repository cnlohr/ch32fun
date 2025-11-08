#define UTIL_PRINT_BITS(val, len, separator) do { \
	for (int i = len-1; i >= 0; i--) { \
		printf("%s", (((val) >> i) & 1) ? "1" : "0"); \
		if (i % 8 == 0 && i != 0) printf(" %s ", separator); \
		else if (i > 0) printf(" "); \
	} \
} while(0)

#define UTIL_PRINT_BITS8(val) UTIL_PRINT_BITS(val, 8, "|")
#define UTIL_PRINT_BITS16(val) UTIL_PRINT_BITS(val, 16, "|")
#define UTIL_PRINT_BITS32(val) UTIL_PRINT_BITS(val, 32, "|")

#define UTIL_PRINT_REG8(reg, label) do { \
	printf("%s: 0x%02X [", label, reg); \
	UTIL_PRINT_BITS8(reg); \
	printf("]\n"); \
} while(0)

#define UTIL_PRINT_REG16(reg, label) do { \
	printf("%s: 0x%04X [", label, reg); \
	UTIL_PRINT_BITS16(reg); \
	printf("]\n"); \
} while(0)

#define UTIL_PRINT_REG32(reg, label) do { \
	printf("%s: 0x%08lX [", label, reg); \
	UTIL_PRINT_BITS32(reg); \
	printf("]\n"); \
} while(0)
