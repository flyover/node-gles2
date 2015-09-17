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

#include <stdlib.h> // malloc, free
#include <string.h> // strdup

#ifndef strdup
#define strdup(str) strcpy((char*)malloc(strlen(str)+1),str)
#endif

#if defined(__ANDROID__)
#include <android/log.h>
#define printf(...) __android_log_print(ANDROID_LOG_INFO, "printf", __VA_ARGS__)
#endif

#define countof(_a) (sizeof(_a)/sizeof((_a)[0]))

// nan extensions

#define NANX_STRING(STRING) Nan::New<v8::String>(STRING).ToLocalChecked()
#define NANX_SYMBOL(SYMBOL) Nan::New<v8::String>(SYMBOL).ToLocalChecked()

#define NANX_CONSTANT(TARGET, CONSTANT) Nan::Set(TARGET, NANX_SYMBOL(#CONSTANT), Nan::New(CONSTANT))
#define NANX_CONSTANT_VALUE(TARGET, CONSTANT, VALUE) Nan::Set(TARGET, NANX_SYMBOL(#CONSTANT), Nan::New(VALUE))

#define NANX_CONSTANT_STRING(TARGET, CONSTANT) Nan::Set(TARGET, NANX_SYMBOL(#CONSTANT), NANX_STRING(CONSTANT))
#define NANX_CONSTANT_STRING_VALUE(TARGET, CONSTANT, VALUE) Nan::Set(TARGET, NANX_SYMBOL(#CONSTANT), NANX_STRING(VALUE))

#define NANX_EXPORT_APPLY(OBJECT, NAME) Nan::Export(OBJECT, #NAME, _export_##NAME)
#define NANX_EXPORT(NAME) static NAN_METHOD(_export_##NAME)

#define NANX_METHOD_APPLY(OBJECT_TEMPLATE, NAME) Nan::SetMethod(OBJECT_TEMPLATE, #NAME, _method_##NAME);
#define NANX_METHOD(NAME) static NAN_METHOD(_method_##NAME)

#define NANX_MEMBER_APPLY(OBJECT_TEMPLATE, NAME) Nan::SetAccessor(OBJECT_TEMPLATE, NANX_SYMBOL(#NAME), _get_##NAME, _set_##NAME);
#define NANX_MEMBER_APPLY_GET(OBJECT_TEMPLATE, NAME) Nan::SetAccessor(OBJECT_TEMPLATE, NANX_SYMBOL(#NAME), _get_##NAME, NULL); // get only
#define NANX_MEMBER_APPLY_SET(OBJECT_TEMPLATE, NAME) Nan::SetAccessor(OBJECT_TEMPLATE, NANX_SYMBOL(#NAME), NULL, _set_##NAME); // set only

#define NANX_MEMBER_VALUE(NAME) NANX_MEMBER_VALUE_GET(NAME) NANX_MEMBER_VALUE_SET(NAME)
#define NANX_MEMBER_VALUE_GET(NAME) static NAN_GETTER(_get_##NAME) { Unwrap(info.This())->SyncPull(); info.GetReturnValue().Set(Nan::New<v8::Value>(Unwrap(info.This())->m_wrap_##NAME)); }
#define NANX_MEMBER_VALUE_SET(NAME) static NAN_SETTER(_set_##NAME) { Unwrap(info.This())->m_wrap_##NAME.Reset(value.As<v8::Value>()); Unwrap(info.This())->SyncPush(); info.GetReturnValue().Set(value); }

#define NANX_MEMBER_BOOLEAN(TYPE, NAME) NANX_MEMBER_BOOLEAN_GET(TYPE, NAME) NANX_MEMBER_BOOLEAN_SET(TYPE, NAME)
#define NANX_MEMBER_BOOLEAN_GET(TYPE, NAME) static NAN_GETTER(_get_##NAME) { info.GetReturnValue().Set(Nan::New<v8::Boolean>(static_cast<bool>(Peek(info.This())->NAME))); }
#define NANX_MEMBER_BOOLEAN_SET(TYPE, NAME) static NAN_SETTER(_set_##NAME) { Peek(info.This())->NAME = static_cast<TYPE>(value->BooleanValue()); }

#define NANX_MEMBER_NUMBER(TYPE, NAME) NANX_MEMBER_NUMBER_GET(TYPE, NAME) NANX_MEMBER_NUMBER_SET(TYPE, NAME)
#define NANX_MEMBER_NUMBER_GET(TYPE, NAME) static NAN_GETTER(_get_##NAME) { info.GetReturnValue().Set(Nan::New<v8::Number>(static_cast<double>(Peek(info.This())->NAME))); }
#define NANX_MEMBER_NUMBER_SET(TYPE, NAME) static NAN_SETTER(_set_##NAME) { Peek(info.This())->NAME = static_cast<TYPE>(value->NumberValue()); }

#define NANX_MEMBER_INTEGER(TYPE, NAME) NANX_MEMBER_INTEGER_GET(TYPE, NAME) NANX_MEMBER_INTEGER_SET(TYPE, NAME)
#define NANX_MEMBER_INTEGER_GET(TYPE, NAME) static NAN_GETTER(_get_##NAME) { info.GetReturnValue().Set(Nan::New<v8::Int32>(static_cast<int32_t>(Peek(info.This())->NAME))); }
#define NANX_MEMBER_INTEGER_SET(TYPE, NAME) static NAN_SETTER(_set_##NAME) { Peek(info.This())->NAME = static_cast<TYPE>(value->IntegerValue()); }

#define NANX_MEMBER_INT32(TYPE, NAME) NANX_MEMBER_INT32_GET(TYPE, NAME) NANX_MEMBER_INT32_SET(TYPE, NAME)
#define NANX_MEMBER_INT32_GET(TYPE, NAME) static NAN_GETTER(_get_##NAME) { info.GetReturnValue().Set(Nan::New<v8::Int32>(static_cast<int32_t>(Peek(info.This())->NAME))); }
#define NANX_MEMBER_INT32_SET(TYPE, NAME) static NAN_SETTER(_set_##NAME) { Peek(info.This())->NAME = static_cast<TYPE>(value->Int32Value()); }

#define NANX_MEMBER_UINT32(TYPE, NAME) NANX_MEMBER_UINT32_GET(TYPE, NAME) NANX_MEMBER_UINT32_SET(TYPE, NAME)
#define NANX_MEMBER_UINT32_GET(TYPE, NAME) static NAN_GETTER(_get_##NAME) { info.GetReturnValue().Set(Nan::New<v8::Uint32>(static_cast<uint32_t>(Peek(info.This())->NAME))); }
#define NANX_MEMBER_UINT32_SET(TYPE, NAME) static NAN_SETTER(_set_##NAME) { Peek(info.This())->NAME = static_cast<TYPE>(value->Uint32Value()); }

#define NANX_MEMBER_STRING(NAME) NANX_MEMBER_STRING_GET(NAME) NANX_MEMBER_STRING_SET(NAME)
#define NANX_MEMBER_STRING_GET(NAME) static NAN_GETTER(_get_##NAME) { Unwrap(info.This())->SyncPull(); info.GetReturnValue().Set(Nan::New<v8::String>(Unwrap(info.This())->m_wrap_##NAME)); }
#define NANX_MEMBER_STRING_SET(NAME) static NAN_SETTER(_set_##NAME) { Unwrap(info.This())->m_wrap_##NAME.Reset(value.As<v8::String>()); Unwrap(info.This())->SyncPush(); info.GetReturnValue().Set(value); }

#define NANX_MEMBER_OBJECT(NAME) NANX_MEMBER_OBJECT_GET(NAME) NANX_MEMBER_OBJECT_SET(NAME)
#define NANX_MEMBER_OBJECT_GET(NAME) static NAN_GETTER(_get_##NAME) { Unwrap(info.This())->SyncPull(); info.GetReturnValue().Set(Nan::New<v8::Object>(Unwrap(info.This())->m_wrap_##NAME)); }
#define NANX_MEMBER_OBJECT_SET(NAME) static NAN_SETTER(_set_##NAME) { Unwrap(info.This())->m_wrap_##NAME.Reset(value.As<v8::Object>()); Unwrap(info.This())->SyncPush(); info.GetReturnValue().Set(value); }

#define NANX_MEMBER_ARRAY(NAME) NANX_MEMBER_ARRAY_GET(NAME) NANX_MEMBER_ARRAY_SET(NAME)
#define NANX_MEMBER_ARRAY_GET(NAME) static NAN_GETTER(_get_##NAME) { Unwrap(info.This())->SyncPull(); info.GetReturnValue().Set(Nan::New<v8::Array>(Unwrap(info.This())->m_wrap_##NAME)); }
#define NANX_MEMBER_ARRAY_SET(NAME) static NAN_SETTER(_set_##NAME) { Unwrap(info.This())->m_wrap_##NAME.Reset(value.As<v8::Array>()); Unwrap(info.This())->SyncPush(); info.GetReturnValue().Set(value); }

#define NANX_GLboolean(value)	static_cast<GLboolean>((value)->BooleanValue())		//	GLboolean	1+		A boolean value, either GL_TRUE or GL_FALSE
#define NANX_GLbyte(value)		static_cast<GLbyte>((value)->Int32Value())			//	GLbyte		8		Signed, 2's complement binary integer								GL_BYTE
#define NANX_GLubyte(value)		static_cast<GLubyte>((value)->Uint32Value())		//	GLubyte		8		Unsigned binary integer												GL_UNSIGNED_BYTE
#define NANX_GLshort(value)		static_cast<GLshort>((value)->Int32Value())			//	GLshort		16		Signed, 2's complement binary integer								GL_SHORT
#define NANX_GLushort(value)	static_cast<GLushort>((value)->Uint32Value())		//	GLushort	16		Unsigned binary integer												GL_UNSIGNED_SHORT
#define NANX_GLint(value)		static_cast<GLint>((value)->Int32Value())			//	GLint		32		Signed, 2's complement binary integer								GL_INT
#define NANX_GLuint(value)		static_cast<GLuint>((value)->Uint32Value())			//	GLuint		32		Unsigned binary integer												GL_UNSIGNED_INT
#define NANX_GLfixed(value)		static_cast<GLfixed>((value)->Int32Value())			//	GLfixed		32		Signed, 2's complement 16.16 integer								GL_FIXED
#define NANX_GLint64(value)		static_cast<GLint64>((value)->Int32Value())			//	GLint64		64		Signed, 2's complement binary integer
#define NANX_GLuint64(value)	static_cast<GLuint64>((value)->Uint32Value())		//	GLuint64	64		Unsigned binary integer
#define NANX_GLsizei(value)		static_cast<GLsizei>((value)->Uint32Value())		//	GLsizei		32		A non-negative binary integer, for sizes.
#define NANX_GLenum(value)		static_cast<GLenum>((value)->Uint32Value())			//	GLenum		32		An OpenGL enumerator value
#define NANX_GLintptr(value)	static_cast<GLintptr>((value)->IntegerValue())		//	GLintptr	ptrbits	Signed, 2's complement binary integer
#define NANX_GLsizeiptr(value)	static_cast<GLsizeiptr>((value)->IntegerValue())	//	GLsizeiptr	ptrbits	Non-negative binary integer size, for pointer offsets and ranges
#define NANX_GLsync(value)		static_cast<GLsync>((value)->IntegerValue())		//	GLsync		ptrbits	Sync Object handle
#define NANX_GLbitfield(value)	static_cast<GLbitfield>((value)->Uint32Value())		//	GLbitfield	32		A bitfield value
#define NANX_GLhalf(value)		static_cast<GLhalf>((value)->NumberValue())			//	GLhalf		16		An IEEE-754 floating-point value									GL_HALF_FLOAT
#define NANX_GLfloat(value)		static_cast<GLfloat>((value)->NumberValue())		//	GLfloat		32		An IEEE-754 floating-point value									GL_FLOAT
#define NANX_GLclampf(value)	static_cast<GLclampf>((value)->NumberValue())		//	GLclampf	32		An IEEE-754 floating-point value, clamped to the range [0,1]
#define NANX_GLdouble(value)	static_cast<GLdouble>((value)->NumberValue())		//	GLdouble	64		An IEEE-754 floating-point value									GL_DOUBLE
#define NANX_GLclampd(value)	static_cast<GLclampd>((value)->NumberValue())		//	GLclampd	64		An IEEE-754 floating-point value, clamped to the range [0,1]

#if NODE_VERSION_AT_LEAST(4, 0, 0)

void* GetArrayBufferViewData(v8::Local<v8::Value> value, size_t byte_length = 0)
{
	if (value->IsNull()) { return NULL; }
	assert(value->IsArrayBufferView());
	v8::Local<v8::ArrayBufferView> array_buffer_view = value.As<v8::ArrayBufferView>();
	if (!array_buffer_view->HasBuffer())
	{
		value.As<v8::Object>()->Get(NANX_SYMBOL("buffer"));
		assert(array_buffer_view->HasBuffer());
	}
	v8::Local<v8::ArrayBuffer> array_buffer = array_buffer_view->Buffer();
	assert((byte_length == 0) || (byte_length == array_buffer_view->ByteLength()));
	void* data = array_buffer->GetContents().Data();
	size_t byte_offset = array_buffer_view->ByteOffset();
	return static_cast<void*>(static_cast<char*>(data) + byte_offset);
}

template <typename TYPE>
void* GetTypedArrayData(v8::Local<v8::Value> value, size_t length = 0)
{
	if (value->IsNull()) { return NULL; }
	assert(value->IsTypedArray());
	v8::Local<v8::TypedArray> typed_array = value.As<v8::TypedArray>();
	if (!typed_array->HasBuffer())
	{
		value.As<v8::Object>()->Get(NANX_SYMBOL("buffer"));
		assert(typed_array->HasBuffer());
	}
	v8::Local<v8::ArrayBuffer> array_buffer = typed_array->Buffer();
	assert((length == 0) || (length == typed_array->Length()));
	void* data = array_buffer->GetContents().Data();
	size_t byte_offset = typed_array->ByteOffset();
	return static_cast<void*>(static_cast<char*>(data) + byte_offset);
}

  int8_t* GetInt8ArrayData   (v8::Local<v8::Value> value, size_t length = 0) { return static_cast<  int8_t*>(GetTypedArrayData<v8::Int8Array   >(value, length)); }
 uint8_t* GetUint8ArrayData  (v8::Local<v8::Value> value, size_t length = 0) { return static_cast< uint8_t*>(GetTypedArrayData<v8::Uint8Array  >(value, length)); }
 int16_t* GetInt16ArrayData  (v8::Local<v8::Value> value, size_t length = 0) { return static_cast< int16_t*>(GetTypedArrayData<v8::Int16Array  >(value, length)); }
uint16_t* GetUint16ArrayData (v8::Local<v8::Value> value, size_t length = 0) { return static_cast<uint16_t*>(GetTypedArrayData<v8::Uint16Array >(value, length)); }
 int32_t* GetInt32ArrayData  (v8::Local<v8::Value> value, size_t length = 0) { return static_cast< int32_t*>(GetTypedArrayData<v8::Int32Array  >(value, length)); }
uint32_t* GetUint32ArrayData (v8::Local<v8::Value> value, size_t length = 0) { return static_cast<uint32_t*>(GetTypedArrayData<v8::Uint32Array >(value, length)); }
   float* GetFloat32ArrayData(v8::Local<v8::Value> value, size_t length = 0) { return static_cast<   float*>(GetTypedArrayData<v8::Float32Array>(value, length)); }
  double* GetFloat64ArrayData(v8::Local<v8::Value> value, size_t length = 0) { return static_cast<  double*>(GetTypedArrayData<v8::Float64Array>(value, length)); }

#else

void* GetArrayBufferViewData(v8::Local<v8::Value> value, size_t byte_length = 0)
{
	if (value->IsNull()) { return NULL; }
	assert(value->IsObject());
	v8::Local<v8::Object> object = value.As<v8::Object>();
	#if NODE_VERSION_AT_LEAST(0, 12, 0)
	object->Get(NANX_SYMBOL("buffer"));
	#endif
	assert(object->HasIndexedPropertiesInExternalArrayData());
	assert((byte_length == 0) || (byte_length <= object->GetIndexedPropertiesExternalArrayDataLength()));
	return object->GetIndexedPropertiesExternalArrayData();
}

template <v8::ExternalArrayType TYPE>
void* GetTypedArrayData(v8::Local<v8::Value> value, size_t length = 0)
{
	if (value->IsNull()) { return NULL; }
	assert(value->IsObject());
	v8::Local<v8::Object> object = value.As<v8::Object>();
	#if NODE_VERSION_AT_LEAST(0, 12, 0)
	object->Get(NANX_SYMBOL("buffer"));
	#endif
	assert(object->HasIndexedPropertiesInExternalArrayData());
	assert((length == 0) || (length <= object->GetIndexedPropertiesExternalArrayDataLength()));
	assert(TYPE == object->GetIndexedPropertiesExternalArrayDataType());
	return object->GetIndexedPropertiesExternalArrayData();
}

  int8_t* GetInt8ArrayData   (v8::Local<v8::Value> value, size_t length = 0) { return static_cast<  int8_t*>(GetTypedArrayData<v8::kExternalInt8Array   >(value, length)); }
 uint8_t* GetUint8ArrayData  (v8::Local<v8::Value> value, size_t length = 0) { return static_cast< uint8_t*>(GetTypedArrayData<v8::kExternalUint8Array  >(value, length)); }
 int16_t* GetInt16ArrayData  (v8::Local<v8::Value> value, size_t length = 0) { return static_cast< int16_t*>(GetTypedArrayData<v8::kExternalInt16Array  >(value, length)); }
uint16_t* GetUint16ArrayData (v8::Local<v8::Value> value, size_t length = 0) { return static_cast<uint16_t*>(GetTypedArrayData<v8::kExternalUint16Array >(value, length)); }
 int32_t* GetInt32ArrayData  (v8::Local<v8::Value> value, size_t length = 0) { return static_cast< int32_t*>(GetTypedArrayData<v8::kExternalInt32Array  >(value, length)); }
uint32_t* GetUint32ArrayData (v8::Local<v8::Value> value, size_t length = 0) { return static_cast<uint32_t*>(GetTypedArrayData<v8::kExternalUint32Array >(value, length)); }
   float* GetFloat32ArrayData(v8::Local<v8::Value> value, size_t length = 0) { return static_cast<   float*>(GetTypedArrayData<v8::kExternalFloat32Array>(value, length)); }
  double* GetFloat64ArrayData(v8::Local<v8::Value> value, size_t length = 0) { return static_cast<  double*>(GetTypedArrayData<v8::kExternalFloat64Array>(value, length)); }

#endif

namespace node_gles2 {

#ifdef HAVE_OPENGLES2

//void glActiveTexture(GLenum texture);
NANX_EXPORT(glActiveTexture)
{
	GLenum texture = NANX_GLenum(info[0]);
	::glActiveTexture(texture);
}

//void glAttachShader(GLuint program, GLuint shader);
NANX_EXPORT(glAttachShader)
{
	GLuint program = NANX_GLuint(info[0]);
	GLuint shader = NANX_GLuint(info[1]);
	::glAttachShader(program, shader);
}

//void glBindAttribLocation(GLuint program, GLuint index, const GLchar* name);
NANX_EXPORT(glBindAttribLocation)
{
	GLuint program = NANX_GLuint(info[0]);
	GLuint index = NANX_GLuint(info[1]);
	v8::Local<v8::String> name = v8::Local<v8::String>::Cast(info[2]);
	::glBindAttribLocation(program, index, *v8::String::Utf8Value(name));
}

//void glBindBuffer(GLenum target, GLuint buffer);
NANX_EXPORT(glBindBuffer)
{
	GLenum target = NANX_GLenum(info[0]);
	GLuint buffer = NANX_GLuint(info[1]);
	::glBindBuffer(target, buffer);
}

//void glBindFramebuffer(GLenum target, GLuint framebuffer);
NANX_EXPORT(glBindFramebuffer)
{
	GLenum target = NANX_GLenum(info[0]);
	GLuint framebuffer = NANX_GLuint(info[1]);
	::glBindFramebuffer(target, framebuffer);
}

//void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
NANX_EXPORT(glBindRenderbuffer)
{
	GLenum target = NANX_GLenum(info[0]);
	GLuint renderbuffer = NANX_GLuint(info[1]);
	::glBindRenderbuffer(target, renderbuffer);
}

//void glBindTexture(GLenum target, GLuint texture);
NANX_EXPORT(glBindTexture)
{
	GLenum target = NANX_GLenum(info[0]);
	GLuint texture = NANX_GLuint(info[1]);
	::glBindTexture(target, texture);
}

//void glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
NANX_EXPORT(glBlendColor)
{
	GLclampf red = NANX_GLclampf(info[0]);
	GLclampf green = NANX_GLclampf(info[1]);
	GLclampf blue = NANX_GLclampf(info[2]);
	GLclampf alpha = NANX_GLclampf(info[3]);
	::glBlendColor(red, green, blue, alpha);
}

//void glBlendEquation(GLenum mode);
NANX_EXPORT(glBlendEquation)
{
	GLenum mode = NANX_GLenum(info[0]);
	::glBlendEquation(mode);
}

//void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
NANX_EXPORT(glBlendEquationSeparate)
{
	GLenum modeRGB = NANX_GLenum(info[0]);
	GLenum modeAlpha = NANX_GLenum(info[1]);
	::glBlendEquationSeparate(modeRGB, modeAlpha);
}

//void glBlendFunc(GLenum sfactor, GLenum dfactor);
NANX_EXPORT(glBlendFunc)
{
	GLenum sfactor = NANX_GLenum(info[0]);
	GLenum dfactor = NANX_GLenum(info[1]);
	::glBlendFunc(sfactor, dfactor);
}

//void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
NANX_EXPORT(glBlendFuncSeparate)
{
	GLenum srcRGB = NANX_GLenum(info[0]);
	GLenum dstRGB = NANX_GLenum(info[1]);
	GLenum srcAlpha = NANX_GLenum(info[2]);
	GLenum dstAlpha = NANX_GLenum(info[3]);
	::glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
}

//void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
NANX_EXPORT(glBufferData)
{
	GLenum target = NANX_GLenum(info[0]);
	GLsizeiptr size = NANX_GLsizeiptr(info[1]);
	const GLvoid* data = GetArrayBufferViewData(info[2], size);
	GLenum usage = NANX_GLenum(info[3]);
	::glBufferData(target, size, data, usage);
}

//void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
NANX_EXPORT(glBufferSubData)
{
	GLenum target = NANX_GLenum(info[0]);
	GLintptr offset = NANX_GLintptr(info[1]);
	GLsizeiptr size = NANX_GLsizeiptr(info[2]);
	const GLvoid* data = GetArrayBufferViewData(info[3], size);
	::glBufferSubData(target, offset, size, data);
}

//GLenum glCheckFramebufferStatus(GLenum target);
NANX_EXPORT(glCheckFramebufferStatus)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum status = ::glCheckFramebufferStatus(target);
	info.GetReturnValue().Set(Nan::New(status));
}

//void glClear(GLbitfield mask);
NANX_EXPORT(glClear)
{
	GLbitfield mask = NANX_GLbitfield(info[0]);
	::glClear(mask);
}

//void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
NANX_EXPORT(glClearColor)
{
	GLclampf red = NANX_GLclampf(info[0]);
	GLclampf green = NANX_GLclampf(info[1]);
	GLclampf blue = NANX_GLclampf(info[2]);
	GLclampf alpha = NANX_GLclampf(info[3]);
	::glClearColor(red, green, blue, alpha);
}

//void glClearDepthf(GLclampf depth);
NANX_EXPORT(glClearDepthf)
{
	GLclampf depth = NANX_GLclampf(info[0]);
	::glClearDepthf(depth);
}

//void glClearStencil(GLint s);
NANX_EXPORT(glClearStencil)
{
	GLint s = NANX_GLint(info[0]);
	::glClearStencil(s);
}

//void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
NANX_EXPORT(glColorMask)
{
	GLboolean red = NANX_GLboolean(info[0]);
	GLboolean green = NANX_GLboolean(info[1]);
	GLboolean blue = NANX_GLboolean(info[2]);
	GLboolean alpha = NANX_GLboolean(info[3]);
	::glColorMask(red, green, blue, alpha);
}

//void glCompileShader(GLuint shader);
NANX_EXPORT(glCompileShader)
{
	GLuint shader = NANX_GLuint(info[0]);
	::glCompileShader(shader);
}

//void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data);
NANX_EXPORT(glCompressedTexImage2D)
{
	GLenum target = NANX_GLenum(info[0]);
	GLint level = NANX_GLint(info[1]);
	GLenum internalformat = NANX_GLenum(info[2]);
	GLsizei width = NANX_GLsizei(info[3]);
	GLsizei height = NANX_GLsizei(info[4]);
	GLint border = NANX_GLint(info[5]);
	GLsizei imageSize = NANX_GLsizei(info[6]);
	const GLvoid* data = GetArrayBufferViewData(info[7], imageSize);
	::glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
}

//void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data);
NANX_EXPORT(glCompressedTexSubImage2D)
{
	GLenum target = NANX_GLenum(info[0]);
	GLint level = NANX_GLint(info[1]);
	GLint xoffset = NANX_GLint(info[2]);
	GLint yoffset = NANX_GLint(info[3]);
	GLsizei width = NANX_GLsizei(info[4]);
	GLsizei height = NANX_GLsizei(info[5]);
	GLenum format = NANX_GLenum(info[6]);
	GLsizei imageSize = NANX_GLsizei(info[7]);
	const GLvoid* data = GetArrayBufferViewData(info[8], imageSize);
	::glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

//void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
NANX_EXPORT(glCopyTexImage2D)
{
	GLenum target = NANX_GLenum(info[0]);
	GLint level = NANX_GLint(info[1]);
	GLenum internalformat = NANX_GLenum(info[2]);
	GLint x = NANX_GLint(info[3]);
	GLint y = NANX_GLint(info[4]);
	GLsizei width = NANX_GLsizei(info[5]);
	GLsizei height = NANX_GLsizei(info[6]);
	GLint border = NANX_GLint(info[7]);
	::glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
}

//void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
NANX_EXPORT(glCopyTexSubImage2D)
{
	GLenum target = NANX_GLenum(info[0]);
	GLint level = NANX_GLint(info[1]);
	GLint xoffset = NANX_GLint(info[2]);
	GLint yoffset = NANX_GLint(info[3]);
	GLint x = NANX_GLint(info[4]);
	GLint y = NANX_GLint(info[5]);
	GLsizei width = NANX_GLsizei(info[6]);
	GLsizei height = NANX_GLsizei(info[7]);
	::glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}

//GLuint glCreateProgram(void);
NANX_EXPORT(glCreateProgram)
{
	GLuint program = ::glCreateProgram();
	info.GetReturnValue().Set(Nan::New(program));
}

//GLuint glCreateShader(GLenum type);
NANX_EXPORT(glCreateShader)
{
	GLenum type = NANX_GLenum(info[0]);
	GLuint shader = ::glCreateShader(type);
	info.GetReturnValue().Set(Nan::New(shader));
}

//void glCullFace(GLenum mode);
NANX_EXPORT(glCullFace)
{
	GLenum mode = NANX_GLenum(info[0]);
	::glCullFace(mode);
}

//void glDeleteBuffers(GLsizei n, const GLuint* buffers);
NANX_EXPORT(glDeleteBuffers)
{
	GLsizei n = NANX_GLsizei(info[0]);
	GLuint* buffers = (GLuint*) GetUint32ArrayData(info[1], n);
	::glDeleteBuffers(n, buffers);
}

//void glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers);
NANX_EXPORT(glDeleteFramebuffers)
{
	GLsizei n = NANX_GLsizei(info[0]);
	GLuint* framebuffers = (GLuint*) GetUint32ArrayData(info[1], n);
	::glDeleteFramebuffers(n, framebuffers);
}

//void glDeleteProgram(GLuint program);
NANX_EXPORT(glDeleteProgram)
{
	GLuint program = NANX_GLuint(info[0]);
	::glDeleteProgram(program);
}

//void glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers);
NANX_EXPORT(glDeleteRenderbuffers)
{
	GLsizei n = NANX_GLsizei(info[0]);
	GLuint* renderbuffers = (GLuint*) GetUint32ArrayData(info[1], n);
	::glDeleteRenderbuffers(n, renderbuffers);
}

//void glDeleteShader(GLuint shader);
NANX_EXPORT(glDeleteShader)
{
	GLuint shader = NANX_GLuint(info[0]);
	::glDeleteShader(shader);
}

//void glDeleteTextures(GLsizei n, const GLuint* textures);
NANX_EXPORT(glDeleteTextures)
{
	GLsizei n = NANX_GLsizei(info[0]);
	GLuint* textures = (GLuint*) GetUint32ArrayData(info[1], n);
	::glDeleteTextures(n, textures);
}

//void glDepthFunc(GLenum func);
NANX_EXPORT(glDepthFunc)
{
	GLenum func = NANX_GLenum(info[0]);
	::glDepthFunc(func);
}

//void glDepthMask(GLboolean flag);
NANX_EXPORT(glDepthMask)
{
	GLboolean flag = NANX_GLboolean(info[0]);
	::glDepthMask(flag);
}

//void glDepthRangef(GLclampf zNear, GLclampf zFar);
NANX_EXPORT(glDepthRangef)
{
	GLclampf zNear = NANX_GLclampf(info[0]);
	GLclampf zFar = NANX_GLclampf(info[1]);
	::glDepthRangef(zNear, zFar);
}

//void glDetachShader(GLuint program, GLuint shader);
NANX_EXPORT(glDetachShader)
{
	GLuint program = NANX_GLuint(info[0]);
	GLuint shader = NANX_GLuint(info[1]);
	::glDetachShader(program, shader);
}

//void glDisable(GLenum cap);
NANX_EXPORT(glDisable)
{
	GLenum cap = NANX_GLenum(info[0]);
	::glDisable(cap);
}

//void glDisableVertexAttribArray(GLuint index);
NANX_EXPORT(glDisableVertexAttribArray)
{
	GLuint index = NANX_GLuint(info[0]);
	::glDisableVertexAttribArray(index);
}

//void glDrawArrays(GLenum mode, GLint first, GLsizei count);
NANX_EXPORT(glDrawArrays)
{
	GLenum mode = NANX_GLenum(info[0]);
	GLint first = NANX_GLint(info[1]);
	GLsizei count = NANX_GLsizei(info[2]);
	::glDrawArrays(mode, first, count);
}

//void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
NANX_EXPORT(glDrawElements)
{
	GLenum mode = NANX_GLenum(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	GLenum type = NANX_GLenum(info[2]);
	const GLvoid* indices = (const GLvoid*) info[3]->IntegerValue(); // TODO
	::glDrawElements(mode, count, type, indices);
}

//void glEnable(GLenum cap);
NANX_EXPORT(glEnable)
{
	GLenum cap = NANX_GLenum(info[0]);
	::glEnable(cap);
}

//void glEnableVertexAttribArray(GLuint index);
NANX_EXPORT(glEnableVertexAttribArray)
{
	GLuint index = NANX_GLuint(info[0]);
	::glEnableVertexAttribArray(index);
}

//void glFinish(void);
NANX_EXPORT(glFinish)
{
	::glFinish();
}

//void glFlush(void);
NANX_EXPORT(glFlush)
{
	::glFlush();
}

//void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
NANX_EXPORT(glFramebufferRenderbuffer)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum attachment = NANX_GLenum(info[1]);
	GLenum renderbuffertarget = NANX_GLenum(info[2]);
	GLuint renderbuffer = NANX_GLuint(info[3]);
	::glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

//void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
NANX_EXPORT(glFramebufferTexture2D)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum attachment = NANX_GLenum(info[1]);
	GLenum textarget = NANX_GLenum(info[2]);
	GLuint texture = NANX_GLuint(info[3]);
	GLint level = NANX_GLint(info[4]);
	::glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

//void glFrontFace(GLenum mode);
NANX_EXPORT(glFrontFace)
{
	GLenum mode = NANX_GLenum(info[0]);
	::glFrontFace(mode);
}

//void glGenBuffers(GLsizei n, GLuint* buffers);
NANX_EXPORT(glGenBuffers)
{
	GLsizei n = NANX_GLsizei(info[0]);
	GLuint* buffers = (GLuint*) GetUint32ArrayData(info[1], n);
	::glGenBuffers(n, buffers);
}

//void glGenerateMipmap(GLenum target);
NANX_EXPORT(glGenerateMipmap)
{
	GLenum target = NANX_GLenum(info[0]);
	::glGenerateMipmap(target);
}

//void glGenFramebuffers(GLsizei n, GLuint* framebuffers);
NANX_EXPORT(glGenFramebuffers)
{
	GLsizei n = NANX_GLsizei(info[0]);
	GLuint* framebuffers = (GLuint*) GetUint32ArrayData(info[1], n);
	::glGenFramebuffers(n, framebuffers);
}

//void glGenRenderbuffers(GLsizei n, GLuint* renderbuffers);
NANX_EXPORT(glGenRenderbuffers)
{
	GLsizei n = NANX_GLsizei(info[0]);
	GLuint* renderbuffers = (GLuint*) GetUint32ArrayData(info[1], n);
	::glGenRenderbuffers(n, renderbuffers);
}

//void glGenTextures(GLsizei n, GLuint* textures);
NANX_EXPORT(glGenTextures)
{
	GLsizei n = NANX_GLsizei(info[0]);
	GLuint* textures = (GLuint*) GetUint32ArrayData(info[1], n);
	::glGenTextures(n, textures);
}

//void glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
NANX_EXPORT(glGetActiveAttrib)
{
	GLuint program = NANX_GLuint(info[0]);
	GLuint index = NANX_GLuint(info[1]);
	GLsizei bufsize = NANX_GLsizei(info[2]);
	GLint* length = (GLint*) GetInt32ArrayData(info[3], 1);
	GLint* size = (GLint*) GetInt32ArrayData(info[4], 1);
	GLenum* type = (GLenum*) GetUint32ArrayData(info[5], 1);
	v8::Local<v8::Array> _name = v8::Local<v8::Array>::Cast(info[6]);
	const size_t s_name_count = 64;
	static GLchar s_name[s_name_count];
	GLchar* name = ((size_t) bufsize <= s_name_count)?(s_name):((GLchar*) malloc(bufsize * sizeof(GLchar)));
	::glGetActiveAttrib(program, index, bufsize, length, size, type, name);
	_name->Set(0, NANX_STRING(name));
	if (name != s_name) { free(name); }
}

//void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
NANX_EXPORT(glGetActiveUniform)
{
	GLuint program = NANX_GLuint(info[0]);
	GLuint index = NANX_GLuint(info[1]);
	GLsizei bufsize = NANX_GLsizei(info[2]);
	GLint* length = (GLint*) GetInt32ArrayData(info[3], 1);
	GLint* size = (GLint*) GetInt32ArrayData(info[4], 1);
	GLenum* type = (GLenum*) GetUint32ArrayData(info[5], 1);
	v8::Local<v8::Array> _name = v8::Local<v8::Array>::Cast(info[6]);
	const size_t s_name_count = 64;
	static GLchar s_name[s_name_count];
	GLchar* name = ((size_t) bufsize <= s_name_count)?(s_name):((GLchar*) malloc(bufsize * sizeof(GLchar)));
	::glGetActiveUniform(program, index, bufsize, length, size, type, name);
	_name->Set(0, NANX_STRING(name));
	if (name != s_name) { free(name); }
}

//void glGetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
NANX_EXPORT(glGetAttachedShaders)
{
	GLuint program = NANX_GLuint(info[0]);
	GLsizei maxcount = NANX_GLsizei(info[1]);
	GLsizei* count = (GLsizei*) GetUint32ArrayData(info[2], 1);
	GLuint* shaders = (GLuint*) GetUint32ArrayData(info[3], maxcount);
	::glGetAttachedShaders(program, maxcount, count, shaders);
}

//GLint glGetAttribLocation(GLuint program, const GLchar* name);
NANX_EXPORT(glGetAttribLocation)
{
	GLuint program = NANX_GLuint(info[0]);
	v8::Local<v8::String> name = v8::Local<v8::String>::Cast(info[1]);
	GLint location = ::glGetAttribLocation(program, *v8::String::Utf8Value(name));
	info.GetReturnValue().Set(Nan::New(location));
}

//void glGetBooleanv(GLenum pname, GLboolean* params);
NANX_EXPORT(glGetBooleanv)
{
	GLenum pname = NANX_GLenum(info[0]);
	GLboolean* params = static_cast<GLboolean*>(GetUint8ArrayData(info[1])); // TODO: length
	::glGetBooleanv(pname, params);
}

//void glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params);
NANX_EXPORT(glGetBufferParameteriv)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	GLint* params = static_cast<GLint*>(GetInt32ArrayData(info[2])); // TODO: length
	::glGetBufferParameteriv(target, pname, params);
}

//GLenum glGetError(void);
NANX_EXPORT(glGetError)
{
	GLenum gl_error = ::glGetError();
	info.GetReturnValue().Set(Nan::New(gl_error));
}

//void glGetFloatv(GLenum pname, GLfloat* params);
NANX_EXPORT(glGetFloatv)
{
	GLenum pname = NANX_GLenum(info[0]);
	GLfloat* params = static_cast<GLfloat*>(GetFloat32ArrayData(info[1])); // TODO: length
	::glGetFloatv(pname, params);
}

//void glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params);
NANX_EXPORT(glGetFramebufferAttachmentParameteriv)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum attachment = NANX_GLenum(info[1]);
	GLenum pname = NANX_GLenum(info[2]);
	GLint* params = static_cast<GLint*>(GetInt32ArrayData(info[3])); // TODO: length
	::glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
}

//void glGetIntegerv(GLenum pname, GLint* params);
NANX_EXPORT(glGetIntegerv)
{
	GLenum pname = NANX_GLenum(info[0]);
	GLint* params = static_cast<GLint*>(GetInt32ArrayData(info[1])); // TODO: length
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

}

//void glGetProgramiv(GLuint program, GLenum pname, GLint* params);
NANX_EXPORT(glGetProgramiv)
{
	GLuint program = NANX_GLuint(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	GLint* params = static_cast<GLint*>(GetInt32ArrayData(info[2])); // TODO: length
	::glGetProgramiv(program, pname, params);
}

//void glGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog);
NANX_EXPORT(glGetProgramInfoLog)
{
	GLuint program = NANX_GLuint(info[0]);
	GLsizei bufsize = NANX_GLsizei(info[1]);
	GLsizei* length = static_cast<GLsizei*>(GetInt32ArrayData(info[2], 1));
	v8::Local<v8::Array> _infolog = v8::Local<v8::Array>::Cast(info[3]);
	const size_t s_infolog_count = 64;
	static GLchar s_infolog[s_infolog_count];
	GLchar* infolog = ((size_t) bufsize <= s_infolog_count)?(s_infolog):(static_cast<GLchar*>(malloc(bufsize * sizeof(GLchar))));
	::glGetProgramInfoLog(program, bufsize, length, infolog);
	_infolog->Set(0, NANX_STRING(infolog));
	if (infolog != s_infolog) { free(infolog); }
}

//void glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params);
NANX_EXPORT(glGetRenderbufferParameteriv)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	GLint* params = static_cast<GLint*>(GetInt32ArrayData(info[2])); // TODO: length
	::glGetRenderbufferParameteriv(target, pname, params);
}

//void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
NANX_EXPORT(glGetShaderiv)
{
	GLuint shader = NANX_GLuint(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	GLint* params = static_cast<GLint*>(GetInt32ArrayData(info[2])); // TODO: length
	::glGetShaderiv(shader, pname, params);
}

//void glGetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog);
NANX_EXPORT(glGetShaderInfoLog)
{
	GLuint shader = NANX_GLuint(info[0]);
	GLsizei bufsize = NANX_GLsizei(info[1]);
	GLsizei* length = static_cast<GLsizei*>(GetInt32ArrayData(info[2], 1));
	v8::Local<v8::Array> _infolog = v8::Local<v8::Array>::Cast(info[3]);
	const size_t s_infolog_count = 64;
	static GLchar s_infolog[s_infolog_count];
	GLchar* infolog = ((size_t) bufsize <= s_infolog_count)?(s_infolog):(static_cast<GLchar*>(malloc(bufsize * sizeof(GLchar))));
	::glGetShaderInfoLog(shader, bufsize, length, infolog);
	_infolog->Set(0, NANX_STRING(infolog));
	if (infolog != s_infolog) { free(infolog); }
}

//void glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
NANX_EXPORT(glGetShaderPrecisionFormat)
{
	GLenum shadertype = NANX_GLenum(info[0]);
	GLenum precisiontype = NANX_GLenum(info[1]);
	v8::Local<v8::Array> _range = v8::Local<v8::Array>::Cast(info[2]);
	v8::Local<v8::Array> _precision = v8::Local<v8::Array>::Cast(info[3]);
	GLsizei range[2] = { static_cast<GLsizei>(_range->Get(0)->Uint32Value()), static_cast<GLsizei>(_range->Get(1)->Uint32Value()) };
	GLint precision = _precision->Get(0)->Int32Value();
	::glGetShaderPrecisionFormat(shadertype, precisiontype, range, &precision);
	_range->Set(0, Nan::New(range[0]));
	_range->Set(1, Nan::New(range[1]));
	_precision->Set(0, Nan::New(precision));
}

//void glGetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source);
NANX_EXPORT(glGetShaderSource)
{
	GLuint shader = NANX_GLuint(info[0]);
	GLsizei bufsize = NANX_GLsizei(info[1]);
	GLsizei* length = static_cast<GLsizei*>(GetInt32ArrayData(info[2], 1));
	v8::Local<v8::Array> _source = v8::Local<v8::Array>::Cast(info[3]);
	const size_t s_source_count = 256;
	static GLchar s_source[s_source_count];
	GLchar* source = ((size_t) bufsize <= s_source_count)?(s_source):(static_cast<GLchar*>(malloc(bufsize * sizeof(GLchar))));
	::glGetShaderSource(shader, bufsize, length, source);
	_source->Set(0, NANX_STRING(source));
	if (source != s_source) { free(source); }
}

//const GLubyte* glGetString(GLenum name);
NANX_EXPORT(glGetString)
{
	GLenum name = NANX_GLenum(info[0]);
	const GLubyte* gl_string = ::glGetString(name);
	if (gl_string != 0)
	{
		info.GetReturnValue().Set(NANX_STRING(reinterpret_cast<const char*>(gl_string)));
		return;
	}
}

//void glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params);
NANX_EXPORT(glGetTexParameterfv)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	GLfloat* params = static_cast<GLfloat*>(GetFloat32ArrayData(info[2])); // TODO: length
	::glGetTexParameterfv(target, pname, params);
}

//void glGetTexParameteriv(GLenum target, GLenum pname, GLint* params);
NANX_EXPORT(glGetTexParameteriv)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	GLint* params = static_cast<GLint*>(GetInt32ArrayData(info[2])); // TODO: length
	::glGetTexParameteriv(target, pname, params);
}

//void glGetUniformfv(GLuint program, GLint location, GLfloat* params);
NANX_EXPORT(glGetUniformfv)
{
	GLuint program = NANX_GLuint(info[0]);
	GLint location = NANX_GLint(info[1]);
	GLfloat* params = static_cast<GLfloat*>(GetFloat32ArrayData(info[2])); // TODO: length
	::glGetUniformfv(program, location, params);
}

//void glGetUniformiv(GLuint program, GLint location, GLint* params);
NANX_EXPORT(glGetUniformiv)
{
	GLuint program = NANX_GLuint(info[0]);
	GLint location = NANX_GLint(info[1]);
	GLint* params = static_cast<GLint*>(GetInt32ArrayData(info[2])); // TODO: length
	::glGetUniformiv(program, location, params);
}

//GLint glGetUniformLocation(GLuint program, const GLchar* name);
NANX_EXPORT(glGetUniformLocation)
{
	GLuint program = NANX_GLuint(info[0]);
	v8::Local<v8::String> name = v8::Local<v8::String>::Cast(info[1]);
	GLint location = ::glGetUniformLocation(program, *v8::String::Utf8Value(name));
	info.GetReturnValue().Set(Nan::New(location));
}

//void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params);
NANX_EXPORT(glGetVertexAttribfv)
{
	GLuint index = NANX_GLuint(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	GLfloat* params = static_cast<GLfloat*>(GetFloat32ArrayData(info[2])); // TODO: length
	::glGetVertexAttribfv(index, pname, params);
}

//void glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params);
NANX_EXPORT(glGetVertexAttribiv)
{
	GLuint index = NANX_GLuint(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	GLint* params = static_cast<GLint*>(GetInt32ArrayData(info[2])); // TODO: length
	::glGetVertexAttribiv(index, pname, params);
}

//void glGetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid** pointer);
NANX_EXPORT(glGetVertexAttribPointerv)
{
	GLuint index = NANX_GLuint(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	v8::Local<v8::Array> _pointer = v8::Local<v8::Array>::Cast(info[2]);
	GLuint offset = 0;
	GLvoid* pointer = static_cast<GLvoid*>(&offset);
	::glGetVertexAttribPointerv(index, pname, &pointer);
	_pointer->Set(0, Nan::New(offset));
}

//void glHint(GLenum target, GLenum mode);
NANX_EXPORT(glHint)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum mode = NANX_GLenum(info[1]);
	::glHint(target, mode);
}

//GLboolean glIsBuffer(GLuint buffer);
NANX_EXPORT(glIsBuffer)
{
	GLuint buffer = NANX_GLuint(info[0]);
	GLboolean b = ::glIsBuffer(buffer);
	info.GetReturnValue().Set(Nan::New(b != GL_FALSE));
}

//GLboolean glIsEnabled(GLenum cap);
NANX_EXPORT(glIsEnabled)
{
	GLenum cap = NANX_GLenum(info[0]);
	GLboolean b = ::glIsEnabled(cap);
	info.GetReturnValue().Set(Nan::New(b != GL_FALSE));
}

//GLboolean glIsFramebuffer(GLuint framebuffer);
NANX_EXPORT(glIsFramebuffer)
{
	GLuint framebuffer = NANX_GLuint(info[0]);
	GLboolean b = ::glIsFramebuffer(framebuffer);
	info.GetReturnValue().Set(Nan::New(b != GL_FALSE));
}

//GLboolean glIsProgram(GLuint program);
NANX_EXPORT(glIsProgram)
{
	GLuint program = NANX_GLuint(info[0]);
	GLboolean b = ::glIsProgram(program);
	info.GetReturnValue().Set(Nan::New(b != GL_FALSE));
}

//GLboolean glIsRenderbuffer(GLuint renderbuffer);
NANX_EXPORT(glIsRenderbuffer)
{
	GLuint renderbuffer = NANX_GLuint(info[0]);
	GLboolean b = ::glIsRenderbuffer(renderbuffer);
	info.GetReturnValue().Set(Nan::New(b != GL_FALSE));
}

//GLboolean glIsShader(GLuint shader);
NANX_EXPORT(glIsShader)
{
	GLuint shader = NANX_GLuint(info[0]);
	GLboolean b = ::glIsShader(shader);
	info.GetReturnValue().Set(Nan::New(b != GL_FALSE));
}

//GLboolean glIsTexture(GLuint texture);
NANX_EXPORT(glIsTexture)
{
	GLuint texture = NANX_GLuint(info[0]);
	GLboolean b = ::glIsTexture(texture);
	info.GetReturnValue().Set(Nan::New(b != GL_FALSE));
}

//void glLineWidth(GLfloat width);
NANX_EXPORT(glLineWidth)
{
	GLfloat width = NANX_GLfloat(info[0]);
	::glLineWidth(width);
}

//void glLinkProgram(GLuint program);
NANX_EXPORT(glLinkProgram)
{
	GLuint program = NANX_GLuint(info[0]);
	::glLinkProgram(program);
}

//void glPixelStorei(GLenum pname, GLint param);
NANX_EXPORT(glPixelStorei)
{
	GLenum pname = NANX_GLenum(info[0]);
	GLint param = NANX_GLint(info[1]);
	::glPixelStorei(pname, param);
}

//void glPolygonOffset(GLfloat factor, GLfloat units);
NANX_EXPORT(glPolygonOffset)
{
	GLfloat factor = NANX_GLfloat(info[0]);
	GLfloat units = NANX_GLfloat(info[1]);
	::glPolygonOffset(factor, units);
}

//void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels);
NANX_EXPORT(glReadPixels)
{
	GLint x = NANX_GLint(info[0]);
	GLint y = NANX_GLint(info[1]);
	GLsizei width = NANX_GLsizei(info[2]);
	GLsizei height = NANX_GLsizei(info[3]);
	GLenum format = NANX_GLenum(info[4]);
	GLenum type = NANX_GLenum(info[5]);
	size_t byte_length = 0;
	switch (type)
	{
	case GL_UNSIGNED_BYTE:
		switch (format)
		{
		case GL_ALPHA:	byte_length = 1; break;
		case GL_RGB:	byte_length = 3; break;
		case GL_RGBA:	byte_length = 4; break;
		}
		break;
	case GL_UNSIGNED_SHORT_5_6_5:
	case GL_UNSIGNED_SHORT_4_4_4_4:
	case GL_UNSIGNED_SHORT_5_5_5_1:
		byte_length = 2;
		break;
	}
	byte_length *= width * height;
	GLvoid* pixels = GetArrayBufferViewData(info[6], byte_length);
	::glReadPixels(x, y, width, height, format, type, pixels);
}

//void glReleaseShaderCompiler(void);
NANX_EXPORT(glReleaseShaderCompiler)
{
	::glReleaseShaderCompiler();
}

//void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
NANX_EXPORT(glRenderbufferStorage)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum internalformat = NANX_GLenum(info[1]);
	GLsizei width = NANX_GLsizei(info[2]);
	GLsizei height = NANX_GLsizei(info[3]);
	::glRenderbufferStorage(target, internalformat, width, height);
}

//void glSampleCoverage(GLclampf value, GLboolean invert);
NANX_EXPORT(glSampleCoverage)
{
	GLclampf value = NANX_GLclampf(info[0]);
	GLboolean invert = NANX_GLboolean(info[1]);
	::glSampleCoverage(value, invert);
}

//void glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
NANX_EXPORT(glScissor)
{
	GLint x = NANX_GLint(info[0]);
	GLint y = NANX_GLint(info[1]);
	GLsizei width = NANX_GLsizei(info[2]);
	GLsizei height = NANX_GLsizei(info[3]);
	::glScissor(x, y, width, height);
}

//void glShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length);
NANX_EXPORT(glShaderBinary)
{
	GLsizei n = NANX_GLsizei(info[0]);
	const GLuint* shaders = (const GLuint*) GetUint32ArrayData(info[1], n);
	GLenum binaryformat = NANX_GLenum(info[2]);
	GLsizei length = NANX_GLsizei(info[4]);
	const GLvoid* binary = (const GLvoid*) GetUint32ArrayData(info[3], length);
	::glShaderBinary(n, shaders, binaryformat, binary, length);
}

//void glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
NANX_EXPORT(glShaderSource)
{
	GLuint shader = NANX_GLuint(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	v8::Local<v8::Array> _string = v8::Local<v8::Array>::Cast(info[2]);
	v8::Local<v8::Array> _length = v8::Local<v8::Array>::Cast(info[3]);
	GLchar** string = (GLchar**) malloc(count * sizeof(GLchar*));
	GLint* length = (GLint*) malloc(count * sizeof(GLint));
	for (int i = 0; i < count; ++i)
	{
		v8::Local<v8::String> str = v8::Local<v8::String>::Cast(_string->Get(i));
		v8::Local<v8::Integer> len = v8::Local<v8::Integer>::Cast(_length->Get(i));
		string[i] = strdup(*v8::String::Utf8Value(str));
		length[i] = len->Int32Value();
	}
	::glShaderSource(shader, count, (const GLchar**) string, length);
	for (int i = 0; i < count; ++i)
	{
		free(string[i]); // strdup
	}
	free(string);
	free(length);
}

//void glStencilFunc(GLenum func, GLint ref, GLuint mask);
NANX_EXPORT(glStencilFunc)
{
	GLenum func = NANX_GLenum(info[0]);
	GLint ref = NANX_GLint(info[1]);
	GLuint mask = NANX_GLuint(info[2]);
	::glStencilFunc(func, ref, mask);
}

//void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask);
NANX_EXPORT(glStencilFuncSeparate)
{
	GLenum face = NANX_GLenum(info[0]);
	GLenum func = NANX_GLenum(info[1]);
	GLint ref = NANX_GLint(info[2]);
	GLuint mask = NANX_GLuint(info[3]);
	::glStencilFuncSeparate(face, func, ref, mask);
}

//void glStencilMask(GLuint mask);
NANX_EXPORT(glStencilMask)
{
	GLuint mask = NANX_GLuint(info[0]);
	::glStencilMask(mask);
}

//void glStencilMaskSeparate(GLenum face, GLuint mask);
NANX_EXPORT(glStencilMaskSeparate)
{
	GLenum face = NANX_GLenum(info[0]);
	GLuint mask = NANX_GLuint(info[1]);
	::glStencilMaskSeparate(face, mask);
}

//void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass);
NANX_EXPORT(glStencilOp)
{
	GLenum fail = NANX_GLenum(info[0]);
	GLenum zfail = NANX_GLenum(info[1]);
	GLenum zpass = NANX_GLenum(info[2]);
	::glStencilOp(fail, zfail, zpass);
}

//void glStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
NANX_EXPORT(glStencilOpSeparate)
{
	GLenum face = NANX_GLenum(info[0]);
	GLenum fail = NANX_GLenum(info[1]);
	GLenum zfail = NANX_GLenum(info[2]);
	GLenum zpass = NANX_GLenum(info[3]);
	::glStencilOpSeparate(face, fail, zfail, zpass);
}

//void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
NANX_EXPORT(glTexImage2D)
{
	GLenum target = NANX_GLenum(info[0]);
	GLint level = NANX_GLint(info[1]);
	GLint internalformat = NANX_GLint(info[2]);
	GLsizei width = NANX_GLsizei(info[3]);
	GLsizei height = NANX_GLsizei(info[4]);
	GLint border = NANX_GLint(info[5]);
	GLenum format = NANX_GLenum(info[6]);
	GLenum type = NANX_GLenum(info[7]);
	const GLvoid* pixels = GetArrayBufferViewData(info[8]); // TODO: byte_length
	::glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels); // TODO
}

//void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
NANX_EXPORT(glTexParameterf)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	GLfloat param = NANX_GLfloat(info[2]);
	::glTexParameterf(target, pname, param);
}

//void glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params);
NANX_EXPORT(glTexParameterfv)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	const GLfloat* params = static_cast<const GLfloat*>(GetFloat32ArrayData(info[2])); // TODO: length
	::glTexParameterfv(target, pname, params);
}

//void glTexParameteri(GLenum target, GLenum pname, GLint param);
NANX_EXPORT(glTexParameteri)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	GLint param = NANX_GLint(info[2]);
	::glTexParameteri(target, pname, param);
}

//void glTexParameteriv(GLenum target, GLenum pname, const GLint* params);
NANX_EXPORT(glTexParameteriv)
{
	GLenum target = NANX_GLenum(info[0]);
	GLenum pname = NANX_GLenum(info[1]);
	const GLint* params = static_cast<const GLint*>(GetInt32ArrayData(info[2])); // TODO: length
	::glTexParameteriv(target, pname, params);
}

//void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels);
NANX_EXPORT(glTexSubImage2D)
{
	GLenum target = NANX_GLenum(info[0]);
	GLint level = NANX_GLint(info[1]);
	GLint xoffset = NANX_GLint(info[2]);
	GLint yoffset = NANX_GLint(info[3]);
	GLsizei width = NANX_GLsizei(info[4]);
	GLsizei height = NANX_GLsizei(info[5]);
	GLenum format = NANX_GLenum(info[6]);
	GLenum type = NANX_GLenum(info[7]);
	const GLvoid* pixels = GetArrayBufferViewData(info[8]); // TODO: byte_length
	::glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels); // TODO
}

//void glUniform1f(GLint location, GLfloat x);
NANX_EXPORT(glUniform1f)
{
	GLint location = NANX_GLint(info[0]);
	GLfloat x = NANX_GLfloat(info[1]);
	::glUniform1f(location, x);
}

//void glUniform1fv(GLint location, GLsizei count, const GLfloat* v);
NANX_EXPORT(glUniform1fv)
{
	GLint location = NANX_GLint(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	const GLfloat* v = (const GLfloat*) GetFloat32ArrayData(info[2], count);
	::glUniform1fv(location, count, v);
}

//void glUniform1i(GLint location, GLint x);
NANX_EXPORT(glUniform1i)
{
	GLint location = NANX_GLint(info[0]);
	GLint x = NANX_GLint(info[1]);
	::glUniform1i(location, x);
}

//void glUniform1iv(GLint location, GLsizei count, const GLint* v);
NANX_EXPORT(glUniform1iv)
{
	GLint location = NANX_GLint(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	const GLint* v = (const GLint*) GetInt32ArrayData(info[2], count);
	::glUniform1iv(location, count, v);
}

//void glUniform2f(GLint location, GLfloat x, GLfloat y);
NANX_EXPORT(glUniform2f)
{
	GLint location = NANX_GLint(info[0]);
	GLfloat x = NANX_GLfloat(info[1]);
	GLfloat y = NANX_GLfloat(info[2]);
	::glUniform2f(location, x, y);
}

//void glUniform2fv(GLint location, GLsizei count, const GLfloat* v);
NANX_EXPORT(glUniform2fv)
{
	GLint location = NANX_GLint(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	const GLfloat* v = (const GLfloat*) GetFloat32ArrayData(info[2], count * 2);
	::glUniform2fv(location, count, v);
}

//void glUniform2i(GLint location, GLint x, GLint y);
NANX_EXPORT(glUniform2i)
{
	GLint location = NANX_GLint(info[0]);
	GLint x = NANX_GLint(info[1]);
	GLint y = NANX_GLint(info[2]);
	::glUniform2i(location, x, y);
}

//void glUniform2iv(GLint location, GLsizei count, const GLint* v);
NANX_EXPORT(glUniform2iv)
{
	GLint location = NANX_GLint(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	const GLint* v = (const GLint*) GetInt32ArrayData(info[2], count * 2);
	::glUniform2iv(location, count, v);
}

//void glUniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z);
NANX_EXPORT(glUniform3f)
{
	GLint location = NANX_GLint(info[0]);
	GLfloat x = NANX_GLfloat(info[1]);
	GLfloat y = NANX_GLfloat(info[2]);
	GLfloat z = NANX_GLfloat(info[3]);
	::glUniform3f(location, x, y, z);
}

//void glUniform3fv(GLint location, GLsizei count, const GLfloat* v);
NANX_EXPORT(glUniform3fv)
{
	GLint location = NANX_GLint(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	const GLfloat* v = (const GLfloat*) GetFloat32ArrayData(info[2], count * 3);
	::glUniform3fv(location, count, v);
}

//void glUniform3i(GLint location, GLint x, GLint y, GLint z);
NANX_EXPORT(glUniform3i)
{
	GLint location = NANX_GLint(info[0]);
	GLint x = NANX_GLint(info[1]);
	GLint y = NANX_GLint(info[2]);
	GLint z = NANX_GLint(info[3]);
	::glUniform3i(location, x, y, z);
}

//void glUniform3iv(GLint location, GLsizei count, const GLint* v);
NANX_EXPORT(glUniform3iv)
{
	GLint location = NANX_GLint(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	const GLint* v = (const GLint*) GetInt32ArrayData(info[2], count * 3);
	::glUniform3iv(location, count, v);
}

//void glUniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
NANX_EXPORT(glUniform4f)
{
	GLint location = NANX_GLint(info[0]);
	GLfloat x = NANX_GLfloat(info[1]);
	GLfloat y = NANX_GLfloat(info[2]);
	GLfloat z = NANX_GLfloat(info[3]);
	GLfloat w = NANX_GLfloat(info[4]);
	::glUniform4f(location, x, y, z, w);
}

//void glUniform4fv(GLint location, GLsizei count, const GLfloat* v);
NANX_EXPORT(glUniform4fv)
{
	GLint location = NANX_GLint(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	const GLfloat* v = (const GLfloat*) GetFloat32ArrayData(info[2], count * 4);
	::glUniform4fv(location, count, v);
}

//void glUniform4i(GLint location, GLint x, GLint y, GLint z, GLint w);
NANX_EXPORT(glUniform4i)
{
	GLint location = NANX_GLint(info[0]);
	GLint x = NANX_GLint(info[1]);
	GLint y = NANX_GLint(info[2]);
	GLint z = NANX_GLint(info[3]);
	GLint w = NANX_GLint(info[4]);
	::glUniform4i(location, x, y, z, w);
}

//void glUniform4iv(GLint location, GLsizei count, const GLint* v);
NANX_EXPORT(glUniform4iv)
{
	GLint location = NANX_GLint(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	const GLint* v = (const GLint*) GetInt32ArrayData(info[2], count * 4);
	::glUniform4iv(location, count, v);
}

//void glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
NANX_EXPORT(glUniformMatrix2fv)
{
	GLint location = NANX_GLint(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	GLboolean transpose = NANX_GLboolean(info[2]);
	const GLfloat* value = (const GLfloat*) GetFloat32ArrayData(info[3], count * 4);
	::glUniformMatrix2fv(location, count, transpose, value);
}

//void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
NANX_EXPORT(glUniformMatrix3fv)
{
	GLint location = NANX_GLint(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	GLboolean transpose = NANX_GLboolean(info[2]);
	const GLfloat* value = (const GLfloat*) GetFloat32ArrayData(info[3], count * 9);
	::glUniformMatrix3fv(location, count, transpose, value);
}

//void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
NANX_EXPORT(glUniformMatrix4fv)
{
	GLint location = NANX_GLint(info[0]);
	GLsizei count = NANX_GLsizei(info[1]);
	GLboolean transpose = NANX_GLboolean(info[2]);
	const GLfloat* value = (const GLfloat*) GetFloat32ArrayData(info[3], count * 16);
	::glUniformMatrix4fv(location, count, transpose, value);
}

//void glUseProgram(GLuint program);
NANX_EXPORT(glUseProgram)
{
	GLuint program = NANX_GLuint(info[0]);
	::glUseProgram(program);
}

//void glValidateProgram(GLuint program);
NANX_EXPORT(glValidateProgram)
{
	GLuint program = NANX_GLuint(info[0]);
	::glValidateProgram(program);
}

//void glVertexAttrib1f(GLuint indx, GLfloat x);
NANX_EXPORT(glVertexAttrib1f)
{
	GLuint indx = NANX_GLuint(info[0]);
	GLfloat x = NANX_GLfloat(info[1]);
	::glVertexAttrib1f(indx, x);
}

//void glVertexAttrib1fv(GLuint indx, const GLfloat* values);
NANX_EXPORT(glVertexAttrib1fv)
{
	GLuint indx = NANX_GLuint(info[0]);
	const GLfloat* values = (const GLfloat*) GetFloat32ArrayData(info[1], 1);
	::glVertexAttrib1fv(indx, values);
}

//void glVertexAttrib2f(GLuint indx, GLfloat x, GLfloat y);
NANX_EXPORT(glVertexAttrib2f)
{
	GLuint indx = NANX_GLuint(info[0]);
	GLfloat x = NANX_GLfloat(info[1]);
	GLfloat y = NANX_GLfloat(info[2]);
	::glVertexAttrib2f(indx, x, y);
}

//void glVertexAttrib2fv(GLuint indx, const GLfloat* values);
NANX_EXPORT(glVertexAttrib2fv)
{
	GLuint indx = NANX_GLuint(info[0]);
	const GLfloat* values = (const GLfloat*) GetFloat32ArrayData(info[1], 2);
	::glVertexAttrib2fv(indx, values);
}

//void glVertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z);
NANX_EXPORT(glVertexAttrib3f)
{
	GLuint indx = NANX_GLuint(info[0]);
	GLfloat x = NANX_GLfloat(info[1]);
	GLfloat y = NANX_GLfloat(info[2]);
	GLfloat z = NANX_GLfloat(info[3]);
	::glVertexAttrib3f(indx, x, y, z);
}

//void glVertexAttrib3fv(GLuint indx, const GLfloat* values);
NANX_EXPORT(glVertexAttrib3fv)
{
	GLuint indx = NANX_GLuint(info[0]);
	const GLfloat* values = (const GLfloat*) GetFloat32ArrayData(info[1], 3);
	::glVertexAttrib3fv(indx, values);
}

//void glVertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
NANX_EXPORT(glVertexAttrib4f)
{
	GLuint indx = NANX_GLuint(info[0]);
	GLfloat x = NANX_GLfloat(info[1]);
	GLfloat y = NANX_GLfloat(info[2]);
	GLfloat z = NANX_GLfloat(info[3]);
	GLfloat w = NANX_GLfloat(info[4]);
	::glVertexAttrib4f(indx, x, y, z, w);
}

//void glVertexAttrib4fv(GLuint indx, const GLfloat* values);
NANX_EXPORT(glVertexAttrib4fv)
{
	GLuint indx = NANX_GLuint(info[0]);
	const GLfloat* values = (const GLfloat*) GetFloat32ArrayData(info[1], 4);
	::glVertexAttrib4fv(indx, values);
}

//void glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr);
NANX_EXPORT(glVertexAttribPointer)
{
	GLuint indx = NANX_GLuint(info[0]);
	GLint size = NANX_GLint(info[1]);
	GLenum type = NANX_GLenum(info[2]);
	GLboolean normalized = NANX_GLboolean(info[3]);
	GLsizei stride = NANX_GLsizei(info[4]);
	GLuint offset = NANX_GLuint(info[5]);
	const GLvoid* ptr = (const GLvoid*)(((char*)(0)) + offset);
	::glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
}

//void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
NANX_EXPORT(glViewport)
{
	GLint x = NANX_GLint(info[0]);
	GLint y = NANX_GLint(info[1]);
	GLsizei width = NANX_GLsizei(info[2]);
	GLsizei height = NANX_GLsizei(info[3]);
	::glViewport(x, y, width, height);
}

#endif // HAVE_OPENGLES2

NAN_MODULE_INIT(init)
{

	#ifdef HAVE_OPENGLES2

//	NANX_CONSTANT(target, GL_ES_VERSION_2_0);
	NANX_CONSTANT(target, GL_DEPTH_BUFFER_BIT);
	NANX_CONSTANT(target, GL_STENCIL_BUFFER_BIT);
	NANX_CONSTANT(target, GL_COLOR_BUFFER_BIT);
	NANX_CONSTANT(target, GL_FALSE);
	NANX_CONSTANT(target, GL_TRUE);
	NANX_CONSTANT(target, GL_POINTS);
	NANX_CONSTANT(target, GL_LINES);
	NANX_CONSTANT(target, GL_LINE_LOOP);
	NANX_CONSTANT(target, GL_LINE_STRIP);
	NANX_CONSTANT(target, GL_TRIANGLES);
	NANX_CONSTANT(target, GL_TRIANGLE_STRIP);
	NANX_CONSTANT(target, GL_TRIANGLE_FAN);
	NANX_CONSTANT(target, GL_ZERO);
	NANX_CONSTANT(target, GL_ONE);
	NANX_CONSTANT(target, GL_SRC_COLOR);
	NANX_CONSTANT(target, GL_ONE_MINUS_SRC_COLOR);
	NANX_CONSTANT(target, GL_SRC_ALPHA);
	NANX_CONSTANT(target, GL_ONE_MINUS_SRC_ALPHA);
	NANX_CONSTANT(target, GL_DST_ALPHA);
	NANX_CONSTANT(target, GL_ONE_MINUS_DST_ALPHA);
	NANX_CONSTANT(target, GL_DST_COLOR);
	NANX_CONSTANT(target, GL_ONE_MINUS_DST_COLOR);
	NANX_CONSTANT(target, GL_SRC_ALPHA_SATURATE);
	NANX_CONSTANT(target, GL_FUNC_ADD);
	NANX_CONSTANT(target, GL_BLEND_EQUATION);
	NANX_CONSTANT(target, GL_BLEND_EQUATION_RGB);
	NANX_CONSTANT(target, GL_BLEND_EQUATION_ALPHA);
	NANX_CONSTANT(target, GL_FUNC_SUBTRACT);
	NANX_CONSTANT(target, GL_FUNC_REVERSE_SUBTRACT);
	NANX_CONSTANT(target, GL_BLEND_DST_RGB);
	NANX_CONSTANT(target, GL_BLEND_SRC_RGB);
	NANX_CONSTANT(target, GL_BLEND_DST_ALPHA);
	NANX_CONSTANT(target, GL_BLEND_SRC_ALPHA);
	NANX_CONSTANT(target, GL_CONSTANT_COLOR);
	NANX_CONSTANT(target, GL_ONE_MINUS_CONSTANT_COLOR);
	NANX_CONSTANT(target, GL_CONSTANT_ALPHA);
	NANX_CONSTANT(target, GL_ONE_MINUS_CONSTANT_ALPHA);
	NANX_CONSTANT(target, GL_BLEND_COLOR);
	NANX_CONSTANT(target, GL_ARRAY_BUFFER);
	NANX_CONSTANT(target, GL_ELEMENT_ARRAY_BUFFER);
	NANX_CONSTANT(target, GL_ARRAY_BUFFER_BINDING);
	NANX_CONSTANT(target, GL_ELEMENT_ARRAY_BUFFER_BINDING);
	NANX_CONSTANT(target, GL_STREAM_DRAW);
	NANX_CONSTANT(target, GL_STATIC_DRAW);
	NANX_CONSTANT(target, GL_DYNAMIC_DRAW);
	NANX_CONSTANT(target, GL_BUFFER_SIZE);
	NANX_CONSTANT(target, GL_BUFFER_USAGE);
	NANX_CONSTANT(target, GL_CURRENT_VERTEX_ATTRIB);
	NANX_CONSTANT(target, GL_FRONT);
	NANX_CONSTANT(target, GL_BACK);
	NANX_CONSTANT(target, GL_FRONT_AND_BACK);
	NANX_CONSTANT(target, GL_TEXTURE_2D);
	NANX_CONSTANT(target, GL_CULL_FACE);
	NANX_CONSTANT(target, GL_BLEND);
	NANX_CONSTANT(target, GL_DITHER);
	NANX_CONSTANT(target, GL_STENCIL_TEST);
	NANX_CONSTANT(target, GL_DEPTH_TEST);
	NANX_CONSTANT(target, GL_SCISSOR_TEST);
	NANX_CONSTANT(target, GL_POLYGON_OFFSET_FILL);
	NANX_CONSTANT(target, GL_SAMPLE_ALPHA_TO_COVERAGE);
	NANX_CONSTANT(target, GL_SAMPLE_COVERAGE);
	NANX_CONSTANT(target, GL_NO_ERROR);
	NANX_CONSTANT(target, GL_INVALID_ENUM);
	NANX_CONSTANT(target, GL_INVALID_VALUE);
	NANX_CONSTANT(target, GL_INVALID_OPERATION);
	NANX_CONSTANT(target, GL_OUT_OF_MEMORY);
	NANX_CONSTANT(target, GL_CW);
	NANX_CONSTANT(target, GL_CCW);
	NANX_CONSTANT(target, GL_LINE_WIDTH);
	NANX_CONSTANT(target, GL_ALIASED_POINT_SIZE_RANGE);
	NANX_CONSTANT(target, GL_ALIASED_LINE_WIDTH_RANGE);
	NANX_CONSTANT(target, GL_CULL_FACE_MODE);
	NANX_CONSTANT(target, GL_FRONT_FACE);
	NANX_CONSTANT(target, GL_DEPTH_RANGE);
	NANX_CONSTANT(target, GL_DEPTH_WRITEMASK);
	NANX_CONSTANT(target, GL_DEPTH_CLEAR_VALUE);
	NANX_CONSTANT(target, GL_DEPTH_FUNC);
	NANX_CONSTANT(target, GL_STENCIL_CLEAR_VALUE);
	NANX_CONSTANT(target, GL_STENCIL_FUNC);
	NANX_CONSTANT(target, GL_STENCIL_FAIL);
	NANX_CONSTANT(target, GL_STENCIL_PASS_DEPTH_FAIL);
	NANX_CONSTANT(target, GL_STENCIL_PASS_DEPTH_PASS);
	NANX_CONSTANT(target, GL_STENCIL_REF);
	NANX_CONSTANT(target, GL_STENCIL_VALUE_MASK);
	NANX_CONSTANT(target, GL_STENCIL_WRITEMASK);
	NANX_CONSTANT(target, GL_STENCIL_BACK_FUNC);
	NANX_CONSTANT(target, GL_STENCIL_BACK_FAIL);
	NANX_CONSTANT(target, GL_STENCIL_BACK_PASS_DEPTH_FAIL);
	NANX_CONSTANT(target, GL_STENCIL_BACK_PASS_DEPTH_PASS);
	NANX_CONSTANT(target, GL_STENCIL_BACK_REF);
	NANX_CONSTANT(target, GL_STENCIL_BACK_VALUE_MASK);
	NANX_CONSTANT(target, GL_STENCIL_BACK_WRITEMASK);
	NANX_CONSTANT(target, GL_VIEWPORT);
	NANX_CONSTANT(target, GL_SCISSOR_BOX);
	NANX_CONSTANT(target, GL_COLOR_CLEAR_VALUE);
	NANX_CONSTANT(target, GL_COLOR_WRITEMASK);
	NANX_CONSTANT(target, GL_UNPACK_ALIGNMENT);
	NANX_CONSTANT(target, GL_PACK_ALIGNMENT);
	NANX_CONSTANT(target, GL_MAX_TEXTURE_SIZE);
	NANX_CONSTANT(target, GL_MAX_VIEWPORT_DIMS);
	NANX_CONSTANT(target, GL_SUBPIXEL_BITS);
	NANX_CONSTANT(target, GL_RED_BITS);
	NANX_CONSTANT(target, GL_GREEN_BITS);
	NANX_CONSTANT(target, GL_BLUE_BITS);
	NANX_CONSTANT(target, GL_ALPHA_BITS);
	NANX_CONSTANT(target, GL_DEPTH_BITS);
	NANX_CONSTANT(target, GL_STENCIL_BITS);
	NANX_CONSTANT(target, GL_POLYGON_OFFSET_UNITS);
	NANX_CONSTANT(target, GL_POLYGON_OFFSET_FACTOR);
	NANX_CONSTANT(target, GL_TEXTURE_BINDING_2D);
	NANX_CONSTANT(target, GL_SAMPLE_BUFFERS);
	NANX_CONSTANT(target, GL_SAMPLES);
	NANX_CONSTANT(target, GL_SAMPLE_COVERAGE_VALUE);
	NANX_CONSTANT(target, GL_SAMPLE_COVERAGE_INVERT);
	NANX_CONSTANT(target, GL_NUM_COMPRESSED_TEXTURE_FORMATS);
	NANX_CONSTANT(target, GL_COMPRESSED_TEXTURE_FORMATS);
	NANX_CONSTANT(target, GL_DONT_CARE);
	NANX_CONSTANT(target, GL_FASTEST);
	NANX_CONSTANT(target, GL_NICEST);
	NANX_CONSTANT(target, GL_GENERATE_MIPMAP_HINT);
	NANX_CONSTANT(target, GL_BYTE);
	NANX_CONSTANT(target, GL_UNSIGNED_BYTE);
	NANX_CONSTANT(target, GL_SHORT);
	NANX_CONSTANT(target, GL_UNSIGNED_SHORT);
	NANX_CONSTANT(target, GL_INT);
	NANX_CONSTANT(target, GL_UNSIGNED_INT);
	NANX_CONSTANT(target, GL_FLOAT);
	NANX_CONSTANT(target, GL_FIXED);
	NANX_CONSTANT(target, GL_DEPTH_COMPONENT);
	NANX_CONSTANT(target, GL_ALPHA);
	NANX_CONSTANT(target, GL_RGB);
	NANX_CONSTANT(target, GL_RGBA);
	NANX_CONSTANT(target, GL_LUMINANCE);
	NANX_CONSTANT(target, GL_LUMINANCE_ALPHA);
	NANX_CONSTANT(target, GL_UNSIGNED_SHORT_4_4_4_4);
	NANX_CONSTANT(target, GL_UNSIGNED_SHORT_5_5_5_1);
	NANX_CONSTANT(target, GL_UNSIGNED_SHORT_5_6_5);
	NANX_CONSTANT(target, GL_FRAGMENT_SHADER);
	NANX_CONSTANT(target, GL_VERTEX_SHADER);
	NANX_CONSTANT(target, GL_MAX_VERTEX_ATTRIBS);
	NANX_CONSTANT(target, GL_MAX_VERTEX_UNIFORM_VECTORS);
	NANX_CONSTANT(target, GL_MAX_VARYING_VECTORS);
	NANX_CONSTANT(target, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
	NANX_CONSTANT(target, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
	NANX_CONSTANT(target, GL_MAX_TEXTURE_IMAGE_UNITS);
	NANX_CONSTANT(target, GL_MAX_FRAGMENT_UNIFORM_VECTORS);
	NANX_CONSTANT(target, GL_SHADER_TYPE);
	NANX_CONSTANT(target, GL_DELETE_STATUS);
	NANX_CONSTANT(target, GL_LINK_STATUS);
	NANX_CONSTANT(target, GL_VALIDATE_STATUS);
	NANX_CONSTANT(target, GL_ATTACHED_SHADERS);
	NANX_CONSTANT(target, GL_ACTIVE_UNIFORMS);
	NANX_CONSTANT(target, GL_ACTIVE_UNIFORM_MAX_LENGTH);
	NANX_CONSTANT(target, GL_ACTIVE_ATTRIBUTES);
	NANX_CONSTANT(target, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH);
	NANX_CONSTANT(target, GL_SHADING_LANGUAGE_VERSION);
	NANX_CONSTANT(target, GL_CURRENT_PROGRAM);
	NANX_CONSTANT(target, GL_NEVER);
	NANX_CONSTANT(target, GL_LESS);
	NANX_CONSTANT(target, GL_EQUAL);
	NANX_CONSTANT(target, GL_LEQUAL);
	NANX_CONSTANT(target, GL_GREATER);
	NANX_CONSTANT(target, GL_NOTEQUAL);
	NANX_CONSTANT(target, GL_GEQUAL);
	NANX_CONSTANT(target, GL_ALWAYS);
	NANX_CONSTANT(target, GL_KEEP);
	NANX_CONSTANT(target, GL_REPLACE);
	NANX_CONSTANT(target, GL_INCR);
	NANX_CONSTANT(target, GL_DECR);
	NANX_CONSTANT(target, GL_INVERT);
	NANX_CONSTANT(target, GL_INCR_WRAP);
	NANX_CONSTANT(target, GL_DECR_WRAP);
	NANX_CONSTANT(target, GL_VENDOR);
	NANX_CONSTANT(target, GL_RENDERER);
	NANX_CONSTANT(target, GL_VERSION);
	NANX_CONSTANT(target, GL_EXTENSIONS);
	NANX_CONSTANT(target, GL_NEAREST);
	NANX_CONSTANT(target, GL_LINEAR);
	NANX_CONSTANT(target, GL_NEAREST_MIPMAP_NEAREST);
	NANX_CONSTANT(target, GL_LINEAR_MIPMAP_NEAREST);
	NANX_CONSTANT(target, GL_NEAREST_MIPMAP_LINEAR);
	NANX_CONSTANT(target, GL_LINEAR_MIPMAP_LINEAR);
	NANX_CONSTANT(target, GL_TEXTURE_MAG_FILTER);
	NANX_CONSTANT(target, GL_TEXTURE_MIN_FILTER);
	NANX_CONSTANT(target, GL_TEXTURE_WRAP_S);
	NANX_CONSTANT(target, GL_TEXTURE_WRAP_T);
	NANX_CONSTANT(target, GL_TEXTURE);
	NANX_CONSTANT(target, GL_TEXTURE_CUBE_MAP);
	NANX_CONSTANT(target, GL_TEXTURE_BINDING_CUBE_MAP);
	NANX_CONSTANT(target, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	NANX_CONSTANT(target, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	NANX_CONSTANT(target, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	NANX_CONSTANT(target, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	NANX_CONSTANT(target, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	NANX_CONSTANT(target, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	NANX_CONSTANT(target, GL_MAX_CUBE_MAP_TEXTURE_SIZE);
	NANX_CONSTANT(target, GL_TEXTURE0);
	NANX_CONSTANT(target, GL_TEXTURE1);
	NANX_CONSTANT(target, GL_TEXTURE2);
	NANX_CONSTANT(target, GL_TEXTURE3);
	NANX_CONSTANT(target, GL_TEXTURE4);
	NANX_CONSTANT(target, GL_TEXTURE5);
	NANX_CONSTANT(target, GL_TEXTURE6);
	NANX_CONSTANT(target, GL_TEXTURE7);
	NANX_CONSTANT(target, GL_TEXTURE8);
	NANX_CONSTANT(target, GL_TEXTURE9);
	NANX_CONSTANT(target, GL_TEXTURE10);
	NANX_CONSTANT(target, GL_TEXTURE11);
	NANX_CONSTANT(target, GL_TEXTURE12);
	NANX_CONSTANT(target, GL_TEXTURE13);
	NANX_CONSTANT(target, GL_TEXTURE14);
	NANX_CONSTANT(target, GL_TEXTURE15);
	NANX_CONSTANT(target, GL_TEXTURE16);
	NANX_CONSTANT(target, GL_TEXTURE17);
	NANX_CONSTANT(target, GL_TEXTURE18);
	NANX_CONSTANT(target, GL_TEXTURE19);
	NANX_CONSTANT(target, GL_TEXTURE20);
	NANX_CONSTANT(target, GL_TEXTURE21);
	NANX_CONSTANT(target, GL_TEXTURE22);
	NANX_CONSTANT(target, GL_TEXTURE23);
	NANX_CONSTANT(target, GL_TEXTURE24);
	NANX_CONSTANT(target, GL_TEXTURE25);
	NANX_CONSTANT(target, GL_TEXTURE26);
	NANX_CONSTANT(target, GL_TEXTURE27);
	NANX_CONSTANT(target, GL_TEXTURE28);
	NANX_CONSTANT(target, GL_TEXTURE29);
	NANX_CONSTANT(target, GL_TEXTURE30);
	NANX_CONSTANT(target, GL_TEXTURE31);
	NANX_CONSTANT(target, GL_ACTIVE_TEXTURE);
	NANX_CONSTANT(target, GL_REPEAT);
	NANX_CONSTANT(target, GL_CLAMP_TO_EDGE);
	NANX_CONSTANT(target, GL_MIRRORED_REPEAT);
	NANX_CONSTANT(target, GL_FLOAT_VEC2);
	NANX_CONSTANT(target, GL_FLOAT_VEC3);
	NANX_CONSTANT(target, GL_FLOAT_VEC4);
	NANX_CONSTANT(target, GL_INT_VEC2);
	NANX_CONSTANT(target, GL_INT_VEC3);
	NANX_CONSTANT(target, GL_INT_VEC4);
	NANX_CONSTANT(target, GL_BOOL);
	NANX_CONSTANT(target, GL_BOOL_VEC2);
	NANX_CONSTANT(target, GL_BOOL_VEC3);
	NANX_CONSTANT(target, GL_BOOL_VEC4);
	NANX_CONSTANT(target, GL_FLOAT_MAT2);
	NANX_CONSTANT(target, GL_FLOAT_MAT3);
	NANX_CONSTANT(target, GL_FLOAT_MAT4);
	NANX_CONSTANT(target, GL_SAMPLER_2D);
	NANX_CONSTANT(target, GL_SAMPLER_CUBE);
	NANX_CONSTANT(target, GL_VERTEX_ATTRIB_ARRAY_ENABLED);
	NANX_CONSTANT(target, GL_VERTEX_ATTRIB_ARRAY_SIZE);
	NANX_CONSTANT(target, GL_VERTEX_ATTRIB_ARRAY_STRIDE);
	NANX_CONSTANT(target, GL_VERTEX_ATTRIB_ARRAY_TYPE);
	NANX_CONSTANT(target, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED);
	NANX_CONSTANT(target, GL_VERTEX_ATTRIB_ARRAY_POINTER);
	NANX_CONSTANT(target, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING);
	NANX_CONSTANT(target, GL_IMPLEMENTATION_COLOR_READ_TYPE);
	NANX_CONSTANT(target, GL_IMPLEMENTATION_COLOR_READ_FORMAT);
	NANX_CONSTANT(target, GL_COMPILE_STATUS);
	NANX_CONSTANT(target, GL_INFO_LOG_LENGTH);
	NANX_CONSTANT(target, GL_SHADER_SOURCE_LENGTH);
	NANX_CONSTANT(target, GL_SHADER_COMPILER);
	NANX_CONSTANT(target, GL_SHADER_BINARY_FORMATS);
	NANX_CONSTANT(target, GL_NUM_SHADER_BINARY_FORMATS);
	NANX_CONSTANT(target, GL_LOW_FLOAT);
	NANX_CONSTANT(target, GL_MEDIUM_FLOAT);
	NANX_CONSTANT(target, GL_HIGH_FLOAT);
	NANX_CONSTANT(target, GL_LOW_INT);
	NANX_CONSTANT(target, GL_MEDIUM_INT);
	NANX_CONSTANT(target, GL_HIGH_INT);
	NANX_CONSTANT(target, GL_FRAMEBUFFER);
	NANX_CONSTANT(target, GL_RENDERBUFFER);
	NANX_CONSTANT(target, GL_RGBA4);
	NANX_CONSTANT(target, GL_RGB5_A1);
	NANX_CONSTANT(target, GL_RGB565);
	NANX_CONSTANT(target, GL_DEPTH_COMPONENT16);
	NANX_CONSTANT(target, GL_STENCIL_INDEX8);
	NANX_CONSTANT(target, GL_RENDERBUFFER_WIDTH);
	NANX_CONSTANT(target, GL_RENDERBUFFER_HEIGHT);
	NANX_CONSTANT(target, GL_RENDERBUFFER_INTERNAL_FORMAT);
	NANX_CONSTANT(target, GL_RENDERBUFFER_RED_SIZE);
	NANX_CONSTANT(target, GL_RENDERBUFFER_GREEN_SIZE);
	NANX_CONSTANT(target, GL_RENDERBUFFER_BLUE_SIZE);
	NANX_CONSTANT(target, GL_RENDERBUFFER_ALPHA_SIZE);
	NANX_CONSTANT(target, GL_RENDERBUFFER_DEPTH_SIZE);
	NANX_CONSTANT(target, GL_RENDERBUFFER_STENCIL_SIZE);
	NANX_CONSTANT(target, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
	NANX_CONSTANT(target, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME);
	NANX_CONSTANT(target, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL);
	NANX_CONSTANT(target, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);
	NANX_CONSTANT(target, GL_COLOR_ATTACHMENT0);
	NANX_CONSTANT(target, GL_DEPTH_ATTACHMENT);
	NANX_CONSTANT(target, GL_STENCIL_ATTACHMENT);
	NANX_CONSTANT(target, GL_NONE);
	NANX_CONSTANT(target, GL_FRAMEBUFFER_COMPLETE);
	NANX_CONSTANT(target, GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
	NANX_CONSTANT(target, GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
//	NANX_CONSTANT(target, GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS);
	NANX_CONSTANT(target, GL_FRAMEBUFFER_UNSUPPORTED);
	NANX_CONSTANT(target, GL_FRAMEBUFFER_BINDING);
	NANX_CONSTANT(target, GL_RENDERBUFFER_BINDING);
	NANX_CONSTANT(target, GL_MAX_RENDERBUFFER_SIZE);
	NANX_CONSTANT(target, GL_INVALID_FRAMEBUFFER_OPERATION);

	#ifndef GL_POINT_SPRITE
	#define GL_POINT_SPRITE 0x8861
	#endif
	NANX_CONSTANT(target, GL_POINT_SPRITE);
	
	#ifndef GL_VERTEX_PROGRAM_POINT_SIZE
	#define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
	#endif
	NANX_CONSTANT(target, GL_VERTEX_PROGRAM_POINT_SIZE);

	NANX_EXPORT_APPLY(target, glActiveTexture);
	NANX_EXPORT_APPLY(target, glAttachShader);
	NANX_EXPORT_APPLY(target, glBindAttribLocation);
	NANX_EXPORT_APPLY(target, glBindBuffer);
	NANX_EXPORT_APPLY(target, glBindFramebuffer);
	NANX_EXPORT_APPLY(target, glBindRenderbuffer);
	NANX_EXPORT_APPLY(target, glBindTexture);
	NANX_EXPORT_APPLY(target, glBlendColor);
	NANX_EXPORT_APPLY(target, glBlendEquation);
	NANX_EXPORT_APPLY(target, glBlendEquationSeparate);
	NANX_EXPORT_APPLY(target, glBlendFunc);
	NANX_EXPORT_APPLY(target, glBlendFuncSeparate);
	NANX_EXPORT_APPLY(target, glBufferData);
	NANX_EXPORT_APPLY(target, glBufferSubData);
	NANX_EXPORT_APPLY(target, glCheckFramebufferStatus);
	NANX_EXPORT_APPLY(target, glClear);
	NANX_EXPORT_APPLY(target, glClearColor);
	NANX_EXPORT_APPLY(target, glClearDepthf);
	NANX_EXPORT_APPLY(target, glClearStencil);
	NANX_EXPORT_APPLY(target, glColorMask);
	NANX_EXPORT_APPLY(target, glCompileShader);
	NANX_EXPORT_APPLY(target, glCompressedTexImage2D);
	NANX_EXPORT_APPLY(target, glCompressedTexSubImage2D);
	NANX_EXPORT_APPLY(target, glCopyTexImage2D);
	NANX_EXPORT_APPLY(target, glCopyTexSubImage2D);
	NANX_EXPORT_APPLY(target, glCreateProgram);
	NANX_EXPORT_APPLY(target, glCreateShader);
	NANX_EXPORT_APPLY(target, glCullFace);
	NANX_EXPORT_APPLY(target, glDeleteBuffers);
	NANX_EXPORT_APPLY(target, glDeleteFramebuffers);
	NANX_EXPORT_APPLY(target, glDeleteProgram);
	NANX_EXPORT_APPLY(target, glDeleteRenderbuffers);
	NANX_EXPORT_APPLY(target, glDeleteShader);
	NANX_EXPORT_APPLY(target, glDeleteTextures);
	NANX_EXPORT_APPLY(target, glDepthFunc);
	NANX_EXPORT_APPLY(target, glDepthMask);
	NANX_EXPORT_APPLY(target, glDepthRangef);
	NANX_EXPORT_APPLY(target, glDetachShader);
	NANX_EXPORT_APPLY(target, glDisable);
	NANX_EXPORT_APPLY(target, glDisableVertexAttribArray);
	NANX_EXPORT_APPLY(target, glDrawArrays);
	NANX_EXPORT_APPLY(target, glDrawElements);
	NANX_EXPORT_APPLY(target, glEnable);
	NANX_EXPORT_APPLY(target, glEnableVertexAttribArray);
	NANX_EXPORT_APPLY(target, glFinish);
	NANX_EXPORT_APPLY(target, glFlush);
	NANX_EXPORT_APPLY(target, glFramebufferRenderbuffer);
	NANX_EXPORT_APPLY(target, glFramebufferTexture2D);
	NANX_EXPORT_APPLY(target, glFrontFace);
	NANX_EXPORT_APPLY(target, glGenBuffers);
	NANX_EXPORT_APPLY(target, glGenerateMipmap);
	NANX_EXPORT_APPLY(target, glGenFramebuffers);
	NANX_EXPORT_APPLY(target, glGenRenderbuffers);
	NANX_EXPORT_APPLY(target, glGenTextures);
	NANX_EXPORT_APPLY(target, glGetActiveAttrib);
	NANX_EXPORT_APPLY(target, glGetActiveUniform);
	NANX_EXPORT_APPLY(target, glGetAttachedShaders);
	NANX_EXPORT_APPLY(target, glGetAttribLocation);
	NANX_EXPORT_APPLY(target, glGetBooleanv);
	NANX_EXPORT_APPLY(target, glGetBufferParameteriv);
	NANX_EXPORT_APPLY(target, glGetError);
	NANX_EXPORT_APPLY(target, glGetFloatv);
	NANX_EXPORT_APPLY(target, glGetFramebufferAttachmentParameteriv);
	NANX_EXPORT_APPLY(target, glGetIntegerv);
	NANX_EXPORT_APPLY(target, glGetProgramiv);
	NANX_EXPORT_APPLY(target, glGetProgramInfoLog);
	NANX_EXPORT_APPLY(target, glGetRenderbufferParameteriv);
	NANX_EXPORT_APPLY(target, glGetShaderiv);
	NANX_EXPORT_APPLY(target, glGetShaderInfoLog);
	NANX_EXPORT_APPLY(target, glGetShaderPrecisionFormat);
	NANX_EXPORT_APPLY(target, glGetShaderSource);
	NANX_EXPORT_APPLY(target, glGetString);
	NANX_EXPORT_APPLY(target, glGetTexParameterfv);
	NANX_EXPORT_APPLY(target, glGetTexParameteriv);
	NANX_EXPORT_APPLY(target, glGetUniformfv);
	NANX_EXPORT_APPLY(target, glGetUniformiv);
	NANX_EXPORT_APPLY(target, glGetUniformLocation);
	NANX_EXPORT_APPLY(target, glGetVertexAttribfv);
	NANX_EXPORT_APPLY(target, glGetVertexAttribiv);
	NANX_EXPORT_APPLY(target, glGetVertexAttribPointerv);
	NANX_EXPORT_APPLY(target, glHint);
	NANX_EXPORT_APPLY(target, glIsBuffer);
	NANX_EXPORT_APPLY(target, glIsEnabled);
	NANX_EXPORT_APPLY(target, glIsFramebuffer);
	NANX_EXPORT_APPLY(target, glIsProgram);
	NANX_EXPORT_APPLY(target, glIsRenderbuffer);
	NANX_EXPORT_APPLY(target, glIsShader);
	NANX_EXPORT_APPLY(target, glIsTexture);
	NANX_EXPORT_APPLY(target, glLineWidth);
	NANX_EXPORT_APPLY(target, glLinkProgram);
	NANX_EXPORT_APPLY(target, glPixelStorei);
	NANX_EXPORT_APPLY(target, glPolygonOffset);
	NANX_EXPORT_APPLY(target, glReadPixels);
	NANX_EXPORT_APPLY(target, glReleaseShaderCompiler);
	NANX_EXPORT_APPLY(target, glRenderbufferStorage);
	NANX_EXPORT_APPLY(target, glSampleCoverage);
	NANX_EXPORT_APPLY(target, glScissor);
	NANX_EXPORT_APPLY(target, glShaderBinary);
	NANX_EXPORT_APPLY(target, glShaderSource);
	NANX_EXPORT_APPLY(target, glStencilFunc);
	NANX_EXPORT_APPLY(target, glStencilFuncSeparate);
	NANX_EXPORT_APPLY(target, glStencilMask);
	NANX_EXPORT_APPLY(target, glStencilMaskSeparate);
	NANX_EXPORT_APPLY(target, glStencilOp);
	NANX_EXPORT_APPLY(target, glStencilOpSeparate);
	NANX_EXPORT_APPLY(target, glTexImage2D);
	NANX_EXPORT_APPLY(target, glTexParameterf);
	NANX_EXPORT_APPLY(target, glTexParameterfv);
	NANX_EXPORT_APPLY(target, glTexParameteri);
	NANX_EXPORT_APPLY(target, glTexParameteriv);
	NANX_EXPORT_APPLY(target, glTexSubImage2D);
	NANX_EXPORT_APPLY(target, glUniform1f);
	NANX_EXPORT_APPLY(target, glUniform1fv);
	NANX_EXPORT_APPLY(target, glUniform1i);
	NANX_EXPORT_APPLY(target, glUniform1iv);
	NANX_EXPORT_APPLY(target, glUniform2f);
	NANX_EXPORT_APPLY(target, glUniform2fv);
	NANX_EXPORT_APPLY(target, glUniform2i);
	NANX_EXPORT_APPLY(target, glUniform2iv);
	NANX_EXPORT_APPLY(target, glUniform3f);
	NANX_EXPORT_APPLY(target, glUniform3fv);
	NANX_EXPORT_APPLY(target, glUniform3i);
	NANX_EXPORT_APPLY(target, glUniform3iv);
	NANX_EXPORT_APPLY(target, glUniform4f);
	NANX_EXPORT_APPLY(target, glUniform4fv);
	NANX_EXPORT_APPLY(target, glUniform4i);
	NANX_EXPORT_APPLY(target, glUniform4iv);
	NANX_EXPORT_APPLY(target, glUniformMatrix2fv);
	NANX_EXPORT_APPLY(target, glUniformMatrix3fv);
	NANX_EXPORT_APPLY(target, glUniformMatrix4fv);
	NANX_EXPORT_APPLY(target, glUseProgram);
	NANX_EXPORT_APPLY(target, glValidateProgram);
	NANX_EXPORT_APPLY(target, glVertexAttrib1f);
	NANX_EXPORT_APPLY(target, glVertexAttrib1fv);
	NANX_EXPORT_APPLY(target, glVertexAttrib2f);
	NANX_EXPORT_APPLY(target, glVertexAttrib2fv);
	NANX_EXPORT_APPLY(target, glVertexAttrib3f);
	NANX_EXPORT_APPLY(target, glVertexAttrib3fv);
	NANX_EXPORT_APPLY(target, glVertexAttrib4f);
	NANX_EXPORT_APPLY(target, glVertexAttrib4fv);
	NANX_EXPORT_APPLY(target, glVertexAttribPointer);
	NANX_EXPORT_APPLY(target, glViewport);

	#endif // HAVE_OPENGLES2
}

} // namespace node_gles2

NODE_MODULE(node_gles2, node_gles2::init)
