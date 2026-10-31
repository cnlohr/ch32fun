#ifndef FUNPROG_LOGIC_H
#define FUNPROG_LOGIC_H

#define PROGRAMMER_VERSION_MAJOR 5
#define PROGRAMMER_VERSION_MINOR 30

static const uint16_t capabilities = (PROGRAMMER_TYPE << 12) | CAPABILITIES;
static uint8_t programmer_mode;
static struct SWIOState state;

#if defined(UPDI_SUPPORT) && UPDI_SUPPORT
static struct UPDIState updi;
#endif

void SwitchMode( uint8_t ** liptr, uint8_t ** lretbuffptr )
{
	programmer_mode = *((*liptr)++);
	*((*lretbuffptr)++) = (uint8_t)PROGRAMMER_VERSION_MAJOR;
	*((*lretbuffptr)++) = programmer_mode;
	*((*lretbuffptr)++) = PROGRAMMER_VERSION_MINOR;
	*((*lretbuffptr)++) = (uint8_t)(capabilities>>8);
	*((*lretbuffptr)++) = (uint8_t)capabilities;
	FUN_PRINTF_DEBUG( "Changing programming mode to %d\n", programmer_mode );
}

static void HandleCommandBuffer( uint8_t * buffer, uint16_t reqlen, uint8_t* ret_buffer, size_t ret_size )
{
	// buffer[0] is the request ID.
	uint8_t * iptr = &buffer[1];
	uint8_t * retbuffptr = ret_buffer+1;
	STATUS_LED_ON;
	FUN_PRINTF_DEBUG( "IRQL: %d\n", reqlen );
	while( iptr - buffer < reqlen )	
	{
		uint8_t cmd = *(iptr++);
		int remain = reqlen - (iptr - buffer);
		
		FUN_PRINTF_DEBUG( "CMD: %02x\n", cmd );
    // Make sure there is plenty of space.
		if( (ret_size-(retbuffptr - ret_buffer)) < 6 ) break;

		if( programmer_mode == 0 )
		{
			if( cmd == 0xfe ) // We will never write to 0x7f.
			{
				cmd = *(iptr++);
				FUN_PRINTF_DEBUG( "ACMD: %02x\n", cmd );

				switch( cmd )
				{
				case 0xfd:
					SwitchMode( &iptr, &retbuffptr );
					break;
				case 0x0e:
				case 0x01:
				{
					//DoSongAndDanceToEnterPgmMode( &state ); was 1.  But really we just want to init.
					// if we expect this, we can use 0x0e to get status.
					POWER_SETUP;
					int r = InitializeSWDSWIO( &state );
					if( cmd == 0x0e )
						*(retbuffptr++) = r;
					break;
				}
				case 0x02: // Power-down 
					POWER_OFF;
					break;
				case 0x03: // Power-up
					POWER_ON;
					break;
				case 0x04: // Delay( uint16_t us )
					DELAY_US(iptr[0] | (iptr[1]<<8) );
					iptr += 2;
					break;
				case 0x05: // Void High Level State
					ResetInternalProgrammingState( &state );
					break;
				case 0x06: // Wait-for-flash-op.
					*(retbuffptr++) = WaitForFlash( &state );
					break;
				case 0x07: // Wait-for-done-op.
					*(retbuffptr++) = WaitForDoneOp( &state );
					break;
				case 0x08: // Write Data32.
				{
					if( remain >= 9 )
					{
						int r = WriteWord( &state, iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24),  iptr[4] | (iptr[5]<<8) | (iptr[6]<<16) | (iptr[7]<<24) );
						iptr += 8;
						*(retbuffptr++) = r;
					}
					break;
				}
				case 0x09: // Read Data32.
				{
					if( remain >= 4 )
					{
						int r = ReadWord( &state, iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24), (uint32_t*)&retbuffptr[1] );
						FUN_PRINTF_DEBUG( "READING: %08x -> %08x\n", (iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24)), (retbuffptr[1] | retbuffptr[2]<<8 | retbuffptr[3]<<16 | retbuffptr[4]<<24) );
						iptr += 4;
						retbuffptr[0] = r;
						if( r < 0 )
							memset( &retbuffptr[1], 0, 4 );
						retbuffptr += 5;
					}
					break;
				}
				case 0x0a:
					ResetInternalProgrammingState( &state );
					break;
				case 0x0b:
					if( remain >= 68 )
					{
						int r = WriteBlock( &state, iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24), (uint8_t*)&iptr[4], 64, 1 );
						iptr += 68;
						*(retbuffptr++) = r;
					}
					break;
#if defined(MCO_FUNCTION) && MCO_FUNCTION
				case 0x0c:
					if( remain >= 8 )
					{
						int use_apll = *(iptr++);  // try 1
						int sdm0 = *(iptr++);      // try 0
						int sdm1 = *(iptr++);      // try 0
						int sdm2 = *(iptr++);      // try 8
						int odiv = *(iptr++);      // try 0
						iptr +=3 ; // reserved.

						// On the ESP32-S2, this will output clock on P2.
						EnableMCO( use_apll, sdm0, sdm1, sdm2, odiv );
					}
					break;
#endif
				case 0x0d:  // Do a terminal log through.
				{
					int tries = 100;
					if( remain >= 8 )
					{
						int r;
						uint32_t leavevalA = iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24);
						iptr += 4;
						uint32_t leavevalB = iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24);
						iptr += 4;
						uint8_t * origretbuf = (retbuffptr++);
						// int canrx = (ret_size-(retbuffptr - ret_buffer)) - 8;
						int realmaxlen = reqlen - 4;
						if( realmaxlen > 250 ) realmaxlen = 250;
						int canrx = (realmaxlen - (retbuffptr - ret_buffer));
						while( canrx > 8 )
						{
							r = PollTerminal( &state, retbuffptr, canrx, leavevalA, leavevalB );
							origretbuf[0] = r;
							if( r >= 0 )
							{
								retbuffptr += r;
								if( tries-- <= 0 ) break; // ran out of time?
							}
							else
							{
								break;
							}
							canrx = (realmaxlen - (retbuffptr - ret_buffer)) - 8;
							// Otherwise all is well.  If we aren't signaling try to poll for more data.
							if( leavevalA != 0 || leavevalB != 0 ) break;
						}
					}
					break;
				}
				case 0x0f: // Override chip type, etc.
					if( remain >= 8 )
					{
						state.target_chip_type = *(iptr++);
						if( state.target_chip_type == CHIP_CH59x ) SLOW_DOWN_PROTOCOL;
						state.sectorsize = iptr[0] | (iptr[1]<<8);
						iptr += 2;
						// Rest is reserved.
						iptr += 5;
						*(retbuffptr++) = 0; // Reply is always 0.
					}
					break;
				case 0x10: //DetermineChip
					{
						int r = DetermineChipTypeAndSectorInfo( &state, &retbuffptr[1] );
						if( state.target_chip_type == CHIP_CH59x ) SLOW_DOWN_PROTOCOL;
						retbuffptr[0] = 0xce;
						if( r < 0 )
							memset( &retbuffptr[1], 0, 6 );
						retbuffptr += 7;
					}
					break;
				// case 0x11:
					//Set clock
					// break;
				case 0x12: //WriteBlock
					if( remain >= 70 )
					{
						uint8_t length = iptr[4];
						uint8_t erase = iptr[5];
						int r = WriteBlock( &state, iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24), (uint8_t*)&iptr[6], length, erase );
						iptr += length + 2;
						*(retbuffptr++) = r;
					}
					break;
#if CH5xx_SUPPORT
				case 0x20: //ch5xx_write_flash
				case 0x21:
					if( remain >= 260 )
					{
						int8_t r = 0;
						uint32_t length = iptr[4];
						if( length == 0 ) length = 256;
						if( cmd == 0x20 ) r = ch5xx_write_flash( &state, iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24), (uint8_t*)&iptr[5], length );
						else r = ch5xx_write_flash_using_microblob( &state, iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24), (uint8_t*)&iptr[5], length ); 
						iptr += 260;
						*(retbuffptr++) = r;
					}
					break;
				case 0x22: // End writing
					{
						if( state.microblob_running > -1 ) ch5xx_microblob_end( &state );
						if( state.writing_flash > 0 )
						{
							ch5xx_flash_close( &state );
							state.writing_flash = 0;
						}
					}
					break;
#if CH5xx_EEPROM
					case 0x23: // CH5xx read EEPROM (356 bytes)
					if( remain >= 6 ) {
						uint32_t length = iptr[4] | (iptr[5]<<8);
						if( length <= (ret_size-(retbuffptr - ret_buffer) - 1) )
						{
							int8_t r = ch5xx_read_eeprom( &state, iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24), &retbuffptr[1], length );
							retbuffptr[0] = r;
							if( r < 0 )
								memset( &retbuffptr[1], 0, length );
							retbuffptr += length + 1;
						}
						else
						{
							retbuffptr[0] = -100;
						}
						iptr += 8;
					}
					break;
#endif
#if CH5xx_OPTIONS
				case 0x24: // CH5xx read Option Bytes (~148 bytes)
					if( remain >= 6 ) {
						uint32_t length = iptr[4] | (iptr[5]<<8);
						if( length <= (ret_size-(retbuffptr - ret_buffer) - 1) )
						{
							int8_t r = ch5xx_read_options_bulk( &state, iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24), &retbuffptr[1], length );
							retbuffptr[0] = r;
							if( r < 0 )
								memset( &retbuffptr[1], 0, length );
							retbuffptr += length + 1;
						}
						else
						{
							retbuffptr[0] = -100;
						}
						iptr += 8;
					}
					break;
#endif
				case 0x25: //CH5xx Erase
					if( remain >= 9 )
					{
						int8_t r = CH5xxErase( &state, iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24), iptr[4] | (iptr[5]<<8) | (iptr[6]<<16) | (iptr[7]<<24), (enum MemoryArea)(iptr[8]));
						iptr += 9;
						*(retbuffptr++) = r;
					}
					break;
				case 0x26: // ch5xx_read_uuid was here, maybe a place for ch5xx_verify_data
					break;
#if CH5xx_UNLOCK
				case 0x27: // This also optional, (428 bytes)
					ch570_disable_read_protection( &state );
					break;
#endif
				case 0x28:
					if( remain >= 4 )
					{
						int8_t r = ch5xx_set_clock( &state, iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24) );
						iptr += 4;
						*(retbuffptr++) = r;
					}
					break;
#endif
				// Done
				}
			} else if( cmd == 0xff )
			{
				break;
			}
			else
			{
				// Otherwise it's a regular command.
				// 7-bit-cmd .. 1-bit read(0) or write(1) 
				// if command lines up to a normal QingKeV2 debug command, treat it as that command.
				// Now addresses are shifted by -1 to fit 0x7f register
				FUN_PRINTF_DEBUG( "Reg Operation %02x %d\n", cmd>>1, cmd & 1 );

				if( cmd & 1 )
				{
					if( remain >= 4 )
					{
						cmd = (cmd >> 1) + 1; // Unshift from -1 to be able to read 0x7f
						if( cmd >= DMPROGBUF0 && cmd <= DMPROGBUF7 ) state.statetag = STTAG( "XXXX" ); // Reset statetag in case we are writing to progbuf from outside
						MCFWriteReg32( &state, cmd, iptr[0] | (iptr[1]<<8) | (iptr[2]<<16) | (iptr[3]<<24) );
						iptr += 4;
					}
				}
				else
				{
					if( remain >= 1 && (ret_size-(retbuffptr - ret_buffer)) >= 4 )
					{
						cmd = (cmd >> 1) + 1;
						int r = MCFReadReg32( &state, cmd, (uint32_t*)&retbuffptr[1] );
						retbuffptr[0] = r;
						if( r < 0 )
							memset( &retbuffptr[1], 0, 4 );
						retbuffptr += 5;
					}
				}
			}
		}
		else if( programmer_mode == 1 )
		{
			if( cmd == 0xff ) break;
#if UPDI_SUPPORT
			switch( cmd )
			{
			case 0xfe:
			{
				cmd = *(iptr++);
				if( cmd == 0xfd )
				{
					SwitchMode( &iptr, &retbuffptr );
				}
				break;
			}
			case 0x90:
			{
				UPDIPinSetup( &updi );
				updi.cpumhz = GetCPUClock();

				if( (ret_size-(retbuffptr - ret_buffer)) >= 18 && remain >= 2 )
				{
					updi.baudrate = *(iptr++);
					updi.baudrate |= (*(iptr++))<<8;

					updi.clocks_per_bit = UPDIComputeClocksPerBit( updi.cpumhz, updi.baudrate*32 );

					UPDIPowerOn( &updi );
					uint8_t sib[17] = { 0 };
					int r = UPDISetup( &updi, sib );
					FUN_PRINTF_DEBUG( "UPDISetup() = %d -> %s\n", r, sib );

					retbuffptr[0] = r;
					memcpy( retbuffptr + 1, sib, 17 );
					retbuffptr += 18;
				}
				break;
			}
			case 0x91:
			{
				UPDIPinSetup( &updi );
				UPDIPowerOn( &updi );
				break;
			}
			case 0x92:
			{
				UPDIPinSetup( &updi );
				UPDIPowerOff( &updi );
				break;
			}
			case 0x93: // Flash 64-byte block.
			{
				UPDIPinSetup( &updi );

				if( remain >= 2+64 )
				{
					int addytowrite = *(iptr++);
					addytowrite |= (*(iptr++))<<8;
					int r;
					r = UPDIFlash( &updi, addytowrite, iptr, 64, 0);
					FUN_PRINTF_DEBUG( "Flash Response: %d\n", r );
					iptr += 64;

					*(retbuffptr++) = r;
				}
				break;
			}
			case 0x94:
			{
				UPDIPinSetup( &updi );

				if( remain >= 3 )
				{
					int addytorx = *(iptr++);
					addytorx |= (*(iptr++))<<8;
					int bytestorx = *(iptr++);

					if( (ret_size-(retbuffptr - ret_buffer)) >= bytestorx + 1 )
					{
						retbuffptr[0] = UPDIReadMemoryArea( &updi, addytorx, (uint8_t*)&retbuffptr[1], bytestorx );
						retbuffptr += bytestorx + 1;
					}
				}
				break;
			}
			case 0x95:
			{
				UPDIPinSetup( &updi );

				*(retbuffptr++) = UPDIErase( &updi );
				break;
			}
			case 0x96:
			{
				*(retbuffptr++) = UPDIReset( &updi );

				break;
			}
			default:
#else
			{
#endif
				*(retbuffptr++) = 0xfe;
				*(retbuffptr++) = cmd;
				break;
			}
		}
		else
		{
			// Unknown Programmer
			*(retbuffptr++) = 0;
			*(retbuffptr++) = programmer_mode;

			break;
		}
	}
	FUN_PRINTF_DEBUG( "IRQO: %d // EP: %d\n", retbuffptr - retbuff - 1, iptr - buffer );
	ret_buffer[0] = (retbuffptr - ret_buffer) - 1;
	STATUS_LED_OFF;
}

#endif
