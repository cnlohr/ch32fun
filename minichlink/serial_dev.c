#include "serial_dev.h"

int serial_dev_create(serial_dev_t *dev, const char* port, unsigned baud) {
	if (!dev) 
		return -1;
	dev->port = port;
	dev->baud = baud;
	#ifdef IS_WINDOWS
	dev->handle = INVALID_HANDLE_VALUE;
	#else
	dev->fd = -1;
	#endif
	return 0;
}

int serial_dev_open(serial_dev_t *dev) {
	fprintf(stderr, "Opening serial port %s at %u baud.\n", dev->port, dev->baud);
#ifdef IS_WINDOWS
	// Windows quirk: port = "COM10" is invalid, has to be encoded as "\\.\COM10".
	// This also works for COM below 9. So, let's give the user the ability to use
	// any "COMx" string and just prepend the "\\.\".
	char winPortName[64];
	if(dev->port[0] != '\\') {
		snprintf(winPortName, sizeof(winPortName), "\\\\.\\%s", dev->port);
	} else {
		// copy verbatim if string already starts with a '\'
		snprintf(winPortName, sizeof(winPortName), "%s", dev->port);
	}
	dev->handle = CreateFileA(winPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0,0);
	if (dev->handle == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			fprintf(stderr, "Serial port %s not found.\n", dev->port);
			// weird: without this, errno = 0 (no error).
			_set_errno(ERROR_FILE_NOT_FOUND);
			return -1; // Device not found
		}
		// Error while opening the device
		return -1;
	}
	DCB dcbSerialParams;
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(dev->handle, &dcbSerialParams)) {
		return -1;
	}
	// set baud and 8N1 serial formatting
	dcbSerialParams.BaudRate = dev->baud;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	// write back
	if (!SetCommState(dev->handle, &dcbSerialParams)){ 
		return -1;
	}
	// Set the timeout parameters to "no timeout" (blocking).
	// see https://learn.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-commtimeouts
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutConstant = MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = MAXDWORD;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	// Write the parameters
	if (!SetCommTimeouts(dev->handle, &timeouts)) {
		return -1;
	}
#else
	struct termios attr;
	if ((dev->fd = open(dev->port, O_RDWR | O_NOCTTY | O_NONBLOCK)) == -1) {
		perror("open");
		return -1;
	}

	// clear the nonblock flag
	if (fcntl(dev->fd, F_SETFL, 0) == -1) {
		perror("fcntl");
		close(dev->fd);
		return -1;
	}

	if (tcgetattr(dev->fd, &attr) == -1) {
		perror("tcgetattr");
		return -1;
	}

	cfmakeraw(&attr);
	cfsetspeed(&attr, dev->baud);
	attr.c_cflag |= CLOCAL;

	if (tcsetattr(dev->fd, TCSANOW, &attr) == -1) {
		perror("tcsetattr");
		return -1;
	}
#endif
	// all okay if we get here
	return 0;
}

int serial_dev_write(serial_dev_t *dev, const void* data, size_t len) {
#ifdef IS_WINDOWS
	DWORD dwBytesWritten;
	if (!WriteFile(dev->handle, data, len, &dwBytesWritten,NULL)) {
		return -1;
	}
	return (int) dwBytesWritten;
#else
	return write(dev->fd, data, len);
#endif
}

int serial_dev_read(serial_dev_t *dev, void* data, size_t len) {
#ifdef IS_WINDOWS
	DWORD dwBytesRead = 0;
	if (!ReadFile(dev->handle, data, len, &dwBytesRead, NULL)) {
		return -1;
	}
	return (int) dwBytesRead;
#else
	int got, count = 0;

	/* On Linux, raw mode only blocks on the first character: man 3 termios. */

	while (count < len) {
		got = read(dev->fd, (char *)data + count, len - count);
		if (got < 0)
			return got;
		count += got;
		if (count < len)
			usleep((100000 / dev->baud) * (len - count));
	}
	return count;
#endif
}

int serial_dev_do_dtr_reset(serial_dev_t *dev) {
#ifdef IS_WINDOWS
	// EscapeCommFunction returns 0 on fail
	if(EscapeCommFunction(dev->handle, SETDTR) == 0) {
		return -1;
	}
	if(EscapeCommFunction(dev->handle, CLRDTR) == 0) {
		return -1;
	}
#else
	int argp = TIOCM_DTR;
	// Arduino DTR reset.
	if (ioctl(dev->fd, TIOCMBIC, &argp) == -1) {
		perror("ioctl");
		return -1;
	}

	if (tcdrain(dev->fd) == -1) {
		perror("tcdrain");
		return -1;
	}

	if (ioctl(dev->fd, TIOCMBIS, &argp) == -1) {
		perror("ioctl");
		return -1;
	}
#endif
	return 0;
}

int serial_dev_flush_rx(serial_dev_t *dev) {
#ifdef IS_WINDOWS
	// PurgeComm returns 0 on fail
	if (PurgeComm(dev->handle, PURGE_RXCLEAR) == 0) {
		return -1;
	}
#else
	if (tcflush(dev->fd, TCIFLUSH) == -1) {
		perror("tcflush");
		return -1;
	}
#endif
	return 0;
}

int serial_dev_close(serial_dev_t *dev) {
#ifdef IS_WINDOWS
	if(!CloseHandle(dev->handle)) {
		return -1;
	}
	dev->handle = INVALID_HANDLE_VALUE;
#else
	int ret = 0;
	if((ret = close(dev->fd)) != 0) {
		return ret;
	}
	dev->fd = -1;
#endif
	return 0;
}
