#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "minichlink.h"

#define CMDSERVER_PORT 4444

#if defined( WIN32 ) || defined( _WIN32 )
#include <winsock2.h>
#if !defined( POLLIN ) 
typedef struct pollfd { SOCKET fd; SHORT  events; SHORT  revents; };
#define POLLIN 0x0001
#define POLLERR 0x008
#define POLLHUP 0x010
int WSAAPI WSAPoll(struct pollfd * fdArray, ULONG fds, INT timeout );
#endif
#define poll WSAPoll
#define socklen_t uint32_t
#define SHUT_RDWR SD_BOTH
#define MSG_NOSIGNAL 0
#else
#define closesocket close
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#endif

#ifdef __linux__
#include <linux/in.h>
#elif defined(__APPLE__)
#include <netinet/in.h>
#endif



// TODO: this is SOOOOOO Broken. Need to use select instead of bind for both servers.
static int g_cmdServerSocket;
static int g_cmdListenMode; // 0 for uninit.  1 for server, 2 for client.

static int CMDListen(void)
{
	struct sockaddr_in sin;
	g_cmdServerSocket = socket(AF_INET, SOCK_STREAM, 0);

	//Make sure the socket worked.
	if( g_cmdServerSocket == -1 )
	{
		fprintf( stderr, "Error: Cannot create socket.\n" );
		return -1;
	}

	//Disable SO_LINGER (Well, enable it but turn it way down)
#if defined( WIN32 ) || defined( _WIN32 )
	struct linger lx;
	lx.l_onoff = 1;
	lx.l_linger = 0;
	setsockopt( g_cmdServerSocket, SOL_SOCKET, SO_LINGER, (const char *)&lx, sizeof( lx ) );

	//Enable SO_REUSEADDR
	int reusevar = 1;
	setsockopt( g_cmdServerSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reusevar, sizeof(reusevar));
#else
	struct linger lx;
	lx.l_onoff = 1;
	lx.l_linger = 0;
	setsockopt( g_cmdServerSocket, SOL_SOCKET, SO_LINGER, &lx, sizeof( lx ) );

	//Enable SO_REUSEADDR
	int reusevar = 1;
	setsockopt(g_cmdServerSocket, SOL_SOCKET, SO_REUSEADDR, &reusevar, sizeof(reusevar));
#endif
	//Setup socket for listening address.
	memset( &sin, 0, sizeof( sin ) );
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons( CMDSERVER_PORT );

	//Actually bind to the socket
	if( bind( g_cmdServerSocket, (struct sockaddr *) &sin, sizeof( sin ) ) == -1 )
	{
		fprintf( stderr, "Could not bind to socket: %d\n", CMDSERVER_PORT );
		closesocket( g_cmdServerSocket );
		g_cmdServerSocket = 0;
		return -1;
	}

	//Finally listen.
	if( listen( g_cmdServerSocket, 5 ) == -1 )
	{
		fprintf(stderr, "Could not lieten to socket.");
		closesocket( g_cmdServerSocket );
		g_cmdServerSocket = 0;
		return -1;
	}

	// fprintf( stderr, "CMD server running on port %d\n", CMDSERVER_PORT );
	
	return 0;
}

static int CMDInit(void)
{
#if defined( WIN32 ) || defined( _WIN32 )
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		fprintf( stderr, "WSAStartup failed with error: %d\n", err);
		return 1;
	}
#endif

	g_cmdListenMode = 1;

	return CMDListen();
}

void CMDWrite(void *dev, uint32_t datareg, uint32_t value)
{
	if( MCF.WriteReg32 && MCF.FlushLLCommands )
	{
		struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
		iss->statetag = STTAG( "XXXX" );
		MCF.FlushLLCommands( dev );
		MCF.WriteReg32( dev, datareg, value );
		MCF.FlushLLCommands( dev );
	}
	else
	{
		fprintf( stderr, "Error: WriteReg32 or FlushLLCommands not implemented.\n" );
	}
}

uint32_t CMDRead(void *dev, uint32_t datareg)
{
	uint32_t value = 0;
	if( MCF.ReadReg32 && MCF.FlushLLCommands )
	{
		struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
		iss->statetag = STTAG( "XXXX" );
		MCF.FlushLLCommands( dev );
		int ret = MCF.ReadReg32( dev, datareg, &value );
		if( ret < 0 )
		{
			fprintf( stderr, "Error reading register %02x: %d\n", datareg, ret );
		}
	}
	else
	{
		fprintf( stderr, "Error: ReadReg32 or FlushLLCommands not implemented.\n" );
	}
	return value;
}

void CMDRequestHandler(void *dev, const char *request, size_t size, char *out, size_t outsize)
{
	// printf( "Received(%d): %.*s\n", (int)size, (int)size, request );
	char *cmd = (char*)request;
	while( *cmd && size > 0 )
	{
		switch(*cmd)
		{
			case 's': // Write command
			{
				uint32_t datareg = 0;
				uint32_t value = 0;
				int consumed = 0;
				int ret = sscanf( cmd + 1, " %x %x%n", &datareg, &value, &consumed);
				if( ret == 2 )
				{
					CMDWrite( dev, datareg, value );
				}
				else
				{
					fprintf( stderr, "Error parsing write command: %s\n", cmd );
				}
				cmd += consumed + 1; // Move past the command and the two hex values
				break;
			}
			case 'm': // Read command
			{
				uint32_t datareg = 0;
				int consumed = 0;
				int ret = sscanf( cmd + 1, " %x%n", &datareg, &consumed );
				if( ret == 1 )
				{
					uint32_t val = CMDRead( dev, datareg );
					int ret = snprintf( out, outsize, "%02x: %08x\n", datareg, val );
					if( ret < 0 )
					{
						fprintf( stderr, "Error formatting read command output: %s\n", cmd );
						return;
					}

					if( ret >= outsize )
					{
						fprintf( stderr, "Output buffer too small for read command output: %s\n", cmd );
						return;
					}
					out += ret;
					outsize -= ret;
				}
				else
				{
					fprintf( stderr, "Error parsing read command: %s\n", cmd );
				}
				cmd += consumed + 1; // Move past the command and the hex value
				break;
			}
			case '\r':
			case '\n': // Newline command
			{
				return;
			}
			default:
			{
				cmd++; // Move to the next character
				break;
			}
		}
	}

}

int CMDPollServer( void * dev )
{
										//
	if( !g_cmdServerSocket ) return -4;

	int pollct = 1;
	struct pollfd allpolls[1] = { 0 };
	allpolls[0].fd = g_cmdServerSocket;
#if defined( WIN32 ) || defined( _WIN32 )
	allpolls[0].events = 0x00000100; //POLLRDNORM;
#else
	allpolls[0].events = POLLIN;
#endif
	int r = poll( allpolls, pollct, 0 );

	if( r < 0 )
	{
		fprintf( stderr, "R poll(...): %d\n", r );
	}

	//If there's faults, bail.
	if( allpolls[0].revents & (POLLERR|POLLHUP) )
	{
		closesocket( g_cmdServerSocket );
		if( g_cmdListenMode == 1 )
		{
			// Some sort of weird fatal close?  Is this even possible?
			fprintf( stderr, "Error: serverSocke was forcibly closed\n" );
			exit( -4 );
		}
		else if( g_cmdListenMode == 2 )
		{
			if( g_cmdServerSocket ) 	closesocket( g_cmdServerSocket );
			g_cmdServerSocket = 0;
			g_cmdListenMode = 1;
			CMDListen();
		}
	}
	if( allpolls[0].revents & POLLIN )
	{
		if( g_cmdListenMode == 1 )
		{
			struct	sockaddr_in tin;
			socklen_t addrlen  = sizeof(tin);
			memset( &tin, 0, addrlen );
			int tsocket = accept( g_cmdServerSocket, (struct sockaddr *)&tin, (void*)&addrlen );
			closesocket( g_cmdServerSocket );
			g_cmdServerSocket = tsocket;
			g_cmdListenMode = 2;
			// fprintf( stderr, "Connection established to cmd server client\n" );	
		}
		else if( g_cmdListenMode == 2 )
		{
			// Got data from a peer.
			uint8_t buffer[16384];
			uint8_t outbuffer[16384];
			outbuffer[0] = 0; // Null-terminate the output buffer.
			ssize_t rx = recv( g_cmdServerSocket, (char*)buffer, sizeof( buffer ), MSG_NOSIGNAL );
			if( rx > 0 )
			{
				// Save the current DMDATA0 register value, so we can restore it later.
				uint32_t data0;
				MCF.FlushLLCommands( dev );
				(void)MCF.ReadReg32( dev, DMDATA0, &data0 );
				MCF.FlushLLCommands( dev );

				CMDRequestHandler( dev, (const char*)buffer, rx , (char*)outbuffer, sizeof( outbuffer ) );
				// Restore the DMDATA0 register value.
				MCF.WriteReg32( dev, DMDATA0, data0 );
				int outsize = strlen( (const char*)outbuffer );
				if( outsize > 0 )
				{
					ssize_t tx = send( g_cmdServerSocket, (const char*)outbuffer, outsize, MSG_NOSIGNAL );
					if( tx < 0 )
					{
						fprintf( stderr, "Error sending response: %zd\n", tx );
					}
				}
			}
			closesocket( g_cmdServerSocket );
			g_cmdServerSocket = 0;
			g_cmdListenMode = 1;
			CMDListen();
			return 1; // Indicate that we processed a command.
		}
	}

	return 0;
}
