#pragma once

#include "Logging.h"

#ifdef _DEBUG
    #if defined(PLATFORM_WIN32)
		#define CG_DEBUG_BREAK() __debugbreak();
	#elif defined(PLATFORM_UNIX)
		#include <signal.h>
		#define CG_DEBUG_BREAK() raise(SIGTRAP);
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define CG_ENABLE_ASSERTS
#else
    #define CG_DEBUG_BREAK()
#endif

#ifdef CG_ENABLE_ASSERTS
    #define CG_ASSERT(check, message) { if (!(check)) { CG_LOGGING_ERROR(message) CG_DEBUG_BREAK() }}
#else
    #define CG_ASSERT(...)
#endif