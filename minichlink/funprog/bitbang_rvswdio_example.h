static inline void Send1BitSWIO( struct SWIOState * state )
{
	// Low for a nominal period of time.
	// High for a nominal period of time.
}

static inline void Send0BitSWIO( struct SWIOState * state )
{
	// Low for a LONG period of time.
	// High for a nominal period of time.
}

// returns 0 if 0
// returns 1 if 1
// returns 2 if timeout.
static inline int ReadBitSWIO( struct SWIOState * state )
{
	// Drive low, very briefly.  Let drift high.
	// See if target chip is holding low.
}


static inline void SendBitRVSWD( struct SWIOState * state, int val )
{
	// Assume:
	// SWD is in indeterminte state.
	// SWC is HIGH
}

static inline int ReadBitRVSWD( struct SWIOState * state )
{
}


static void MCFWriteReg32( struct SWIOState * state, uint8_t command, uint32_t value )
{
}

// returns 0 if no error, otherwise error.
static int MCFReadReg32( struct SWIOState * state, uint8_t command, uint32_t * value )
{
}

static inline void ConfigureIOForRVSWIO(struct SWIOState * state)
{
}

static inline void ConfigureIOForRVSWD(struct SWIOState * state)
{
}
