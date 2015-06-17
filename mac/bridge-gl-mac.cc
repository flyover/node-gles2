#include "bridge-gl-mac.h"

void glReleaseShaderCompiler()
{
}

void glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
{
	range[0] = range[1] = precision[0] = 0;
}

void glShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length)
{
}
