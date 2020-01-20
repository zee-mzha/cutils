#include "errors.h"

const char* cutilsErrorToStr(int code){
	switch(code){
		case CUTILS_OK:
			return "No error.";
		case CUTILS_NOMEM:
			return "Failed to alloacte memory.";
		case CUTILS_OUT_OF_BOUNDS:
			return "Out of bounds access.";
		case CUTILS_FOPEN:
			return "failed to open file.";
		case CUTILS_GETADDRINFO:
			return "failed getaddrinfo call.";
		case CUTILS_SOCKET:
			return "failed to create socket.";
		#ifndef CUTILS_NO_LIBEVENT
		case CUTILS_CREATE_EVENT:
			return "failed to create event or timeout.";
		#endif
		default:
			return "unkown error code.";
	}
}
