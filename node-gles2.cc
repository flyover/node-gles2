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

#include "node-gles2.h"

#include <v8.h>
#include <node.h>

#include <stdlib.h> // malloc, free
#include <string.h> // strdup

#ifndef strdup
#define strdup(str) strcpy((char*)malloc(strlen(str)+1),str)
#endif

#if defined(__ANDROID__)
#include <android/log.h>
#define printf(...) __android_log_print(ANDROID_LOG_INFO, "printf", __VA_ARGS__)
#endif

// macros for modules

#define MODULE_CONSTANT(target, constant) \
	(target)->ForceSet(NanNew<v8::String>(#constant), NanNew(constant), static_cast<v8::PropertyAttribute>(v8::ReadOnly|v8::DontDelete))

#define MODULE_CONSTANT_VALUE(target, constant, value) \
	(target)->ForceSet(NanNew<v8::String>(#constant), NanNew(value), static_cast<v8::PropertyAttribute>(v8::ReadOnly|v8::DontDelete))

#define MODULE_CONSTANT_NUMBER(target, constant) \
	(target)->ForceSet(NanNew<v8::String>(#constant), NanNew<v8::Number>(constant), static_cast<v8::PropertyAttribute>(v8::ReadOnly|v8::DontDelete))

#define MODULE_CONSTANT_STRING(target, constant) \
	(target)->ForceSet(NanNew<v8::String>(#constant), NanNew<v8::String>(constant), static_cast<v8::PropertyAttribute>(v8::ReadOnly|v8::DontDelete))

#define MODULE_EXPORT_APPLY(target, name) NODE_SET_METHOD(target, #name, _native_##name)
#define MODULE_EXPORT_DECLARE(name) static NAN_METHOD(_native_##name);
#define MODULE_EXPORT_IMPLEMENT(name) static NAN_METHOD(_native_##name)
#define MODULE_EXPORT_IMPLEMENT_TODO(name) static NAN_METHOD(_native_##name) { return NanThrowError(NanNew<v8::String>("not implemented: " #name)); }

#define ARG_INT32(i)	args[i]->Int32Value()
#define ARG_UINT32(i)	args[i]->Uint32Value()
#define ARG_BOOLEAN(i)	args[i]->BooleanValue()
#define ARG_NUMBER(i)	args[i]->NumberValue()
#define ARG_STRING(i)	v8::Handle<v8::String>::Cast(args[i])
#define ARG_OBJECT(i)	v8::Handle<v8::Object>::Cast(args[i])
#define ARG_ARRAY(i)	v8::Handle<v8::Array>::Cast(args[i])

void* GetTypedArray(v8::Handle<v8::Object> object, int length = 0)
{
	if (object->IsNull())
	{
		return NULL;
	}
	#if NODE_VERSION_AT_LEAST(0, 12, 0)
	object->Get(NanNew<v8::String>("buffer"));
	#endif
	assert(object->HasIndexedPropertiesInExternalArrayData());
	assert((length == 0) || (length <= object->GetIndexedPropertiesExternalArrayDataLength()));
	return object->GetIndexedPropertiesExternalArrayData();
}

template <v8::ExternalArrayType TYPE>
void* GetTypedArray(v8::Handle<v8::Object> object, int length = 0)
{
	if (object->IsNull())
	{
		return NULL;
	}
	#if NODE_VERSION_AT_LEAST(0, 12, 0)
	object->Get(NanNew<v8::String>("buffer"));
	#endif
	assert(object->HasIndexedPropertiesInExternalArrayData());
	assert((length == 0) || (length <= object->GetIndexedPropertiesExternalArrayDataLength()));
	assert(TYPE == object->GetIndexedPropertiesExternalArrayDataType());
	return object->GetIndexedPropertiesExternalArrayData();
}

using namespace v8;

namespace node_gles2 {

#ifdef HAVE_OPENGLES2

//void glActiveTexture(GLenum texture);
MODULE_EXPORT_IMPLEMENT(glActiveTexture)
{
	NanScope();
	GLenum texture = (GLenum) ARG_UINT32(0);
	::glActiveTexture(texture);
	NanReturnUndefined();
}

//void glAttachShader(GLuint program, GLuint shader);
MODULE_EXPORT_IMPLEMENT(glAttachShader)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	GLuint shader = ARG_UINT32(1);
	::glAttachShader(program, shader);
	NanReturnUndefined();
}

//void glBindAttribLocation(GLuint program, GLuint index, const GLchar* name);
MODULE_EXPORT_IMPLEMENT(glBindAttribLocation)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	GLuint index = ARG_UINT32(1);
	Handle<String> name = ARG_STRING(2);
	::glBindAttribLocation(program, index, *String::Utf8Value(name));
	NanReturnUndefined();
}

//void glBindBuffer(GLenum target, GLuint buffer);
MODULE_EXPORT_IMPLEMENT(glBindBuffer)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLuint buffer = ARG_UINT32(1);
	::glBindBuffer(target, buffer);
	NanReturnUndefined();
}

//void glBindFramebuffer(GLenum target, GLuint framebuffer);
MODULE_EXPORT_IMPLEMENT(glBindFramebuffer)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLuint framebuffer = ARG_UINT32(1);
	::glBindFramebuffer(target, framebuffer);
	NanReturnUndefined();
}

//void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
MODULE_EXPORT_IMPLEMENT(glBindRenderbuffer)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLuint renderbuffer = ARG_UINT32(1);
	::glBindRenderbuffer(target, renderbuffer);
	NanReturnUndefined();
}

//void glBindTexture(GLenum target, GLuint texture);
MODULE_EXPORT_IMPLEMENT(glBindTexture)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLuint texture = ARG_UINT32(1);
	::glBindTexture(target, texture);
	NanReturnUndefined();
}

//void glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
MODULE_EXPORT_IMPLEMENT(glBlendColor)
{
	NanScope();
	GLclampf red = (GLclampf) ARG_NUMBER(0);
	GLclampf green = (GLclampf) ARG_NUMBER(1);
	GLclampf blue = (GLclampf) ARG_NUMBER(2);
	GLclampf alpha = (GLclampf) ARG_NUMBER(3);
	::glBlendColor(red, green, blue, alpha);
	NanReturnUndefined();
}

//void glBlendEquation(GLenum mode);
MODULE_EXPORT_IMPLEMENT(glBlendEquation)
{
	NanScope();
	GLenum mode = ARG_UINT32(0);
	::glBlendEquation(mode);
	NanReturnUndefined();
}

//void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
MODULE_EXPORT_IMPLEMENT(glBlendEquationSeparate)
{
	NanScope();
	GLenum modeRGB = ARG_UINT32(0);
	GLenum modeAlpha = ARG_UINT32(1);
	::glBlendEquationSeparate(modeRGB, modeAlpha);
	NanReturnUndefined();
}

//void glBlendFunc(GLenum sfactor, GLenum dfactor);
MODULE_EXPORT_IMPLEMENT(glBlendFunc)
{
	NanScope();
	GLenum sfactor = ARG_UINT32(0);
	GLenum dfactor = ARG_UINT32(1);
	::glBlendFunc(sfactor, dfactor);
	NanReturnUndefined();
}

//void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
MODULE_EXPORT_IMPLEMENT(glBlendFuncSeparate)
{
	NanScope();
	GLenum srcRGB = ARG_UINT32(0);
	GLenum dstRGB = ARG_UINT32(1);
	GLenum srcAlpha = ARG_UINT32(2);
	GLenum dstAlpha = ARG_UINT32(3);
	::glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
	NanReturnUndefined();
}

//void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
MODULE_EXPORT_IMPLEMENT(glBufferData)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLsizeiptr size = ARG_INT32(1);
	const GLvoid* data = GetTypedArray(ARG_OBJECT(2)); // TODO: length
	GLenum usage = ARG_UINT32(3);
	::glBufferData(target, size, data, usage);
	NanReturnUndefined();
}

//void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
MODULE_EXPORT_IMPLEMENT(glBufferSubData)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLintptr offset = ARG_INT32(1);
	GLsizeiptr size = ARG_INT32(2);
	const GLvoid* data = GetTypedArray(ARG_OBJECT(3)); // TODO: length
	::glBufferSubData(target, offset, size, data);
	NanReturnUndefined();
}

//GLenum glCheckFramebufferStatus(GLenum target);
MODULE_EXPORT_IMPLEMENT(glCheckFramebufferStatus)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum status = ::glCheckFramebufferStatus(target);
	NanReturnValue(NanNew(status));
}

//void glClear(GLbitfield mask);
MODULE_EXPORT_IMPLEMENT(glClear)
{
	NanScope();
	GLbitfield mask = ARG_UINT32(0);
	::glClear(mask);
	NanReturnUndefined();
}

//void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
MODULE_EXPORT_IMPLEMENT(glClearColor)
{
	NanScope();
	GLclampf red = (GLclampf) ARG_NUMBER(0);
	GLclampf green = (GLclampf) ARG_NUMBER(1);
	GLclampf blue = (GLclampf) ARG_NUMBER(2);
	GLclampf alpha = (GLclampf) ARG_NUMBER(3);
	::glClearColor(red, green, blue, alpha);
	NanReturnUndefined();
}

//void glClearDepthf(GLclampf depth);
MODULE_EXPORT_IMPLEMENT(glClearDepthf)
{
	NanScope();
	GLclampf depth = (GLclampf) ARG_NUMBER(0);
	::glClearDepthf(depth);
	NanReturnUndefined();
}

//void glClearStencil(GLint s);
MODULE_EXPORT_IMPLEMENT(glClearStencil)
{
	NanScope();
	GLint s = ARG_INT32(0);
	::glClearStencil(s);
	NanReturnUndefined();
}

//void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
MODULE_EXPORT_IMPLEMENT(glColorMask)
{
	NanScope();
	GLboolean red = ARG_BOOLEAN(0);
	GLboolean green = ARG_BOOLEAN(1);
	GLboolean blue = ARG_BOOLEAN(2);
	GLboolean alpha = ARG_BOOLEAN(3);
	::glColorMask(red, green, blue, alpha);
	NanReturnUndefined();
}

//void glCompileShader(GLuint shader);
MODULE_EXPORT_IMPLEMENT(glCompileShader)
{
	NanScope();
	GLuint shader = ARG_UINT32(0);
	::glCompileShader(shader);
	NanReturnUndefined();
}

//void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data);
MODULE_EXPORT_IMPLEMENT(glCompressedTexImage2D)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLint level = ARG_INT32(1);
	GLenum internalformat = ARG_UINT32(2);
	GLsizei width = ARG_UINT32(3);
	GLsizei height = ARG_UINT32(4);
	GLint border = ARG_INT32(5);
	GLsizei imageSize = ARG_UINT32(6);
	const GLvoid* data = GetTypedArray(ARG_OBJECT(7), imageSize);
	::glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
	NanReturnUndefined();
}

//void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data);
MODULE_EXPORT_IMPLEMENT(glCompressedTexSubImage2D)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLint level = ARG_INT32(1);
	GLint xoffset = ARG_INT32(2);
	GLint yoffset = ARG_INT32(3);
	GLsizei width = ARG_UINT32(4);
	GLsizei height = ARG_UINT32(5);
	GLenum format = ARG_UINT32(6);
	GLsizei imageSize = ARG_UINT32(7);
	const GLvoid* data = GetTypedArray(ARG_OBJECT(8), imageSize);
	::glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
	NanReturnUndefined();
}

//void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
MODULE_EXPORT_IMPLEMENT(glCopyTexImage2D)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLint level = ARG_INT32(1);
	GLenum internalformat = ARG_UINT32(2);
	GLint x = ARG_INT32(3);
	GLint y = ARG_INT32(4);
	GLsizei width = ARG_UINT32(5);
	GLsizei height = ARG_UINT32(6);
	GLint border = ARG_INT32(7);
	::glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
	NanReturnUndefined();
}

//void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
MODULE_EXPORT_IMPLEMENT(glCopyTexSubImage2D)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLint level = ARG_INT32(1);
	GLint xoffset = ARG_INT32(2);
	GLint yoffset = ARG_INT32(3);
	GLint x = ARG_INT32(4);
	GLint y = ARG_INT32(5);
	GLsizei width = ARG_UINT32(6);
	GLsizei height = ARG_UINT32(7);
	::glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
	NanReturnUndefined();
}

//GLuint glCreateProgram(void);
MODULE_EXPORT_IMPLEMENT(glCreateProgram)
{
	NanScope();
	GLuint program = ::glCreateProgram();
	NanReturnValue(NanNew(program));
}

//GLuint glCreateShader(GLenum type);
MODULE_EXPORT_IMPLEMENT(glCreateShader)
{
	NanScope();
	GLenum type = ARG_UINT32(0);
	GLuint shader = ::glCreateShader(type);
	NanReturnValue(NanNew(shader));
}

//void glCullFace(GLenum mode);
MODULE_EXPORT_IMPLEMENT(glCullFace)
{
	NanScope();
	GLenum mode = ARG_UINT32(0);
	::glCullFace(mode);
	NanReturnUndefined();
}

//void glDeleteBuffers(GLsizei n, const GLuint* buffers);
MODULE_EXPORT_IMPLEMENT(glDeleteBuffers)
{
	NanScope();
	GLsizei n = ARG_UINT32(0);
	GLuint* buffers = (GLuint*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(1), n);
	::glDeleteBuffers(n, buffers);
	NanReturnUndefined();
}

//void glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers);
MODULE_EXPORT_IMPLEMENT(glDeleteFramebuffers)
{
	NanScope();
	GLsizei n = ARG_UINT32(0);
	GLuint* framebuffers = (GLuint*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(1), n);
	::glDeleteFramebuffers(n, framebuffers);
	NanReturnUndefined();
}

//void glDeleteProgram(GLuint program);
MODULE_EXPORT_IMPLEMENT(glDeleteProgram)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	::glDeleteProgram(program);
	NanReturnUndefined();
}

//void glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers);
MODULE_EXPORT_IMPLEMENT(glDeleteRenderbuffers)
{
	NanScope();
	GLsizei n = ARG_UINT32(0);
	GLuint* renderbuffers = (GLuint*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(1), n);
	::glDeleteRenderbuffers(n, renderbuffers);
	NanReturnUndefined();
}

//void glDeleteShader(GLuint shader);
MODULE_EXPORT_IMPLEMENT(glDeleteShader)
{
	NanScope();
	GLuint shader = ARG_UINT32(0);
	::glDeleteShader(shader);
	NanReturnUndefined();
}

//void glDeleteTextures(GLsizei n, const GLuint* textures);
MODULE_EXPORT_IMPLEMENT(glDeleteTextures)
{
	NanScope();
	GLsizei n = ARG_UINT32(0);
	GLuint* textures = (GLuint*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(1), n);
	::glDeleteTextures(n, textures);
	NanReturnUndefined();
}

//void glDepthFunc(GLenum func);
MODULE_EXPORT_IMPLEMENT(glDepthFunc)
{
	NanScope();
	GLenum func = ARG_UINT32(0);
	::glDepthFunc(func);
	NanReturnUndefined();
}

//void glDepthMask(GLboolean flag);
MODULE_EXPORT_IMPLEMENT(glDepthMask)
{
	NanScope();
	GLboolean flag = ARG_BOOLEAN(0);
	::glDepthMask(flag);
	NanReturnUndefined();
}

//void glDepthRangef(GLclampf zNear, GLclampf zFar);
MODULE_EXPORT_IMPLEMENT(glDepthRangef)
{
	NanScope();
	GLclampf zNear = (GLclampf) ARG_NUMBER(0);
	GLclampf zFar = (GLclampf) ARG_NUMBER(1);
	::glDepthRangef(zNear, zFar);
	NanReturnUndefined();
}

//void glDetachShader(GLuint program, GLuint shader);
MODULE_EXPORT_IMPLEMENT(glDetachShader)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	GLuint shader = ARG_UINT32(1);
	::glDetachShader(program, shader);
	NanReturnUndefined();
}

//void glDisable(GLenum cap);
MODULE_EXPORT_IMPLEMENT(glDisable)
{
	NanScope();
	GLenum cap = ARG_UINT32(0);
	::glDisable(cap);
	NanReturnUndefined();
}

//void glDisableVertexAttribArray(GLuint index);
MODULE_EXPORT_IMPLEMENT(glDisableVertexAttribArray)
{
	NanScope();
	GLuint index = ARG_UINT32(0);
	::glDisableVertexAttribArray(index);
	NanReturnUndefined();
}

//void glDrawArrays(GLenum mode, GLint first, GLsizei count);
MODULE_EXPORT_IMPLEMENT(glDrawArrays)
{
	NanScope();
	GLenum mode = ARG_UINT32(0);
	GLint first = ARG_INT32(1);
	GLsizei count = ARG_UINT32(2);
	::glDrawArrays(mode, first, count);
	NanReturnUndefined();
}

//void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
MODULE_EXPORT_IMPLEMENT(glDrawElements)
{
	NanScope();
	GLenum mode = ARG_UINT32(0);
	GLsizei count = ARG_UINT32(1);
	GLenum type = ARG_UINT32(2);
	const GLvoid* indices = (const GLvoid*) args[3]->IntegerValue(); // TODO
	::glDrawElements(mode, count, type, indices);
	NanReturnUndefined();
}

//void glEnable(GLenum cap);
MODULE_EXPORT_IMPLEMENT(glEnable)
{
	NanScope();
	GLenum cap = ARG_UINT32(0);
	::glEnable(cap);
	NanReturnUndefined();
}

//void glEnableVertexAttribArray(GLuint index);
MODULE_EXPORT_IMPLEMENT(glEnableVertexAttribArray)
{
	NanScope();
	GLuint index = ARG_UINT32(0);
	::glEnableVertexAttribArray(index);
	NanReturnUndefined();
}

//void glFinish(void);
MODULE_EXPORT_IMPLEMENT(glFinish)
{
	NanScope();
	::glFinish();
	NanReturnUndefined();
}

//void glFlush(void);
MODULE_EXPORT_IMPLEMENT(glFlush)
{
	NanScope();
	::glFlush();
	NanReturnUndefined();
}

//void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
MODULE_EXPORT_IMPLEMENT(glFramebufferRenderbuffer)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum attachment = ARG_UINT32(1);
	GLenum renderbuffertarget = ARG_UINT32(2);
	GLuint renderbuffer = ARG_UINT32(3);
	::glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
	NanReturnUndefined();
}

//void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
MODULE_EXPORT_IMPLEMENT(glFramebufferTexture2D)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum attachment = ARG_UINT32(1);
	GLenum textarget = ARG_UINT32(2);
	GLuint texture = ARG_UINT32(3);
	GLint level = ARG_INT32(4);
	::glFramebufferTexture2D(target, attachment, textarget, texture, level);
	NanReturnUndefined();
}

//void glFrontFace(GLenum mode);
MODULE_EXPORT_IMPLEMENT(glFrontFace)
{
	NanScope();
	GLenum mode = ARG_UINT32(0);
	::glFrontFace(mode);
	NanReturnUndefined();
}

//void glGenBuffers(GLsizei n, GLuint* buffers);
MODULE_EXPORT_IMPLEMENT(glGenBuffers)
{
	NanScope();
	GLsizei n = ARG_UINT32(0);
	GLuint* buffers = (GLuint*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(1), n);
	::glGenBuffers(n, buffers);
	NanReturnUndefined();
}

//void glGenerateMipmap(GLenum target);
MODULE_EXPORT_IMPLEMENT(glGenerateMipmap)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	::glGenerateMipmap(target);
	NanReturnUndefined();
}

//void glGenFramebuffers(GLsizei n, GLuint* framebuffers);
MODULE_EXPORT_IMPLEMENT(glGenFramebuffers)
{
	NanScope();
	GLsizei n = ARG_UINT32(0);
	GLuint* framebuffers = (GLuint*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(1), n);
	::glGenFramebuffers(n, framebuffers);
	NanReturnUndefined();
}

//void glGenRenderbuffers(GLsizei n, GLuint* renderbuffers);
MODULE_EXPORT_IMPLEMENT(glGenRenderbuffers)
{
	NanScope();
	GLsizei n = ARG_UINT32(0);
	GLuint* renderbuffers = (GLuint*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(1), n);
	::glGenRenderbuffers(n, renderbuffers);
	NanReturnUndefined();
}

//void glGenTextures(GLsizei n, GLuint* textures);
MODULE_EXPORT_IMPLEMENT(glGenTextures)
{
	NanScope();
	GLsizei n = ARG_UINT32(0);
	GLuint* textures = (GLuint*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(1), n);
	::glGenTextures(n, textures);
	NanReturnUndefined();
}

//void glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
MODULE_EXPORT_IMPLEMENT(glGetActiveAttrib)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	GLuint index = ARG_UINT32(1);
	GLsizei bufsize = ARG_UINT32(2);
	GLint* length = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(3), 1);
	GLint* size = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(4), 1);
	GLenum* type = (GLenum*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(5), 1);
	Handle<Array> _name = ARG_ARRAY(6);
	const size_t s_name_count = 64;
	static GLchar s_name[s_name_count];
	GLchar* name = ((size_t) bufsize <= s_name_count)?(s_name):((GLchar*) malloc(bufsize * sizeof(GLchar)));
	::glGetActiveAttrib(program, index, bufsize, length, size, type, name);
	_name->Set(0, NanNew<String>(name));
	if (name != s_name) { free(name); }
	NanReturnUndefined();
}

//void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
MODULE_EXPORT_IMPLEMENT(glGetActiveUniform)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	GLuint index = ARG_UINT32(1);
	GLsizei bufsize = ARG_UINT32(2);
	GLint* length = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(3), 1);
	GLint* size = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(4), 1);
	GLenum* type = (GLenum*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(5), 1);
	Handle<Array> _name = ARG_ARRAY(6);
	const size_t s_name_count = 64;
	static GLchar s_name[s_name_count];
	GLchar* name = ((size_t) bufsize <= s_name_count)?(s_name):((GLchar*) malloc(bufsize * sizeof(GLchar)));
	::glGetActiveUniform(program, index, bufsize, length, size, type, name);
	_name->Set(0, NanNew<String>(name));
	if (name != s_name) { free(name); }
	NanReturnUndefined();
}

//void glGetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
MODULE_EXPORT_IMPLEMENT(glGetAttachedShaders)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	GLsizei maxcount = ARG_UINT32(1);
	GLsizei* count = (GLsizei*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(2), 1);
	GLuint* shaders = (GLuint*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(3), maxcount);
	::glGetAttachedShaders(program, maxcount, count, shaders);
	NanReturnUndefined();
}

//GLint glGetAttribLocation(GLuint program, const GLchar* name);
MODULE_EXPORT_IMPLEMENT(glGetAttribLocation)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	Handle<String> name = ARG_STRING(1);
	GLint location = ::glGetAttribLocation(program, *String::Utf8Value(name));
	NanReturnValue(NanNew<Integer>(location));
}

//void glGetBooleanv(GLenum pname, GLboolean* params);
MODULE_EXPORT_IMPLEMENT(glGetBooleanv)
{
	NanScope();
	GLenum pname = ARG_UINT32(0);
	GLboolean* params = (GLboolean*) GetTypedArray<kExternalUnsignedByteArray>(ARG_OBJECT(1)); // TODO: length
	::glGetBooleanv(pname, params);
	NanReturnUndefined();
}

//void glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params);
MODULE_EXPORT_IMPLEMENT(glGetBufferParameteriv)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	GLint* params = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(2)); // TODO: length
	::glGetBufferParameteriv(target, pname, params);
	NanReturnUndefined();
}

//GLenum glGetError(void);
MODULE_EXPORT_IMPLEMENT(glGetError)
{
	NanScope();
	GLenum gl_error = ::glGetError();
	NanReturnValue(NanNew(gl_error));
}

//void glGetFloatv(GLenum pname, GLfloat* params);
MODULE_EXPORT_IMPLEMENT(glGetFloatv)
{
	NanScope();
	GLenum pname = ARG_UINT32(0);
	GLfloat* params = (GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(1)); // TODO: length
	::glGetFloatv(pname, params);
	NanReturnUndefined();
}

//void glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params);
MODULE_EXPORT_IMPLEMENT(glGetFramebufferAttachmentParameteriv)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum attachment = ARG_UINT32(1);
	GLenum pname = ARG_UINT32(2);
	GLint* params = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(3)); // TODO: length
	::glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
	NanReturnUndefined();
}

//void glGetIntegerv(GLenum pname, GLint* params);
MODULE_EXPORT_IMPLEMENT(glGetIntegerv)
{
	NanScope();
	GLenum pname = ARG_UINT32(0);
	GLint* params = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(1)); // TODO: length
	::glGetIntegerv(pname, params);

	#if defined(GL_MAX_VARYING_COMPONENTS)
	/* 
		Additions to Chapter 2 of the OpenGL 4.0 Specification (OpenGL Operation)

		Section 2.14.7, p. 118 (Varying Variables)

		Add after the definition of MAX_VARYING_COMPONENTS:

		The implementation-dependent constant MAX_VARYING_VECTORS has a
		value equal to the value of MAX_VARYING_COMPONENTS divided by four.
	*/ 
	if ((pname == GL_MAX_VARYING_VECTORS) && (::glGetError() != GL_NO_ERROR))
	{
		params[0] = 0;
		::glGetIntegerv(GL_MAX_VARYING_COMPONENTS, params);
		params[0] /= 4;
	}
	#endif

	#if defined(GL_MAX_VERTEX_UNIFORM_COMPONENTS)
	/* 
		Additions to Chapter 2 of the OpenGL 4.0 Specification (OpenGL Operation) 

		Section 2.14.4, p. 97 (Uniform Variables)

		Add after the definition of MAX_VERTEX_UNIFORM_COMPONENTS:

		The implementation-dependent constant MAX_VERTEX_UNIFORM_VECTORS has
		a value equal to the value of MAX_VERTEX_UNIFORM_COMPONENTS divided
		by four.
	*/
	if ((pname == GL_MAX_VERTEX_UNIFORM_VECTORS) && (::glGetError() != GL_NO_ERROR))
	{
		params[0] = 0;
		::glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, params);
		params[0] /= 4;
	}
	#endif

	#if defined(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS)
	/* 
		Additions to Chapter 3 of the OpenGL 4.0 Specification (Rasterization) 

		Section 3.12.1, p. 323 (Shader Variables)

		Add after the definition of MAX_FRAGMENT_UNIFORM_COMPONENTS:

		The implementation-dependent constant MAX_FRAGMENT_UNIFORM_VECTORS
		has a value equal to the value of MAX_FRAGMENT_UNIFORM_COMPONENTS
		divided by four.
	*/
	if ((pname == GL_MAX_FRAGMENT_UNIFORM_VECTORS) && (::glGetError() != GL_NO_ERROR))
	{
		params[0] = 0;
		::glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, params);
		params[0] /= 4;
	}
	#endif

	NanReturnUndefined();
}

//void glGetProgramiv(GLuint program, GLenum pname, GLint* params);
MODULE_EXPORT_IMPLEMENT(glGetProgramiv)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	GLint* params = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(2)); // TODO: length
	::glGetProgramiv(program, pname, params);
	NanReturnUndefined();
}

//void glGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog);
MODULE_EXPORT_IMPLEMENT(glGetProgramInfoLog)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	GLsizei bufsize = ARG_UINT32(1);
	GLsizei* length = (GLsizei*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(2), 1);
	Handle<Array> _infolog = ARG_ARRAY(3);
	const size_t s_infolog_count = 64;
	static GLchar s_infolog[s_infolog_count];
	GLchar* infolog = ((size_t) bufsize <= s_infolog_count)?(s_infolog):((GLchar*) malloc(bufsize * sizeof(GLchar)));
	::glGetProgramInfoLog(program, bufsize, length, infolog);
	_infolog->Set(0, NanNew<String>(infolog));
	if (infolog != s_infolog) { free(infolog); }
	NanReturnUndefined();
}

//void glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params);
MODULE_EXPORT_IMPLEMENT(glGetRenderbufferParameteriv)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	GLint* params = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(2)); // TODO: length
	::glGetRenderbufferParameteriv(target, pname, params);
	NanReturnUndefined();
}

//void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
MODULE_EXPORT_IMPLEMENT(glGetShaderiv)
{
	NanScope();
	GLuint shader = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	GLint* params = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(2)); // TODO: length
	::glGetShaderiv(shader, pname, params);
	NanReturnUndefined();
}

//void glGetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog);
MODULE_EXPORT_IMPLEMENT(glGetShaderInfoLog)
{
	NanScope();
	GLuint shader = ARG_UINT32(0);
	GLsizei bufsize = ARG_UINT32(1);
	GLsizei* length = (GLsizei*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(2), 1);
	Handle<Array> _infolog = ARG_ARRAY(3);
	const size_t s_infolog_count = 64;
	static GLchar s_infolog[s_infolog_count];
	GLchar* infolog = ((size_t) bufsize <= s_infolog_count)?(s_infolog):((GLchar*) malloc(bufsize * sizeof(GLchar)));
	::glGetShaderInfoLog(shader, bufsize, length, infolog);
	_infolog->Set(0, NanNew<String>(infolog));
	if (infolog != s_infolog) { free(infolog); }
	NanReturnUndefined();
}

//void glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
MODULE_EXPORT_IMPLEMENT(glGetShaderPrecisionFormat)
{
	NanScope();
	GLenum shadertype = ARG_UINT32(0);
	GLenum precisiontype = ARG_UINT32(1);
	Handle<Array> _range = ARG_ARRAY(2);
	Handle<Array> _precision = ARG_ARRAY(3);
	GLsizei range[2] = { static_cast<GLsizei>(_range->Get(0)->Uint32Value()), static_cast<GLsizei>(_range->Get(1)->Uint32Value()) };
	GLint precision = _precision->Get(0)->Int32Value();
	::glGetShaderPrecisionFormat(shadertype, precisiontype, range, &precision);
	_range->Set(0, NanNew(range[0]));
	_range->Set(1, NanNew(range[1]));
	_precision->Set(0, NanNew<Integer>(precision));
	NanReturnUndefined();
}

//void glGetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source);
MODULE_EXPORT_IMPLEMENT(glGetShaderSource)
{
	NanScope();
	GLuint shader = ARG_UINT32(0);
	GLsizei bufsize = ARG_UINT32(1);
	GLsizei* length = (GLsizei*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(2), 1);
	Handle<Array> _source = ARG_ARRAY(3);
	const size_t s_source_count = 256;
	static GLchar s_source[s_source_count];
	GLchar* source = ((size_t) bufsize <= s_source_count)?(s_source):((GLchar*) malloc(bufsize * sizeof(GLchar)));
	::glGetShaderSource(shader, bufsize, length, source);
	_source->Set(0, NanNew<String>(source));
	if (source != s_source) { free(source); }
	NanReturnUndefined();
}

//const GLubyte* glGetString(GLenum name);
MODULE_EXPORT_IMPLEMENT(glGetString)
{
	NanScope();
	GLenum name = ARG_UINT32(0);
	const GLubyte* gl_string = ::glGetString(name);
	if (gl_string != 0)
	{
		NanReturnValue(NanNew<String>((const char *) gl_string));
	}
	NanReturnUndefined();
}

//void glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params);
MODULE_EXPORT_IMPLEMENT(glGetTexParameterfv)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	GLfloat* params = (GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(2)); // TODO: length
	::glGetTexParameterfv(target, pname, params);
	NanReturnUndefined();
}

//void glGetTexParameteriv(GLenum target, GLenum pname, GLint* params);
MODULE_EXPORT_IMPLEMENT(glGetTexParameteriv)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	GLint* params = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(2)); // TODO: length
	::glGetTexParameteriv(target, pname, params);
	NanReturnUndefined();
}

//void glGetUniformfv(GLuint program, GLint location, GLfloat* params);
MODULE_EXPORT_IMPLEMENT(glGetUniformfv)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	GLint location = ARG_INT32(1);
	GLfloat* params = (GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(2)); // TODO: length
	::glGetUniformfv(program, location, params);
	NanReturnUndefined();
}

//void glGetUniformiv(GLuint program, GLint location, GLint* params);
MODULE_EXPORT_IMPLEMENT(glGetUniformiv)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	GLint location = ARG_INT32(1);
	GLint* params = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(2)); // TODO: length
	::glGetUniformiv(program, location, params);
	NanReturnUndefined();
}

//GLint glGetUniformLocation(GLuint program, const GLchar* name);
MODULE_EXPORT_IMPLEMENT(glGetUniformLocation)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	Handle<String> name = ARG_STRING(1);
	GLint location = ::glGetUniformLocation(program, *String::Utf8Value(name));
	NanReturnValue(NanNew<Integer>(location));
}

//void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params);
MODULE_EXPORT_IMPLEMENT(glGetVertexAttribfv)
{
	NanScope();
	GLuint index = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	GLfloat* params = (GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(2)); // TODO: length
	::glGetVertexAttribfv(index, pname, params);
	NanReturnUndefined();
}

//void glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params);
MODULE_EXPORT_IMPLEMENT(glGetVertexAttribiv)
{
	NanScope();
	GLuint index = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	GLint* params = (GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(2)); // TODO: length
	::glGetVertexAttribiv(index, pname, params);
	NanReturnUndefined();
}

//void glGetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid** pointer);
MODULE_EXPORT_IMPLEMENT(glGetVertexAttribPointerv)
{
	NanScope();
	GLuint index = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	Handle<Array> _pointer = ARG_ARRAY(2);
	GLuint offset = 0;
	GLvoid* pointer = (GLvoid*) &offset;
	::glGetVertexAttribPointerv(index, pname, &pointer);
	_pointer->Set(0, NanNew(offset));
	NanReturnUndefined();
}

//void glHint(GLenum target, GLenum mode);
MODULE_EXPORT_IMPLEMENT(glHint)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum mode = ARG_UINT32(1);
	::glHint(target, mode);
	NanReturnUndefined();
}

//GLboolean glIsBuffer(GLuint buffer);
MODULE_EXPORT_IMPLEMENT(glIsBuffer)
{
	NanScope();
	GLuint buffer = ARG_UINT32(0);
	GLboolean b = ::glIsBuffer(buffer);
	NanReturnValue(NanNew<Boolean>(b != GL_FALSE));
}

//GLboolean glIsEnabled(GLenum cap);
MODULE_EXPORT_IMPLEMENT(glIsEnabled)
{
	NanScope();
	GLenum cap = ARG_UINT32(0);
	GLboolean b = ::glIsEnabled(cap);
	NanReturnValue(NanNew<Boolean>(b != GL_FALSE));
}

//GLboolean glIsFramebuffer(GLuint framebuffer);
MODULE_EXPORT_IMPLEMENT(glIsFramebuffer)
{
	NanScope();
	GLuint framebuffer = ARG_UINT32(0);
	GLboolean b = ::glIsFramebuffer(framebuffer);
	NanReturnValue(NanNew<Boolean>(b != GL_FALSE));
}

//GLboolean glIsProgram(GLuint program);
MODULE_EXPORT_IMPLEMENT(glIsProgram)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	GLboolean b = ::glIsProgram(program);
	NanReturnValue(NanNew<Boolean>(b != GL_FALSE));
}

//GLboolean glIsRenderbuffer(GLuint renderbuffer);
MODULE_EXPORT_IMPLEMENT(glIsRenderbuffer)
{
	NanScope();
	GLuint renderbuffer = ARG_UINT32(0);
	GLboolean b = ::glIsRenderbuffer(renderbuffer);
	NanReturnValue(NanNew<Boolean>(b != GL_FALSE));
}

//GLboolean glIsShader(GLuint shader);
MODULE_EXPORT_IMPLEMENT(glIsShader)
{
	NanScope();
	GLuint shader = ARG_UINT32(0);
	GLboolean b = ::glIsShader(shader);
	NanReturnValue(NanNew<Boolean>(b != GL_FALSE));
}

//GLboolean glIsTexture(GLuint texture);
MODULE_EXPORT_IMPLEMENT(glIsTexture)
{
	NanScope();
	GLuint texture = ARG_UINT32(0);
	GLboolean b = ::glIsTexture(texture);
	NanReturnValue(NanNew<Boolean>(b != GL_FALSE));
}

//void glLineWidth(GLfloat width);
MODULE_EXPORT_IMPLEMENT(glLineWidth)
{
	NanScope();
	GLfloat width = (GLfloat) ARG_NUMBER(0);
	::glLineWidth(width);
	NanReturnUndefined();
}

//void glLinkProgram(GLuint program);
MODULE_EXPORT_IMPLEMENT(glLinkProgram)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	::glLinkProgram(program);
	NanReturnUndefined();
}

//void glPixelStorei(GLenum pname, GLint param);
MODULE_EXPORT_IMPLEMENT(glPixelStorei)
{
	NanScope();
	GLenum pname = ARG_UINT32(0);
	GLint param = ARG_INT32(1);
	::glPixelStorei(pname, param);
	NanReturnUndefined();
}

//void glPolygonOffset(GLfloat factor, GLfloat units);
MODULE_EXPORT_IMPLEMENT(glPolygonOffset)
{
	NanScope();
	GLfloat factor = (GLfloat) ARG_NUMBER(0);
	GLfloat units = (GLfloat) ARG_NUMBER(1);
	::glPolygonOffset(factor, units);
	NanReturnUndefined();
}

//void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels);
MODULE_EXPORT_IMPLEMENT(glReadPixels)
{
	NanScope();
	GLint x = ARG_INT32(0);
	GLint y = ARG_INT32(1);
	GLsizei width = ARG_UINT32(2);
	GLsizei height = ARG_UINT32(3);
	GLenum format = ARG_UINT32(4);
	GLenum type = ARG_UINT32(5);
	GLvoid* pixels = GetTypedArray(ARG_OBJECT(6)); // TODO: length
	::glReadPixels(x, y, width, height, format, type, pixels);
	NanReturnUndefined();
}

//void glReleaseShaderCompiler(void);
MODULE_EXPORT_IMPLEMENT(glReleaseShaderCompiler)
{
	NanScope();
	::glReleaseShaderCompiler();
	NanReturnUndefined();
}

//void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
MODULE_EXPORT_IMPLEMENT(glRenderbufferStorage)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum internalformat = ARG_UINT32(1);
	GLsizei width = ARG_UINT32(2);
	GLsizei height = ARG_UINT32(3);
	::glRenderbufferStorage(target, internalformat, width, height);
	NanReturnUndefined();
}

//void glSampleCoverage(GLclampf value, GLboolean invert);
MODULE_EXPORT_IMPLEMENT(glSampleCoverage)
{
	NanScope();
	GLclampf value = (GLclampf) ARG_NUMBER(0);
	GLboolean invert = ARG_BOOLEAN(1);
	::glSampleCoverage(value, invert);
	NanReturnUndefined();
}

//void glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
MODULE_EXPORT_IMPLEMENT(glScissor)
{
	NanScope();
	GLint x = ARG_INT32(0);
	GLint y = ARG_INT32(1);
	GLsizei width = ARG_UINT32(2);
	GLsizei height = ARG_UINT32(3);
	::glScissor(x, y, width, height);
	NanReturnUndefined();
}

//void glShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length);
MODULE_EXPORT_IMPLEMENT(glShaderBinary)
{
	NanScope();
	GLsizei n = ARG_UINT32(0);
	const GLuint* shaders = (const GLuint*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(1), n);
	GLenum binaryformat = ARG_UINT32(2);
	GLsizei length = ARG_UINT32(4);
	const GLvoid* binary = (const GLvoid*) GetTypedArray<kExternalUnsignedIntArray>(ARG_OBJECT(3), length);
	::glShaderBinary(n, shaders, binaryformat, binary, length);
	NanReturnUndefined();
}

//void glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
MODULE_EXPORT_IMPLEMENT(glShaderSource)
{
	NanScope();
	GLuint shader = ARG_UINT32(0);
	GLsizei count = ARG_UINT32(1);
	Handle<Array> _string = ARG_ARRAY(2);
	Handle<Array> _length = ARG_ARRAY(3);
	GLchar** string = (GLchar**) malloc(count * sizeof(GLchar*));
	GLint* length = (GLint*) malloc(count * sizeof(GLint));
	for (int i = 0; i < count; ++i)
	{
		Handle<String> str = Handle<String>::Cast(_string->Get(i));
		Handle<Integer> len = Handle<Integer>::Cast(_length->Get(i));
		string[i] = strdup(*String::Utf8Value(str));
		length[i] = len->Int32Value();
	}
	::glShaderSource(shader, count, (const GLchar**) string, length);
	for (int i = 0; i < count; ++i)
	{
		free(string[i]); // strdup
	}
	free(string);
	free(length);
	NanReturnUndefined();
}

//void glStencilFunc(GLenum func, GLint ref, GLuint mask);
MODULE_EXPORT_IMPLEMENT(glStencilFunc)
{
	NanScope();
	GLenum func = ARG_UINT32(0);
	GLint ref = ARG_INT32(1);
	GLuint mask = ARG_UINT32(2);
	::glStencilFunc(func, ref, mask);
	NanReturnUndefined();
}

//void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask);
MODULE_EXPORT_IMPLEMENT(glStencilFuncSeparate)
{
	NanScope();
	GLenum face = ARG_UINT32(0);
	GLenum func = ARG_UINT32(1);
	GLint ref = ARG_INT32(2);
	GLuint mask = ARG_UINT32(3);
	::glStencilFuncSeparate(face, func, ref, mask);
	NanReturnUndefined();
}

//void glStencilMask(GLuint mask);
MODULE_EXPORT_IMPLEMENT(glStencilMask)
{
	NanScope();
	GLuint mask = ARG_UINT32(0);
	::glStencilMask(mask);
	NanReturnUndefined();
}

//void glStencilMaskSeparate(GLenum face, GLuint mask);
MODULE_EXPORT_IMPLEMENT(glStencilMaskSeparate)
{
	NanScope();
	GLenum face = ARG_UINT32(0);
	GLuint mask = ARG_UINT32(1);
	::glStencilMaskSeparate(face, mask);
	NanReturnUndefined();
}

//void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass);
MODULE_EXPORT_IMPLEMENT(glStencilOp)
{
	NanScope();
	GLenum fail = ARG_UINT32(0);
	GLenum zfail = ARG_UINT32(1);
	GLenum zpass = ARG_UINT32(2);
	::glStencilOp(fail, zfail, zpass);
	NanReturnUndefined();
}

//void glStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
MODULE_EXPORT_IMPLEMENT(glStencilOpSeparate)
{
	NanScope();
	GLenum face = ARG_UINT32(0);
	GLenum fail = ARG_UINT32(1);
	GLenum zfail = ARG_UINT32(2);
	GLenum zpass = ARG_UINT32(3);
	::glStencilOpSeparate(face, fail, zfail, zpass);
	NanReturnUndefined();
}

//void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
MODULE_EXPORT_IMPLEMENT(glTexImage2D)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLint level = ARG_INT32(1);
	GLint internalformat = ARG_INT32(2);
	GLsizei width = ARG_UINT32(3);
	GLsizei height = ARG_UINT32(4);
	GLint border = ARG_INT32(5);
	GLenum format = ARG_UINT32(6);
	GLenum type = ARG_UINT32(7);
	const GLvoid* pixels = GetTypedArray(ARG_OBJECT(8)); // TODO: length
	::glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels); // TODO
	NanReturnUndefined();
}

//void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
MODULE_EXPORT_IMPLEMENT(glTexParameterf)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	GLfloat param = (GLfloat) ARG_NUMBER(2);
	::glTexParameterf(target, pname, param);
	NanReturnUndefined();
}

//void glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params);
MODULE_EXPORT_IMPLEMENT(glTexParameterfv)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	const GLfloat* params = (const GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(2)); // TODO: length
	::glTexParameterfv(target, pname, params);
	NanReturnUndefined();
}

//void glTexParameteri(GLenum target, GLenum pname, GLint param);
MODULE_EXPORT_IMPLEMENT(glTexParameteri)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	GLint param = ARG_INT32(2);
	::glTexParameteri(target, pname, param);
	NanReturnUndefined();
}

//void glTexParameteriv(GLenum target, GLenum pname, const GLint* params);
MODULE_EXPORT_IMPLEMENT(glTexParameteriv)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLenum pname = ARG_UINT32(1);
	const GLint* params = (const GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(2)); // TODO: length
	::glTexParameteriv(target, pname, params);
	NanReturnUndefined();
}

//void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels);
MODULE_EXPORT_IMPLEMENT(glTexSubImage2D)
{
	NanScope();
	GLenum target = ARG_UINT32(0);
	GLint level = ARG_INT32(1);
	GLint xoffset = ARG_INT32(2);
	GLint yoffset = ARG_INT32(3);
	GLsizei width = ARG_UINT32(4);
	GLsizei height = ARG_UINT32(5);
	GLenum format = ARG_UINT32(6);
	GLenum type = ARG_UINT32(7);
	const GLvoid* pixels = GetTypedArray(ARG_OBJECT(8)); // TODO: length
	::glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels); // TODO
	NanReturnUndefined();
}

//void glUniform1f(GLint location, GLfloat x);
MODULE_EXPORT_IMPLEMENT(glUniform1f)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLfloat x = (GLfloat) ARG_NUMBER(1);
	::glUniform1f(location, x);
	NanReturnUndefined();
}

//void glUniform1fv(GLint location, GLsizei count, const GLfloat* v);
MODULE_EXPORT_IMPLEMENT(glUniform1fv)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLsizei count = ARG_UINT32(1);
	const GLfloat* v = (const GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(2), count);
	::glUniform1fv(location, count, v);
	NanReturnUndefined();
}

//void glUniform1i(GLint location, GLint x);
MODULE_EXPORT_IMPLEMENT(glUniform1i)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLint x = ARG_INT32(1);
	::glUniform1i(location, x);
	NanReturnUndefined();
}

//void glUniform1iv(GLint location, GLsizei count, const GLint* v);
MODULE_EXPORT_IMPLEMENT(glUniform1iv)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLsizei count = ARG_UINT32(1);
	const GLint* v = (const GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(2), count);
	::glUniform1iv(location, count, v);
	NanReturnUndefined();
}

//void glUniform2f(GLint location, GLfloat x, GLfloat y);
MODULE_EXPORT_IMPLEMENT(glUniform2f)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLfloat x = (GLfloat) ARG_NUMBER(1);
	GLfloat y = (GLfloat) ARG_NUMBER(2);
	::glUniform2f(location, x, y);
	NanReturnUndefined();
}

//void glUniform2fv(GLint location, GLsizei count, const GLfloat* v);
MODULE_EXPORT_IMPLEMENT(glUniform2fv)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLsizei count = ARG_UINT32(1);
	const GLfloat* v = (const GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(2), count * 2);
	::glUniform2fv(location, count, v);
	NanReturnUndefined();
}

//void glUniform2i(GLint location, GLint x, GLint y);
MODULE_EXPORT_IMPLEMENT(glUniform2i)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLint x = ARG_INT32(1);
	GLint y = ARG_INT32(2);
	::glUniform2i(location, x, y);
	NanReturnUndefined();
}

//void glUniform2iv(GLint location, GLsizei count, const GLint* v);
MODULE_EXPORT_IMPLEMENT(glUniform2iv)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLsizei count = ARG_UINT32(1);
	const GLint* v = (const GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(2), count * 2);
	::glUniform2iv(location, count, v);
	NanReturnUndefined();
}

//void glUniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z);
MODULE_EXPORT_IMPLEMENT(glUniform3f)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLfloat x = (GLfloat) ARG_NUMBER(1);
	GLfloat y = (GLfloat) ARG_NUMBER(2);
	GLfloat z = (GLfloat) ARG_NUMBER(3);
	::glUniform3f(location, x, y, z);
	NanReturnUndefined();
}

//void glUniform3fv(GLint location, GLsizei count, const GLfloat* v);
MODULE_EXPORT_IMPLEMENT(glUniform3fv)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLsizei count = ARG_UINT32(1);
	const GLfloat* v = (const GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(2), count * 3);
	::glUniform3fv(location, count, v);
	NanReturnUndefined();
}

//void glUniform3i(GLint location, GLint x, GLint y, GLint z);
MODULE_EXPORT_IMPLEMENT(glUniform3i)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLint x = ARG_INT32(1);
	GLint y = ARG_INT32(2);
	GLint z = ARG_INT32(3);
	::glUniform3i(location, x, y, z);
	NanReturnUndefined();
}

//void glUniform3iv(GLint location, GLsizei count, const GLint* v);
MODULE_EXPORT_IMPLEMENT(glUniform3iv)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLsizei count = ARG_UINT32(1);
	const GLint* v = (const GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(2), count * 3);
	::glUniform3iv(location, count, v);
	NanReturnUndefined();
}

//void glUniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
MODULE_EXPORT_IMPLEMENT(glUniform4f)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLfloat x = (GLfloat) ARG_NUMBER(1);
	GLfloat y = (GLfloat) ARG_NUMBER(2);
	GLfloat z = (GLfloat) ARG_NUMBER(3);
	GLfloat w = (GLfloat) ARG_NUMBER(4);
	::glUniform4f(location, x, y, z, w);
	NanReturnUndefined();
}

//void glUniform4fv(GLint location, GLsizei count, const GLfloat* v);
MODULE_EXPORT_IMPLEMENT(glUniform4fv)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLsizei count = ARG_UINT32(1);
	const GLfloat* v = (const GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(2), count * 4);
	::glUniform4fv(location, count, v);
	NanReturnUndefined();
}

//void glUniform4i(GLint location, GLint x, GLint y, GLint z, GLint w);
MODULE_EXPORT_IMPLEMENT(glUniform4i)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLint x = ARG_INT32(1);
	GLint y = ARG_INT32(2);
	GLint z = ARG_INT32(3);
	GLint w = ARG_INT32(4);
	::glUniform4i(location, x, y, z, w);
	NanReturnUndefined();
}

//void glUniform4iv(GLint location, GLsizei count, const GLint* v);
MODULE_EXPORT_IMPLEMENT(glUniform4iv)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLsizei count = ARG_UINT32(1);
	const GLint* v = (const GLint*) GetTypedArray<kExternalIntArray>(ARG_OBJECT(2), count * 4);
	::glUniform4iv(location, count, v);
	NanReturnUndefined();
}

//void glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
MODULE_EXPORT_IMPLEMENT(glUniformMatrix2fv)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLsizei count = ARG_UINT32(1);
	GLboolean transpose = ARG_BOOLEAN(2);
	const GLfloat* value = (const GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(3), count * 4);
	::glUniformMatrix2fv(location, count, transpose, value);
	NanReturnUndefined();
}

//void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
MODULE_EXPORT_IMPLEMENT(glUniformMatrix3fv)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLsizei count = ARG_UINT32(1);
	GLboolean transpose = ARG_BOOLEAN(2);
	const GLfloat* value = (const GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(3), count * 9);
	::glUniformMatrix3fv(location, count, transpose, value);
	NanReturnUndefined();
}

//void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
MODULE_EXPORT_IMPLEMENT(glUniformMatrix4fv)
{
	NanScope();
	GLint location = ARG_INT32(0);
	GLsizei count = ARG_UINT32(1);
	GLboolean transpose = ARG_BOOLEAN(2);
	const GLfloat* value = (const GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(3), count * 16);
	::glUniformMatrix4fv(location, count, transpose, value);
	NanReturnUndefined();
}

//void glUseProgram(GLuint program);
MODULE_EXPORT_IMPLEMENT(glUseProgram)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	::glUseProgram(program);
	NanReturnUndefined();
}

//void glValidateProgram(GLuint program);
MODULE_EXPORT_IMPLEMENT(glValidateProgram)
{
	NanScope();
	GLuint program = ARG_UINT32(0);
	::glValidateProgram(program);
	NanReturnUndefined();
}

//void glVertexAttrib1f(GLuint indx, GLfloat x);
MODULE_EXPORT_IMPLEMENT(glVertexAttrib1f)
{
	NanScope();
	GLuint indx = ARG_UINT32(0);
	GLfloat x = (GLfloat) ARG_NUMBER(1);
	::glVertexAttrib1f(indx, x);
	NanReturnUndefined();
}

//void glVertexAttrib1fv(GLuint indx, const GLfloat* values);
MODULE_EXPORT_IMPLEMENT(glVertexAttrib1fv)
{
	NanScope();
	GLuint indx = ARG_UINT32(0);
	const GLfloat* values = (const GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(1), 1);
	::glVertexAttrib1fv(indx, values);
	NanReturnUndefined();
}

//void glVertexAttrib2f(GLuint indx, GLfloat x, GLfloat y);
MODULE_EXPORT_IMPLEMENT(glVertexAttrib2f)
{
	NanScope();
	GLuint indx = ARG_UINT32(0);
	GLfloat x = (GLfloat) ARG_NUMBER(1);
	GLfloat y = (GLfloat) ARG_NUMBER(2);
	::glVertexAttrib2f(indx, x, y);
	NanReturnUndefined();
}

//void glVertexAttrib2fv(GLuint indx, const GLfloat* values);
MODULE_EXPORT_IMPLEMENT(glVertexAttrib2fv)
{
	NanScope();
	GLuint indx = ARG_UINT32(0);
	const GLfloat* values = (const GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(1), 2);
	::glVertexAttrib2fv(indx, values);
	NanReturnUndefined();
}

//void glVertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z);
MODULE_EXPORT_IMPLEMENT(glVertexAttrib3f)
{
	NanScope();
	GLuint indx = ARG_UINT32(0);
	GLfloat x = (GLfloat) ARG_NUMBER(1);
	GLfloat y = (GLfloat) ARG_NUMBER(2);
	GLfloat z = (GLfloat) ARG_NUMBER(3);
	::glVertexAttrib3f(indx, x, y, z);
	NanReturnUndefined();
}

//void glVertexAttrib3fv(GLuint indx, const GLfloat* values);
MODULE_EXPORT_IMPLEMENT(glVertexAttrib3fv)
{
	NanScope();
	GLuint indx = ARG_UINT32(0);
	const GLfloat* values = (const GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(1), 3);
	::glVertexAttrib3fv(indx, values);
	NanReturnUndefined();
}

//void glVertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
MODULE_EXPORT_IMPLEMENT(glVertexAttrib4f)
{
	NanScope();
	GLuint indx = ARG_UINT32(0);
	GLfloat x = (GLfloat) ARG_NUMBER(1);
	GLfloat y = (GLfloat) ARG_NUMBER(2);
	GLfloat z = (GLfloat) ARG_NUMBER(3);
	GLfloat w = (GLfloat) ARG_NUMBER(4);
	::glVertexAttrib4f(indx, x, y, z, w);
	NanReturnUndefined();
}

//void glVertexAttrib4fv(GLuint indx, const GLfloat* values);
MODULE_EXPORT_IMPLEMENT(glVertexAttrib4fv)
{
	NanScope();
	GLuint indx = ARG_UINT32(0);
	const GLfloat* values = (const GLfloat*) GetTypedArray<kExternalFloatArray>(ARG_OBJECT(1), 4);
	::glVertexAttrib4fv(indx, values);
	NanReturnUndefined();
}

//void glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr);
MODULE_EXPORT_IMPLEMENT(glVertexAttribPointer)
{
	NanScope();
	GLuint indx = ARG_UINT32(0);
	GLint size = ARG_INT32(1);
	GLenum type = ARG_UINT32(2);
	GLboolean normalized = ARG_BOOLEAN(3);
	GLsizei stride = ARG_UINT32(4);
	GLuint offset = ARG_UINT32(5);
	const GLvoid* ptr = (const GLvoid*)(((char*)(0)) + offset);
	::glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
	NanReturnUndefined();
}

//void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
MODULE_EXPORT_IMPLEMENT(glViewport)
{
	NanScope();
	GLint x = ARG_INT32(0);
	GLint y = ARG_INT32(1);
	GLsizei width = ARG_UINT32(2);
	GLsizei height = ARG_UINT32(3);
	::glViewport(x, y, width, height);
	NanReturnUndefined();
}

#endif // HAVE_OPENGLES2

#if NODE_VERSION_AT_LEAST(0,11,0)
void init(Handle<Object> exports, Handle<Value> module, Handle<Context> context)
#else
void init(Handle<Object> exports/*, Handle<Value> module*/)
#endif
{
	NanScope();

	#ifdef HAVE_OPENGLES2

//	MODULE_CONSTANT(exports, GL_ES_VERSION_2_0);
	MODULE_CONSTANT(exports, GL_DEPTH_BUFFER_BIT);
	MODULE_CONSTANT(exports, GL_STENCIL_BUFFER_BIT);
	MODULE_CONSTANT(exports, GL_COLOR_BUFFER_BIT);
	MODULE_CONSTANT(exports, GL_FALSE);
	MODULE_CONSTANT(exports, GL_TRUE);
	MODULE_CONSTANT(exports, GL_POINTS);
	MODULE_CONSTANT(exports, GL_LINES);
	MODULE_CONSTANT(exports, GL_LINE_LOOP);
	MODULE_CONSTANT(exports, GL_LINE_STRIP);
	MODULE_CONSTANT(exports, GL_TRIANGLES);
	MODULE_CONSTANT(exports, GL_TRIANGLE_STRIP);
	MODULE_CONSTANT(exports, GL_TRIANGLE_FAN);
	MODULE_CONSTANT(exports, GL_ZERO);
	MODULE_CONSTANT(exports, GL_ONE);
	MODULE_CONSTANT(exports, GL_SRC_COLOR);
	MODULE_CONSTANT(exports, GL_ONE_MINUS_SRC_COLOR);
	MODULE_CONSTANT(exports, GL_SRC_ALPHA);
	MODULE_CONSTANT(exports, GL_ONE_MINUS_SRC_ALPHA);
	MODULE_CONSTANT(exports, GL_DST_ALPHA);
	MODULE_CONSTANT(exports, GL_ONE_MINUS_DST_ALPHA);
	MODULE_CONSTANT(exports, GL_DST_COLOR);
	MODULE_CONSTANT(exports, GL_ONE_MINUS_DST_COLOR);
	MODULE_CONSTANT(exports, GL_SRC_ALPHA_SATURATE);
	MODULE_CONSTANT(exports, GL_FUNC_ADD);
	MODULE_CONSTANT(exports, GL_BLEND_EQUATION);
	MODULE_CONSTANT(exports, GL_BLEND_EQUATION_RGB);
	MODULE_CONSTANT(exports, GL_BLEND_EQUATION_ALPHA);
	MODULE_CONSTANT(exports, GL_FUNC_SUBTRACT);
	MODULE_CONSTANT(exports, GL_FUNC_REVERSE_SUBTRACT);
	MODULE_CONSTANT(exports, GL_BLEND_DST_RGB);
	MODULE_CONSTANT(exports, GL_BLEND_SRC_RGB);
	MODULE_CONSTANT(exports, GL_BLEND_DST_ALPHA);
	MODULE_CONSTANT(exports, GL_BLEND_SRC_ALPHA);
	MODULE_CONSTANT(exports, GL_CONSTANT_COLOR);
	MODULE_CONSTANT(exports, GL_ONE_MINUS_CONSTANT_COLOR);
	MODULE_CONSTANT(exports, GL_CONSTANT_ALPHA);
	MODULE_CONSTANT(exports, GL_ONE_MINUS_CONSTANT_ALPHA);
	MODULE_CONSTANT(exports, GL_BLEND_COLOR);
	MODULE_CONSTANT(exports, GL_ARRAY_BUFFER);
	MODULE_CONSTANT(exports, GL_ELEMENT_ARRAY_BUFFER);
	MODULE_CONSTANT(exports, GL_ARRAY_BUFFER_BINDING);
	MODULE_CONSTANT(exports, GL_ELEMENT_ARRAY_BUFFER_BINDING);
	MODULE_CONSTANT(exports, GL_STREAM_DRAW);
	MODULE_CONSTANT(exports, GL_STATIC_DRAW);
	MODULE_CONSTANT(exports, GL_DYNAMIC_DRAW);
	MODULE_CONSTANT(exports, GL_BUFFER_SIZE);
	MODULE_CONSTANT(exports, GL_BUFFER_USAGE);
	MODULE_CONSTANT(exports, GL_CURRENT_VERTEX_ATTRIB);
	MODULE_CONSTANT(exports, GL_FRONT);
	MODULE_CONSTANT(exports, GL_BACK);
	MODULE_CONSTANT(exports, GL_FRONT_AND_BACK);
	MODULE_CONSTANT(exports, GL_TEXTURE_2D);
	MODULE_CONSTANT(exports, GL_CULL_FACE);
	MODULE_CONSTANT(exports, GL_BLEND);
	MODULE_CONSTANT(exports, GL_DITHER);
	MODULE_CONSTANT(exports, GL_STENCIL_TEST);
	MODULE_CONSTANT(exports, GL_DEPTH_TEST);
	MODULE_CONSTANT(exports, GL_SCISSOR_TEST);
	MODULE_CONSTANT(exports, GL_POLYGON_OFFSET_FILL);
	MODULE_CONSTANT(exports, GL_SAMPLE_ALPHA_TO_COVERAGE);
	MODULE_CONSTANT(exports, GL_SAMPLE_COVERAGE);
	MODULE_CONSTANT(exports, GL_NO_ERROR);
	MODULE_CONSTANT(exports, GL_INVALID_ENUM);
	MODULE_CONSTANT(exports, GL_INVALID_VALUE);
	MODULE_CONSTANT(exports, GL_INVALID_OPERATION);
	MODULE_CONSTANT(exports, GL_OUT_OF_MEMORY);
	MODULE_CONSTANT(exports, GL_CW);
	MODULE_CONSTANT(exports, GL_CCW);
	MODULE_CONSTANT(exports, GL_LINE_WIDTH);
	MODULE_CONSTANT(exports, GL_ALIASED_POINT_SIZE_RANGE);
	MODULE_CONSTANT(exports, GL_ALIASED_LINE_WIDTH_RANGE);
	MODULE_CONSTANT(exports, GL_CULL_FACE_MODE);
	MODULE_CONSTANT(exports, GL_FRONT_FACE);
	MODULE_CONSTANT(exports, GL_DEPTH_RANGE);
	MODULE_CONSTANT(exports, GL_DEPTH_WRITEMASK);
	MODULE_CONSTANT(exports, GL_DEPTH_CLEAR_VALUE);
	MODULE_CONSTANT(exports, GL_DEPTH_FUNC);
	MODULE_CONSTANT(exports, GL_STENCIL_CLEAR_VALUE);
	MODULE_CONSTANT(exports, GL_STENCIL_FUNC);
	MODULE_CONSTANT(exports, GL_STENCIL_FAIL);
	MODULE_CONSTANT(exports, GL_STENCIL_PASS_DEPTH_FAIL);
	MODULE_CONSTANT(exports, GL_STENCIL_PASS_DEPTH_PASS);
	MODULE_CONSTANT(exports, GL_STENCIL_REF);
	MODULE_CONSTANT(exports, GL_STENCIL_VALUE_MASK);
	MODULE_CONSTANT(exports, GL_STENCIL_WRITEMASK);
	MODULE_CONSTANT(exports, GL_STENCIL_BACK_FUNC);
	MODULE_CONSTANT(exports, GL_STENCIL_BACK_FAIL);
	MODULE_CONSTANT(exports, GL_STENCIL_BACK_PASS_DEPTH_FAIL);
	MODULE_CONSTANT(exports, GL_STENCIL_BACK_PASS_DEPTH_PASS);
	MODULE_CONSTANT(exports, GL_STENCIL_BACK_REF);
	MODULE_CONSTANT(exports, GL_STENCIL_BACK_VALUE_MASK);
	MODULE_CONSTANT(exports, GL_STENCIL_BACK_WRITEMASK);
	MODULE_CONSTANT(exports, GL_VIEWPORT);
	MODULE_CONSTANT(exports, GL_SCISSOR_BOX);
	MODULE_CONSTANT(exports, GL_COLOR_CLEAR_VALUE);
	MODULE_CONSTANT(exports, GL_COLOR_WRITEMASK);
	MODULE_CONSTANT(exports, GL_UNPACK_ALIGNMENT);
	MODULE_CONSTANT(exports, GL_PACK_ALIGNMENT);
	MODULE_CONSTANT(exports, GL_MAX_TEXTURE_SIZE);
	MODULE_CONSTANT(exports, GL_MAX_VIEWPORT_DIMS);
	MODULE_CONSTANT(exports, GL_SUBPIXEL_BITS);
	MODULE_CONSTANT(exports, GL_RED_BITS);
	MODULE_CONSTANT(exports, GL_GREEN_BITS);
	MODULE_CONSTANT(exports, GL_BLUE_BITS);
	MODULE_CONSTANT(exports, GL_ALPHA_BITS);
	MODULE_CONSTANT(exports, GL_DEPTH_BITS);
	MODULE_CONSTANT(exports, GL_STENCIL_BITS);
	MODULE_CONSTANT(exports, GL_POLYGON_OFFSET_UNITS);
	MODULE_CONSTANT(exports, GL_POLYGON_OFFSET_FACTOR);
	MODULE_CONSTANT(exports, GL_TEXTURE_BINDING_2D);
	MODULE_CONSTANT(exports, GL_SAMPLE_BUFFERS);
	MODULE_CONSTANT(exports, GL_SAMPLES);
	MODULE_CONSTANT(exports, GL_SAMPLE_COVERAGE_VALUE);
	MODULE_CONSTANT(exports, GL_SAMPLE_COVERAGE_INVERT);
	MODULE_CONSTANT(exports, GL_NUM_COMPRESSED_TEXTURE_FORMATS);
	MODULE_CONSTANT(exports, GL_COMPRESSED_TEXTURE_FORMATS);
	MODULE_CONSTANT(exports, GL_DONT_CARE);
	MODULE_CONSTANT(exports, GL_FASTEST);
	MODULE_CONSTANT(exports, GL_NICEST);
	MODULE_CONSTANT(exports, GL_GENERATE_MIPMAP_HINT);
	MODULE_CONSTANT(exports, GL_BYTE);
	MODULE_CONSTANT(exports, GL_UNSIGNED_BYTE);
	MODULE_CONSTANT(exports, GL_SHORT);
	MODULE_CONSTANT(exports, GL_UNSIGNED_SHORT);
	MODULE_CONSTANT(exports, GL_INT);
	MODULE_CONSTANT(exports, GL_UNSIGNED_INT);
	MODULE_CONSTANT(exports, GL_FLOAT);
	MODULE_CONSTANT(exports, GL_FIXED);
	MODULE_CONSTANT(exports, GL_DEPTH_COMPONENT);
	MODULE_CONSTANT(exports, GL_ALPHA);
	MODULE_CONSTANT(exports, GL_RGB);
	MODULE_CONSTANT(exports, GL_RGBA);
	MODULE_CONSTANT(exports, GL_LUMINANCE);
	MODULE_CONSTANT(exports, GL_LUMINANCE_ALPHA);
	MODULE_CONSTANT(exports, GL_UNSIGNED_SHORT_4_4_4_4);
	MODULE_CONSTANT(exports, GL_UNSIGNED_SHORT_5_5_5_1);
	MODULE_CONSTANT(exports, GL_UNSIGNED_SHORT_5_6_5);
	MODULE_CONSTANT(exports, GL_FRAGMENT_SHADER);
	MODULE_CONSTANT(exports, GL_VERTEX_SHADER);
	MODULE_CONSTANT(exports, GL_MAX_VERTEX_ATTRIBS);
	MODULE_CONSTANT(exports, GL_MAX_VERTEX_UNIFORM_VECTORS);
	MODULE_CONSTANT(exports, GL_MAX_VARYING_VECTORS);
	MODULE_CONSTANT(exports, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
	MODULE_CONSTANT(exports, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
	MODULE_CONSTANT(exports, GL_MAX_TEXTURE_IMAGE_UNITS);
	MODULE_CONSTANT(exports, GL_MAX_FRAGMENT_UNIFORM_VECTORS);
	MODULE_CONSTANT(exports, GL_SHADER_TYPE);
	MODULE_CONSTANT(exports, GL_DELETE_STATUS);
	MODULE_CONSTANT(exports, GL_LINK_STATUS);
	MODULE_CONSTANT(exports, GL_VALIDATE_STATUS);
	MODULE_CONSTANT(exports, GL_ATTACHED_SHADERS);
	MODULE_CONSTANT(exports, GL_ACTIVE_UNIFORMS);
	MODULE_CONSTANT(exports, GL_ACTIVE_UNIFORM_MAX_LENGTH);
	MODULE_CONSTANT(exports, GL_ACTIVE_ATTRIBUTES);
	MODULE_CONSTANT(exports, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH);
	MODULE_CONSTANT(exports, GL_SHADING_LANGUAGE_VERSION);
	MODULE_CONSTANT(exports, GL_CURRENT_PROGRAM);
	MODULE_CONSTANT(exports, GL_NEVER);
	MODULE_CONSTANT(exports, GL_LESS);
	MODULE_CONSTANT(exports, GL_EQUAL);
	MODULE_CONSTANT(exports, GL_LEQUAL);
	MODULE_CONSTANT(exports, GL_GREATER);
	MODULE_CONSTANT(exports, GL_NOTEQUAL);
	MODULE_CONSTANT(exports, GL_GEQUAL);
	MODULE_CONSTANT(exports, GL_ALWAYS);
	MODULE_CONSTANT(exports, GL_KEEP);
	MODULE_CONSTANT(exports, GL_REPLACE);
	MODULE_CONSTANT(exports, GL_INCR);
	MODULE_CONSTANT(exports, GL_DECR);
	MODULE_CONSTANT(exports, GL_INVERT);
	MODULE_CONSTANT(exports, GL_INCR_WRAP);
	MODULE_CONSTANT(exports, GL_DECR_WRAP);
	MODULE_CONSTANT(exports, GL_VENDOR);
	MODULE_CONSTANT(exports, GL_RENDERER);
	MODULE_CONSTANT(exports, GL_VERSION);
	MODULE_CONSTANT(exports, GL_EXTENSIONS);
	MODULE_CONSTANT(exports, GL_NEAREST);
	MODULE_CONSTANT(exports, GL_LINEAR);
	MODULE_CONSTANT(exports, GL_NEAREST_MIPMAP_NEAREST);
	MODULE_CONSTANT(exports, GL_LINEAR_MIPMAP_NEAREST);
	MODULE_CONSTANT(exports, GL_NEAREST_MIPMAP_LINEAR);
	MODULE_CONSTANT(exports, GL_LINEAR_MIPMAP_LINEAR);
	MODULE_CONSTANT(exports, GL_TEXTURE_MAG_FILTER);
	MODULE_CONSTANT(exports, GL_TEXTURE_MIN_FILTER);
	MODULE_CONSTANT(exports, GL_TEXTURE_WRAP_S);
	MODULE_CONSTANT(exports, GL_TEXTURE_WRAP_T);
	MODULE_CONSTANT(exports, GL_TEXTURE);
	MODULE_CONSTANT(exports, GL_TEXTURE_CUBE_MAP);
	MODULE_CONSTANT(exports, GL_TEXTURE_BINDING_CUBE_MAP);
	MODULE_CONSTANT(exports, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	MODULE_CONSTANT(exports, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	MODULE_CONSTANT(exports, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	MODULE_CONSTANT(exports, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	MODULE_CONSTANT(exports, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	MODULE_CONSTANT(exports, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	MODULE_CONSTANT(exports, GL_MAX_CUBE_MAP_TEXTURE_SIZE);
	MODULE_CONSTANT(exports, GL_TEXTURE0);
	MODULE_CONSTANT(exports, GL_TEXTURE1);
	MODULE_CONSTANT(exports, GL_TEXTURE2);
	MODULE_CONSTANT(exports, GL_TEXTURE3);
	MODULE_CONSTANT(exports, GL_TEXTURE4);
	MODULE_CONSTANT(exports, GL_TEXTURE5);
	MODULE_CONSTANT(exports, GL_TEXTURE6);
	MODULE_CONSTANT(exports, GL_TEXTURE7);
	MODULE_CONSTANT(exports, GL_TEXTURE8);
	MODULE_CONSTANT(exports, GL_TEXTURE9);
	MODULE_CONSTANT(exports, GL_TEXTURE10);
	MODULE_CONSTANT(exports, GL_TEXTURE11);
	MODULE_CONSTANT(exports, GL_TEXTURE12);
	MODULE_CONSTANT(exports, GL_TEXTURE13);
	MODULE_CONSTANT(exports, GL_TEXTURE14);
	MODULE_CONSTANT(exports, GL_TEXTURE15);
	MODULE_CONSTANT(exports, GL_TEXTURE16);
	MODULE_CONSTANT(exports, GL_TEXTURE17);
	MODULE_CONSTANT(exports, GL_TEXTURE18);
	MODULE_CONSTANT(exports, GL_TEXTURE19);
	MODULE_CONSTANT(exports, GL_TEXTURE20);
	MODULE_CONSTANT(exports, GL_TEXTURE21);
	MODULE_CONSTANT(exports, GL_TEXTURE22);
	MODULE_CONSTANT(exports, GL_TEXTURE23);
	MODULE_CONSTANT(exports, GL_TEXTURE24);
	MODULE_CONSTANT(exports, GL_TEXTURE25);
	MODULE_CONSTANT(exports, GL_TEXTURE26);
	MODULE_CONSTANT(exports, GL_TEXTURE27);
	MODULE_CONSTANT(exports, GL_TEXTURE28);
	MODULE_CONSTANT(exports, GL_TEXTURE29);
	MODULE_CONSTANT(exports, GL_TEXTURE30);
	MODULE_CONSTANT(exports, GL_TEXTURE31);
	MODULE_CONSTANT(exports, GL_ACTIVE_TEXTURE);
	MODULE_CONSTANT(exports, GL_REPEAT);
	MODULE_CONSTANT(exports, GL_CLAMP_TO_EDGE);
	MODULE_CONSTANT(exports, GL_MIRRORED_REPEAT);
	MODULE_CONSTANT(exports, GL_FLOAT_VEC2);
	MODULE_CONSTANT(exports, GL_FLOAT_VEC3);
	MODULE_CONSTANT(exports, GL_FLOAT_VEC4);
	MODULE_CONSTANT(exports, GL_INT_VEC2);
	MODULE_CONSTANT(exports, GL_INT_VEC3);
	MODULE_CONSTANT(exports, GL_INT_VEC4);
	MODULE_CONSTANT(exports, GL_BOOL);
	MODULE_CONSTANT(exports, GL_BOOL_VEC2);
	MODULE_CONSTANT(exports, GL_BOOL_VEC3);
	MODULE_CONSTANT(exports, GL_BOOL_VEC4);
	MODULE_CONSTANT(exports, GL_FLOAT_MAT2);
	MODULE_CONSTANT(exports, GL_FLOAT_MAT3);
	MODULE_CONSTANT(exports, GL_FLOAT_MAT4);
	MODULE_CONSTANT(exports, GL_SAMPLER_2D);
	MODULE_CONSTANT(exports, GL_SAMPLER_CUBE);
	MODULE_CONSTANT(exports, GL_VERTEX_ATTRIB_ARRAY_ENABLED);
	MODULE_CONSTANT(exports, GL_VERTEX_ATTRIB_ARRAY_SIZE);
	MODULE_CONSTANT(exports, GL_VERTEX_ATTRIB_ARRAY_STRIDE);
	MODULE_CONSTANT(exports, GL_VERTEX_ATTRIB_ARRAY_TYPE);
	MODULE_CONSTANT(exports, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED);
	MODULE_CONSTANT(exports, GL_VERTEX_ATTRIB_ARRAY_POINTER);
	MODULE_CONSTANT(exports, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING);
	MODULE_CONSTANT(exports, GL_IMPLEMENTATION_COLOR_READ_TYPE);
	MODULE_CONSTANT(exports, GL_IMPLEMENTATION_COLOR_READ_FORMAT);
	MODULE_CONSTANT(exports, GL_COMPILE_STATUS);
	MODULE_CONSTANT(exports, GL_INFO_LOG_LENGTH);
	MODULE_CONSTANT(exports, GL_SHADER_SOURCE_LENGTH);
	MODULE_CONSTANT(exports, GL_SHADER_COMPILER);
	MODULE_CONSTANT(exports, GL_SHADER_BINARY_FORMATS);
	MODULE_CONSTANT(exports, GL_NUM_SHADER_BINARY_FORMATS);
	MODULE_CONSTANT(exports, GL_LOW_FLOAT);
	MODULE_CONSTANT(exports, GL_MEDIUM_FLOAT);
	MODULE_CONSTANT(exports, GL_HIGH_FLOAT);
	MODULE_CONSTANT(exports, GL_LOW_INT);
	MODULE_CONSTANT(exports, GL_MEDIUM_INT);
	MODULE_CONSTANT(exports, GL_HIGH_INT);
	MODULE_CONSTANT(exports, GL_FRAMEBUFFER);
	MODULE_CONSTANT(exports, GL_RENDERBUFFER);
	MODULE_CONSTANT(exports, GL_RGBA4);
	MODULE_CONSTANT(exports, GL_RGB5_A1);
	MODULE_CONSTANT(exports, GL_RGB565);
	MODULE_CONSTANT(exports, GL_DEPTH_COMPONENT16);
	MODULE_CONSTANT(exports, GL_STENCIL_INDEX8);
	MODULE_CONSTANT(exports, GL_RENDERBUFFER_WIDTH);
	MODULE_CONSTANT(exports, GL_RENDERBUFFER_HEIGHT);
	MODULE_CONSTANT(exports, GL_RENDERBUFFER_INTERNAL_FORMAT);
	MODULE_CONSTANT(exports, GL_RENDERBUFFER_RED_SIZE);
	MODULE_CONSTANT(exports, GL_RENDERBUFFER_GREEN_SIZE);
	MODULE_CONSTANT(exports, GL_RENDERBUFFER_BLUE_SIZE);
	MODULE_CONSTANT(exports, GL_RENDERBUFFER_ALPHA_SIZE);
	MODULE_CONSTANT(exports, GL_RENDERBUFFER_DEPTH_SIZE);
	MODULE_CONSTANT(exports, GL_RENDERBUFFER_STENCIL_SIZE);
	MODULE_CONSTANT(exports, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
	MODULE_CONSTANT(exports, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME);
	MODULE_CONSTANT(exports, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL);
	MODULE_CONSTANT(exports, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);
	MODULE_CONSTANT(exports, GL_COLOR_ATTACHMENT0);
	MODULE_CONSTANT(exports, GL_DEPTH_ATTACHMENT);
	MODULE_CONSTANT(exports, GL_STENCIL_ATTACHMENT);
	MODULE_CONSTANT(exports, GL_NONE);
	MODULE_CONSTANT(exports, GL_FRAMEBUFFER_COMPLETE);
	MODULE_CONSTANT(exports, GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
	MODULE_CONSTANT(exports, GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
//	MODULE_CONSTANT(exports, GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS);
	MODULE_CONSTANT(exports, GL_FRAMEBUFFER_UNSUPPORTED);
	MODULE_CONSTANT(exports, GL_FRAMEBUFFER_BINDING);
	MODULE_CONSTANT(exports, GL_RENDERBUFFER_BINDING);
	MODULE_CONSTANT(exports, GL_MAX_RENDERBUFFER_SIZE);
	MODULE_CONSTANT(exports, GL_INVALID_FRAMEBUFFER_OPERATION);

	#ifndef GL_POINT_SPRITE
	#define GL_POINT_SPRITE 0x8861
	#endif
	MODULE_CONSTANT(exports, GL_POINT_SPRITE);
	
	#ifndef GL_VERTEX_PROGRAM_POINT_SIZE
	#define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
	#endif
	MODULE_CONSTANT(exports, GL_VERTEX_PROGRAM_POINT_SIZE);

	MODULE_EXPORT_APPLY(exports, glActiveTexture);
	MODULE_EXPORT_APPLY(exports, glAttachShader);
	MODULE_EXPORT_APPLY(exports, glBindAttribLocation);
	MODULE_EXPORT_APPLY(exports, glBindBuffer);
	MODULE_EXPORT_APPLY(exports, glBindFramebuffer);
	MODULE_EXPORT_APPLY(exports, glBindRenderbuffer);
	MODULE_EXPORT_APPLY(exports, glBindTexture);
	MODULE_EXPORT_APPLY(exports, glBlendColor);
	MODULE_EXPORT_APPLY(exports, glBlendEquation);
	MODULE_EXPORT_APPLY(exports, glBlendEquationSeparate);
	MODULE_EXPORT_APPLY(exports, glBlendFunc);
	MODULE_EXPORT_APPLY(exports, glBlendFuncSeparate);
	MODULE_EXPORT_APPLY(exports, glBufferData);
	MODULE_EXPORT_APPLY(exports, glBufferSubData);
	MODULE_EXPORT_APPLY(exports, glCheckFramebufferStatus);
	MODULE_EXPORT_APPLY(exports, glClear);
	MODULE_EXPORT_APPLY(exports, glClearColor);
	MODULE_EXPORT_APPLY(exports, glClearDepthf);
	MODULE_EXPORT_APPLY(exports, glClearStencil);
	MODULE_EXPORT_APPLY(exports, glColorMask);
	MODULE_EXPORT_APPLY(exports, glCompileShader);
	MODULE_EXPORT_APPLY(exports, glCompressedTexImage2D);
	MODULE_EXPORT_APPLY(exports, glCompressedTexSubImage2D);
	MODULE_EXPORT_APPLY(exports, glCopyTexImage2D);
	MODULE_EXPORT_APPLY(exports, glCopyTexSubImage2D);
	MODULE_EXPORT_APPLY(exports, glCreateProgram);
	MODULE_EXPORT_APPLY(exports, glCreateShader);
	MODULE_EXPORT_APPLY(exports, glCullFace);
	MODULE_EXPORT_APPLY(exports, glDeleteBuffers);
	MODULE_EXPORT_APPLY(exports, glDeleteFramebuffers);
	MODULE_EXPORT_APPLY(exports, glDeleteProgram);
	MODULE_EXPORT_APPLY(exports, glDeleteRenderbuffers);
	MODULE_EXPORT_APPLY(exports, glDeleteShader);
	MODULE_EXPORT_APPLY(exports, glDeleteTextures);
	MODULE_EXPORT_APPLY(exports, glDepthFunc);
	MODULE_EXPORT_APPLY(exports, glDepthMask);
	MODULE_EXPORT_APPLY(exports, glDepthRangef);
	MODULE_EXPORT_APPLY(exports, glDetachShader);
	MODULE_EXPORT_APPLY(exports, glDisable);
	MODULE_EXPORT_APPLY(exports, glDisableVertexAttribArray);
	MODULE_EXPORT_APPLY(exports, glDrawArrays);
	MODULE_EXPORT_APPLY(exports, glDrawElements);
	MODULE_EXPORT_APPLY(exports, glEnable);
	MODULE_EXPORT_APPLY(exports, glEnableVertexAttribArray);
	MODULE_EXPORT_APPLY(exports, glFinish);
	MODULE_EXPORT_APPLY(exports, glFlush);
	MODULE_EXPORT_APPLY(exports, glFramebufferRenderbuffer);
	MODULE_EXPORT_APPLY(exports, glFramebufferTexture2D);
	MODULE_EXPORT_APPLY(exports, glFrontFace);
	MODULE_EXPORT_APPLY(exports, glGenBuffers);
	MODULE_EXPORT_APPLY(exports, glGenerateMipmap);
	MODULE_EXPORT_APPLY(exports, glGenFramebuffers);
	MODULE_EXPORT_APPLY(exports, glGenRenderbuffers);
	MODULE_EXPORT_APPLY(exports, glGenTextures);
	MODULE_EXPORT_APPLY(exports, glGetActiveAttrib);
	MODULE_EXPORT_APPLY(exports, glGetActiveUniform);
	MODULE_EXPORT_APPLY(exports, glGetAttachedShaders);
	MODULE_EXPORT_APPLY(exports, glGetAttribLocation);
	MODULE_EXPORT_APPLY(exports, glGetBooleanv);
	MODULE_EXPORT_APPLY(exports, glGetBufferParameteriv);
	MODULE_EXPORT_APPLY(exports, glGetError);
	MODULE_EXPORT_APPLY(exports, glGetFloatv);
	MODULE_EXPORT_APPLY(exports, glGetFramebufferAttachmentParameteriv);
	MODULE_EXPORT_APPLY(exports, glGetIntegerv);
	MODULE_EXPORT_APPLY(exports, glGetProgramiv);
	MODULE_EXPORT_APPLY(exports, glGetProgramInfoLog);
	MODULE_EXPORT_APPLY(exports, glGetRenderbufferParameteriv);
	MODULE_EXPORT_APPLY(exports, glGetShaderiv);
	MODULE_EXPORT_APPLY(exports, glGetShaderInfoLog);
	MODULE_EXPORT_APPLY(exports, glGetShaderPrecisionFormat);
	MODULE_EXPORT_APPLY(exports, glGetShaderSource);
	MODULE_EXPORT_APPLY(exports, glGetString);
	MODULE_EXPORT_APPLY(exports, glGetTexParameterfv);
	MODULE_EXPORT_APPLY(exports, glGetTexParameteriv);
	MODULE_EXPORT_APPLY(exports, glGetUniformfv);
	MODULE_EXPORT_APPLY(exports, glGetUniformiv);
	MODULE_EXPORT_APPLY(exports, glGetUniformLocation);
	MODULE_EXPORT_APPLY(exports, glGetVertexAttribfv);
	MODULE_EXPORT_APPLY(exports, glGetVertexAttribiv);
	MODULE_EXPORT_APPLY(exports, glGetVertexAttribPointerv);
	MODULE_EXPORT_APPLY(exports, glHint);
	MODULE_EXPORT_APPLY(exports, glIsBuffer);
	MODULE_EXPORT_APPLY(exports, glIsEnabled);
	MODULE_EXPORT_APPLY(exports, glIsFramebuffer);
	MODULE_EXPORT_APPLY(exports, glIsProgram);
	MODULE_EXPORT_APPLY(exports, glIsRenderbuffer);
	MODULE_EXPORT_APPLY(exports, glIsShader);
	MODULE_EXPORT_APPLY(exports, glIsTexture);
	MODULE_EXPORT_APPLY(exports, glLineWidth);
	MODULE_EXPORT_APPLY(exports, glLinkProgram);
	MODULE_EXPORT_APPLY(exports, glPixelStorei);
	MODULE_EXPORT_APPLY(exports, glPolygonOffset);
	MODULE_EXPORT_APPLY(exports, glReadPixels);
	MODULE_EXPORT_APPLY(exports, glReleaseShaderCompiler);
	MODULE_EXPORT_APPLY(exports, glRenderbufferStorage);
	MODULE_EXPORT_APPLY(exports, glSampleCoverage);
	MODULE_EXPORT_APPLY(exports, glScissor);
	MODULE_EXPORT_APPLY(exports, glShaderBinary);
	MODULE_EXPORT_APPLY(exports, glShaderSource);
	MODULE_EXPORT_APPLY(exports, glStencilFunc);
	MODULE_EXPORT_APPLY(exports, glStencilFuncSeparate);
	MODULE_EXPORT_APPLY(exports, glStencilMask);
	MODULE_EXPORT_APPLY(exports, glStencilMaskSeparate);
	MODULE_EXPORT_APPLY(exports, glStencilOp);
	MODULE_EXPORT_APPLY(exports, glStencilOpSeparate);
	MODULE_EXPORT_APPLY(exports, glTexImage2D);
	MODULE_EXPORT_APPLY(exports, glTexParameterf);
	MODULE_EXPORT_APPLY(exports, glTexParameterfv);
	MODULE_EXPORT_APPLY(exports, glTexParameteri);
	MODULE_EXPORT_APPLY(exports, glTexParameteriv);
	MODULE_EXPORT_APPLY(exports, glTexSubImage2D);
	MODULE_EXPORT_APPLY(exports, glUniform1f);
	MODULE_EXPORT_APPLY(exports, glUniform1fv);
	MODULE_EXPORT_APPLY(exports, glUniform1i);
	MODULE_EXPORT_APPLY(exports, glUniform1iv);
	MODULE_EXPORT_APPLY(exports, glUniform2f);
	MODULE_EXPORT_APPLY(exports, glUniform2fv);
	MODULE_EXPORT_APPLY(exports, glUniform2i);
	MODULE_EXPORT_APPLY(exports, glUniform2iv);
	MODULE_EXPORT_APPLY(exports, glUniform3f);
	MODULE_EXPORT_APPLY(exports, glUniform3fv);
	MODULE_EXPORT_APPLY(exports, glUniform3i);
	MODULE_EXPORT_APPLY(exports, glUniform3iv);
	MODULE_EXPORT_APPLY(exports, glUniform4f);
	MODULE_EXPORT_APPLY(exports, glUniform4fv);
	MODULE_EXPORT_APPLY(exports, glUniform4i);
	MODULE_EXPORT_APPLY(exports, glUniform4iv);
	MODULE_EXPORT_APPLY(exports, glUniformMatrix2fv);
	MODULE_EXPORT_APPLY(exports, glUniformMatrix3fv);
	MODULE_EXPORT_APPLY(exports, glUniformMatrix4fv);
	MODULE_EXPORT_APPLY(exports, glUseProgram);
	MODULE_EXPORT_APPLY(exports, glValidateProgram);
	MODULE_EXPORT_APPLY(exports, glVertexAttrib1f);
	MODULE_EXPORT_APPLY(exports, glVertexAttrib1fv);
	MODULE_EXPORT_APPLY(exports, glVertexAttrib2f);
	MODULE_EXPORT_APPLY(exports, glVertexAttrib2fv);
	MODULE_EXPORT_APPLY(exports, glVertexAttrib3f);
	MODULE_EXPORT_APPLY(exports, glVertexAttrib3fv);
	MODULE_EXPORT_APPLY(exports, glVertexAttrib4f);
	MODULE_EXPORT_APPLY(exports, glVertexAttrib4fv);
	MODULE_EXPORT_APPLY(exports, glVertexAttribPointer);
	MODULE_EXPORT_APPLY(exports, glViewport);

	#endif // HAVE_OPENGLES2
}

} // namespace node_gles2

#if NODE_VERSION_AT_LEAST(0,11,0)
NODE_MODULE_CONTEXT_AWARE_BUILTIN(node_gles2, node_gles2::init)
#else
NODE_MODULE(node_gles2, node_gles2::init)
#endif

