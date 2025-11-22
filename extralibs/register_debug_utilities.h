#include <stdio.h>

//! ####################################
//! PRINT BITS
//! ####################################

// eg: UTIL_PRINT_BITS(reg, 32, 16);
void UTIL_PRINT_BITS(u32 val, u8 len, u8 divider_len) {
	const char* separator = "\n";

	for (int i = (len)-1; i >= 0; i--) {
		printf("%d| ", i);
		if (i > 0) printf(i % divider_len ? "" : "%s", separator);
	}
	printf("\n");

	for (int i = (len)-1; i >= 0; i--) {
		printf(i < 10 ? "%2d" : "%3d", ((val) >> i) & 1);
		if (i > 0) printf(i % divider_len ? " " : " %s", separator);
	}
}

//! ####################################
//! PRINT BITS VALUES
//! ####################################

// eg: UTIL_PRINT_BITS_VALUES(
//	reg, 
//	"a", 0,
//	"b", 1,
//	"c", 2,
// );

void UTIL_PRINT_BITS_VALUES(u32 reg, ...) {
	struct BitPair { const char* name;  int pos; };
	va_list args;
	va_start(args, reg);
	
	// Count how many pairs were passed (until NULL name)
	int pair_count = 0;
	va_list count_args;
	va_copy(count_args, args);
	
	while (1) {
		const char* name = va_arg(count_args, const char*);
		if (name == NULL) break;
		int pos = va_arg(count_args, int);
		(void)pos;
		pair_count++;
	}
	va_end(count_args);
	
	// Read all the pairs
	struct BitPair pairs[pair_count];
	for (int i = 0; i < pair_count; i++) {
		pairs[i].name = va_arg(args, const char*);
		pairs[i].pos = va_arg(args, int);
	}
	
	va_end(args);
	
	// Print the pairs
	for (int i = 0; i < pair_count; i++) {
		if (i > 0) printf(", ");
		printf("%s=%d", pairs[i].name, (unsigned int)((reg >> pairs[i].pos) & 1));
	}
}

//! ####################################
//! PRINT BITS RANGE
//! ####################################

// eg: UTIL_PRINT_BIT_RANGE(
//	reg,
//	"FIELD3", 5, 3,
//	"FIELD2", 2, 1,
//	"FIELD1", 0, 0,
//	NULL
// );

// NOTE: NULL terminated REQUIRED
void UTIL_PRINT_BIT_RANGE(u32 reg, ...) {
	// Define the struct
	typedef struct { const char* name; int end; int start; } BitField;
	
	va_list args;
	va_start(args, reg);
	
	// Count how many fields were passed (until NULL name)
	int field_count = 0;
	va_list count_args;
	va_copy(count_args, args);
	
	while (1) {
		const char* name = va_arg(count_args, const char*);
		if (name == NULL) break;
		int start = va_arg(count_args, int);
		(void)start;
		int end = va_arg(count_args, int);
		(void)end;
		field_count++;
	}
	va_end(count_args);
	
	// Read all the fields
	BitField fields[field_count];
	for (int i = 0; i < field_count; i++) {
		fields[i].name = va_arg(args, const char*);
		fields[i].end = va_arg(args, int);			// get end first
		fields[i].start = va_arg(args, int);		// then get start
	}
	
	va_end(args);
	
	// Print the fields
	for (int i = 0; i < field_count; i++) {
		if (i > 0) printf(", ");
		int start = fields[i].start;
		int end = fields[i].end;
		
		if (end == start) {
			// Single bit
			printf("%s = %d [%d]", fields[i].name, (reg >> start) & 1, start);
		} else {
			// Multiple bits
			int mask = ((1 << (end - start + 1)) - 1);
			printf("%s = 0x%02X [%d:%d]", fields[i].name, (reg >> start) & mask, end, start);
		}
	}
	printf("\n");
}


//! ####################################
//! PRINT REGS
//! ####################################

#define UTIL_PRINT_REG8(reg, label) printf("%s: 0x%02X\n", label, (unsigned int)(reg));
#define UTIL_PRINT_REG16(reg, label) printf("%s: 0x%04X\n", label, (unsigned int)(reg));
#define UTIL_PRINT_REG32(reg, label) printf("%s: 0x%08X\n", label, (unsigned int)(reg));


