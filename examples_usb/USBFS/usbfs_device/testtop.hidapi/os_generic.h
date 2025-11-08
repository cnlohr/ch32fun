#ifndef _OS_GENERIC_H
#define _OS_GENERIC_H
/*
	"osgeneric" Generic, platform independent tool for threads and time.
		Geared around Windows and Linux. Designed for operation on MSVC,
		TCC, GCC and clang.  Others may work.

    It offers the following operations:

	Delay functions:
		void OGSleep( int is );
		void OGUSleep( int ius );

	Getting current time (may be time from program start, boot, or epoc)
		double OGGetAbsoluteTime();
		double OGGetFileTime( const char * file );

	Thread functions
		og_thread_t OGCreateThread( void * (routine)( void * ), void * parameter );
		void * OGJoinThread( og_thread_t ot );
		void OGCancelThread( og_thread_t ot );

	Mutex functions, used for protecting data structures.
		 (recursive on platforms where available.)
		og_mutex_t OGCreateMutex();
		void OGLockMutex( og_mutex_t om );
		void OGUnlockMutex( og_mutex_t om );
		void OGDeleteMutex( og_mutex_t om );

	Always a semaphore (not recursive)
		og_sema_t OGCreateSema(); //Create a semaphore, comes locked initially.
          NOTE: For platform compatibility, max count is 32767
		void OGLockSema( og_sema_t os );
		int OGGetSema( og_sema_t os );  //if <0 there was a failure.
		void OGUnlockSema( og_sema_t os );
		void OGDeleteSema( og_sema_t os );

	TLS (Thread-Local Storage)
		og_tls_t OGCreateTLS();
		void OGDeleteTLS( og_tls_t tls );
		void OGSetTLS( og_tls_t tls, void * data );
		void * OGGetTLS( og_tls_t tls );

   You can permute the operations of this file by the following means:
    OSG_NO_IMPLEMENTATION
	OSG_PREFIX
	OSG_NOSTATIC

   The default behavior is to do static inline.

   Copyright (c) 2011-2012,2013,2016,2018,2019,2020 <>< Charles Lohr

   This file may be licensed under the MIT/x11 license, NewBSD or CC0 licenses

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of this file.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

	Date Stamp: 2019-09-05 CNL: Allow for noninstantiation and added TLS.
	Date Stamp: 2018-03-25 CNL: Switched to header-only format.
*/


#if defined( OSG_NOSTATIC ) && OSG_NOSTATIC != 0
#ifndef OSG_PREFIX
#define OSG_PREFIX
#endif
#ifndef OSG_NO_IMPLEMENTATION
#define OSG_NO_IMPLEMENTATION
#endif
#endif

#ifndef OSG_PREFIX
#ifdef __wasm__
#define OSG_PREFIX
#else
#define OSG_PREFIX static inline
#endif
#endif

//In case you want to hook the closure of a thread, i.e. if your system has thread-local storage.
#ifndef OSG_TERM_THREAD_CODE
#define OSG_TERM_THREAD_CODE
#endif

typedef void* og_thread_t;
typedef void* og_mutex_t;
typedef void* og_sema_t;
typedef void* og_tls_t;

#ifdef __cplusplus
extern "C" {
#endif

OSG_PREFIX void OGSleep( int is );
OSG_PREFIX void OGUSleep( int ius );
OSG_PREFIX double OGGetAbsoluteTime();
OSG_PREFIX double OGGetFileTime( const char * file );
OSG_PREFIX og_thread_t OGCreateThread( void * (routine)( void * ), void * parameter );
OSG_PREFIX void * OGJoinThread( og_thread_t ot );
OSG_PREFIX void OGCancelThread( og_thread_t ot );
OSG_PREFIX og_mutex_t OGCreateMutex();
OSG_PREFIX void OGLockMutex( og_mutex_t om );
OSG_PREFIX void OGUnlockMutex( og_mutex_t om );
OSG_PREFIX void OGDeleteMutex( og_mutex_t om );
OSG_PREFIX og_sema_t OGCreateSema();
OSG_PREFIX int OGGetSema( og_sema_t os );
OSG_PREFIX void OGLockSema( og_sema_t os );
OSG_PREFIX void OGUnlockSema( og_sema_t os );
OSG_PREFIX void OGDeleteSema( og_sema_t os );
OSG_PREFIX og_tls_t OGCreateTLS();
OSG_PREFIX void OGDeleteTLS( og_tls_t key );
OSG_PREFIX void * OGGetTLS( og_tls_t key );
OSG_PREFIX void OGSetTLS( og_tls_t key, void * data );

#ifdef __cplusplus
};
#endif

#ifndef OSG_NO_IMPLEMENTATION

#if defined( WIN32 ) || defined (WINDOWS) || defined( _WIN32)
#define USE_WINDOWS
#endif


#ifdef __cplusplus
extern "C" {
#endif


#ifdef USE_WINDOWS

#include <windows.h>
#include <stdint.h>

OSG_PREFIX void OGSleep( int is )
{
	Sleep( is*1000 );
}

OSG_PREFIX void OGUSleep( int ius )
{
	Sleep( ius/1000 );
}

OSG_PREFIX double OGGetAbsoluteTime()
{
	static LARGE_INTEGER lpf;
	LARGE_INTEGER li;

	if( !lpf.QuadPart )
	{
		QueryPerformanceFrequency( &lpf );
	}

	QueryPerformanceCounter( &li );
	return (double)li.QuadPart / (double)lpf.QuadPart;
}


OSG_PREFIX double OGGetFileTime( const char * file )
{
	FILETIME ft;

	HANDLE h = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if( h==INVALID_HANDLE_VALUE )
		return -1;

	GetFileTime( h, 0, 0, &ft );

	CloseHandle( h );

	return ft.dwHighDateTime + ft.dwLowDateTime;
}


OSG_PREFIX og_thread_t OGCreateThread( void * (routine)( void * ), void * parameter )
{
	return (og_thread_t)CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)routine, parameter, 0, 0 );
}

OSG_PREFIX void * OGJoinThread( og_thread_t ot )
{
	WaitForSingleObject( ot, INFINITE );
	OSG_TERM_THREAD_CODE
	CloseHandle( ot );
	return 0;
}

OSG_PREFIX void OGCancelThread( og_thread_t ot )
{
	OSG_TERM_THREAD_CODE
	TerminateThread( ot, 0);
	CloseHandle( ot );	
}

OSG_PREFIX og_mutex_t OGCreateMutex()
{
	return CreateMutex( 0, 0, 0 );
}

OSG_PREFIX void OGLockMutex( og_mutex_t om )
{
	WaitForSingleObject(om, INFINITE);
}

OSG_PREFIX void OGUnlockMutex( og_mutex_t om )
{
	ReleaseMutex(om);
}

OSG_PREFIX void OGDeleteMutex( og_mutex_t om )
{
	CloseHandle( om );
}



OSG_PREFIX og_sema_t OGCreateSema()
{
	HANDLE sem = CreateSemaphore( 0, 0, 32767, 0 );
	return (og_sema_t)sem;
}

OSG_PREFIX int OGGetSema( og_sema_t os )
{
	typedef LONG NTSTATUS;
	HANDLE sem = (HANDLE)os;
	typedef NTSTATUS (NTAPI *_NtQuerySemaphore)(
		HANDLE SemaphoreHandle, 
		DWORD SemaphoreInformationClass, /* Would be SEMAPHORE_INFORMATION_CLASS */
		PVOID SemaphoreInformation,      /* but this is to much to dump here     */
		ULONG SemaphoreInformationLength, 
		PULONG ReturnLength OPTIONAL
	);

	typedef struct _SEMAPHORE_BASIC_INFORMATION {   
		ULONG CurrentCount; 
		ULONG MaximumCount;
	} SEMAPHORE_BASIC_INFORMATION;


	static _NtQuerySemaphore NtQuerySemaphore;
	SEMAPHORE_BASIC_INFORMATION BasicInfo;
	NTSTATUS Status;

	if( !NtQuerySemaphore )
	{	
	    NtQuerySemaphore = (_NtQuerySemaphore)GetProcAddress (GetModuleHandle ("ntdll.dll"), "NtQuerySemaphore");
		if( !NtQuerySemaphore )
		{
			return -1;
		}
	}

	
    Status = NtQuerySemaphore (sem, 0 /*SemaphoreBasicInformation*/, 
        &BasicInfo, sizeof (SEMAPHORE_BASIC_INFORMATION), NULL);

    if (Status == ERROR_SUCCESS)
    {       
        return BasicInfo.CurrentCount;
    }

	return -2;
}

OSG_PREFIX void OGLockSema( og_sema_t os )
{
	WaitForSingleObject( (HANDLE)os, INFINITE );
}

OSG_PREFIX void OGUnlockSema( og_sema_t os )
{
	ReleaseSemaphore( (HANDLE)os, 1, 0 );
}

OSG_PREFIX void OGDeleteSema( og_sema_t os )
{
	CloseHandle( os );
}

OSG_PREFIX og_tls_t OGCreateTLS()
{
	return (og_tls_t)(intptr_t)TlsAlloc();
}

OSG_PREFIX void OGDeleteTLS( og_tls_t key )
{
	TlsFree( (DWORD)(intptr_t)key );
}

OSG_PREFIX void * OGGetTLS( og_tls_t key )
{
	return TlsGetValue( (DWORD)(intptr_t)key );
}

OSG_PREFIX void OGSetTLS( og_tls_t key, void * data )
{
	TlsSetValue( (DWORD)(intptr_t)key, data );
}

#elif defined( __wasm__ )

//We don't actually have any function defintions here.
//The outside system will handle it.

#else

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <unistd.h>

OSG_PREFIX void OGSleep( int is )
{
	sleep( is );
}

OSG_PREFIX void OGUSleep( int ius )
{
	usleep( ius );
}

OSG_PREFIX double OGGetAbsoluteTime()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	return ((double)tv.tv_usec)/1000000. + (tv.tv_sec);
}

OSG_PREFIX double OGGetFileTime( const char * file )
{
	struct stat buff; 

	int r = stat( file, &buff );

	if( r < 0 )
	{
		return -1;
	}

	return buff.st_mtime;
}



OSG_PREFIX og_thread_t OGCreateThread( void * (routine)( void * ), void * parameter )
{
	pthread_t * ret = (pthread_t *)malloc( sizeof( pthread_t ) );
	if( !ret ) return 0;
	int r = pthread_create( ret, 0, routine, parameter );
	if( r )
	{
		free( ret );
		return 0;
	}
	return (og_thread_t)ret;
}

OSG_PREFIX void * OGJoinThread( og_thread_t ot )
{
	void * retval;
	if( !ot )
	{
		return 0;
	}
	pthread_join( *(pthread_t*)ot, &retval );
	OSG_TERM_THREAD_CODE
	free( ot );
	return retval;
}

OSG_PREFIX void OGCancelThread( og_thread_t ot )
{
	if( !ot )
	{
		return;
	}
#ifdef ANDROID
	pthread_kill( *(pthread_t*)ot, SIGTERM );
#else
	pthread_cancel( *(pthread_t*)ot );
#endif
	OSG_TERM_THREAD_CODE
	free( ot );
}

OSG_PREFIX og_mutex_t OGCreateMutex()
{
	pthread_mutexattr_t   mta;
	og_mutex_t r = malloc( sizeof( pthread_mutex_t ) );
	if( !r ) return 0;

	pthread_mutexattr_init(&mta);
	pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);

	pthread_mutex_init( (pthread_mutex_t *)r, &mta );

	return r;
}

OSG_PREFIX void OGLockMutex( og_mutex_t om )
{
	if( !om )
	{
		return;
	}
	pthread_mutex_lock( (pthread_mutex_t*)om );
}

OSG_PREFIX void OGUnlockMutex( og_mutex_t om )
{
	if( !om )
	{
		return;
	}
	pthread_mutex_unlock( (pthread_mutex_t*)om );
}

OSG_PREFIX void OGDeleteMutex( og_mutex_t om )
{
	if( !om )
	{
		return;
	}

	pthread_mutex_destroy( (pthread_mutex_t*)om );
	free( om );
}




OSG_PREFIX og_sema_t OGCreateSema()
{
	sem_t * sem = (sem_t *)malloc( sizeof( sem_t ) );
	if( !sem ) return 0;
	sem_init( sem, 0, 0 );
	return (og_sema_t)sem;
}

OSG_PREFIX int OGGetSema( og_sema_t os )
{
	int valp;
	sem_getvalue( (sem_t*)os, &valp );
	return valp;
}


OSG_PREFIX void OGLockSema( og_sema_t os )
{
	sem_wait( (sem_t*)os );
}

OSG_PREFIX void OGUnlockSema( og_sema_t os )
{
	sem_post( (sem_t*)os );
}

OSG_PREFIX void OGDeleteSema( og_sema_t os )
{
	sem_destroy( (sem_t*)os );
	free(os);
}

OSG_PREFIX og_tls_t OGCreateTLS()
{
	pthread_key_t ret = 0;
	pthread_key_create(&ret, 0);
	return (og_tls_t)(intptr_t)ret;
}

OSG_PREFIX void OGDeleteTLS( og_tls_t key )
{
	pthread_key_delete( (pthread_key_t)(intptr_t)key );
}

OSG_PREFIX void * OGGetTLS( og_tls_t key )
{
	return pthread_getspecific( (pthread_key_t)(intptr_t)key );
}

OSG_PREFIX void OGSetTLS( og_tls_t key, void * data )
{
	pthread_setspecific( (pthread_key_t)(intptr_t)key, data );
}

#endif

#ifdef __cplusplus
};
#endif

#endif //OSG_NO_IMPLEMENTATION

#endif //_OS_GENERIC_H


