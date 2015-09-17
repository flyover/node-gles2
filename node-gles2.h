/**
 * Copyright (c) Flyover Games, LLC.  All rights reserved. 
 *  
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated 
 * documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights 
 * to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to 
 * whom the Software is furnished to do so, subject to the 
 * following conditions: 
 *  
 * The above copyright notice and this permission notice shall 
 * be included in all copies or substantial portions of the 
 * Software. 
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 */

#ifndef NODE_GLES2_H
#define NODE_GLES2_H

#include <nan.h>

#if defined(__ANDROID__)
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#define HAVE_OPENGLES2
#elif defined(__APPLE__)
	#include "TargetConditionals.h"
	#if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR)
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
		#define HAVE_OPENGLES2
	#elif TARGET_OS_MAC
		#include "mac/bridge-gl-mac.h"
		#define HAVE_OPENGLES2
	#else
		#error "unsupported Apple platform"
	#endif
#elif defined(_WIN32)
	#include "bridge-gl-win.h"
	#define HAVE_OPENGLES2
#else
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#define HAVE_OPENGLES2
#endif

namespace node_gles2 {

NAN_MODULE_INIT(init);

} // namespace node_gles2

#endif // NODE_GLES2_H
