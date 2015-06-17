#ifndef BRIDGE_GL_WIN_H
#define BRIDGE_GL_WIN_H

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include <GL/gl.h>
#include <GL/glext.h>

#define MACRO(_PFN, _RET, _NAME, _ARGS, _CALL, _ERR) \
	extern "C" _RET _NAME _ARGS;

#include "bridge-gl-win.inl"

#undef MACRO

#endif // BRIDGE_GL_WIN_H

