#include "bridge-gl-win.h"

#define MACRO(_PFN, _RET, _NAME, _ARGS, _CALL, _ERR) \
	extern "C" _RET _NAME _ARGS \
	{ \
		static _PFN pfn = 0; \
		if (!pfn) { pfn = (_PFN) wglGetProcAddress(#_NAME); } \
		if (pfn) { _CALL; } else { _ERR; } \
	}

#include "bridge-gl-win.inl"

#undef MACRO

