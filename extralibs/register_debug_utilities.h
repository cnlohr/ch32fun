//! ####################################
//! PRINT BITS
//! ####################################

// eg: UTIL_PRINT_BITS(reg, 32, "\n", 16);
void UTIL_PRINT_BITS(u32 val, u8 len, const char* format, ...) {
	const char* separator = "|";
	int divider_len = 8;
	
	if (format != NULL) {
		va_list args;
		va_start(args, format);
		
		if (format[0] != '\0') {  // Non-empty string means we have at least one arg
			separator = format;
			
			// Check if we have a second arg
			int div_arg = va_arg(args, int);
			if (div_arg > 0) divider_len = div_arg;
		}
		
		va_end(args);
	}
	
	printf("[");
	for (int i = (len)-1; i >= 0; i--) {
		printf("%d", ((val) >> i) & 1);
		if (i > 0) printf(i % divider_len ? " " : " %s ", separator);
	}
	printf("]\n");
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
		printf("%s=%d", pairs[i].name, (reg >> pairs[i].pos) & 1);
	}
}

//! ####################################
//! PRINT BITS RANGE
//! ####################################

// eg: UTIL_PRINT_BIT_RANGE(
//	reg,
//	"FIELD1", 0, 0,
//	"FIELD2", 1, 2,
//	"FIELD3", 3, 5,
//	NULL
// );

// NOTE: NULL terminated REQUIRED
void UTIL_PRINT_BIT_RANGE(u32 reg, ...) {
	// Define the struct
	typedef struct { const char* name; int start; int end; } BitField;
	
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
		int end = va_arg(count_args, int);
		field_count++;
	}
	va_end(count_args);
	
	// Read all the fields
	BitField fields[field_count];
	for (int i = 0; i < field_count; i++) {
		fields[i].name = va_arg(args, const char*);
		fields[i].start = va_arg(args, int);
		fields[i].end = va_arg(args, int);
	}
	
	va_end(args);
	
	// Print the fields
	for (int i = 0; i < field_count; i++) {
		if (i > 0) printf(", ");
		
		if (fields[i].end == fields[i].start) {
			// Single bit
			printf("%s=%d", fields[i].name, (reg >> fields[i].start) & 1);
		} else {
			// Multiple bits
			int mask = ((1 << (fields[i].end - fields[i].start + 1)) - 1);
			printf("%s=0x%02X", fields[i].name, (reg >> fields[i].start) & mask);
		}
	}
	printf("\n");
}


//! ####################################
//! PRINT REGS
//! ####################################

#define UTIL_PRINT_REG8(reg, label) printf("%s: 0x%02X\n", label, reg);
#define UTIL_PRINT_REG16(reg, label) printf("%s: 0x%04X\n", label, reg);
#define UTIL_PRINT_REG32(reg, label) printf("%s: 0x%08lX\n", label, reg);


