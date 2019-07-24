#pragma once
#include <cstdint>

namespace gl {
	typedef unsigned int GLenum;
	typedef unsigned char GLboolean;
	typedef unsigned int GLbitfield;
	typedef void GLvoid;
	typedef signed char GLbyte;
	typedef short GLshort;
	typedef int GLint;
	typedef unsigned char GLubyte;
	typedef unsigned short GLushort;
	typedef unsigned int GLuint;
	typedef int GLsizei;
	typedef float GLfloat;
	typedef float GLclampf;
	typedef double GLdouble;
	typedef double GLclampd;
	typedef char GLchar;
	typedef char GLcharARB;

	typedef unsigned short GLhalfARB;
	typedef unsigned short GLhalf;
	typedef GLint GLfixed;
	typedef ptrdiff_t GLintptr;
	typedef ptrdiff_t GLsizeiptr;
	typedef int64_t GLint64;
	typedef uint64_t GLuint64;
	typedef ptrdiff_t GLintptrARB;
	typedef ptrdiff_t GLsizeiptrARB;
	typedef int64_t GLint64EXT;
	typedef uint64_t GLuint64EXT;
	typedef struct __GLsync *GLsync;
	struct _cl_context;
	struct _cl_event;
}
