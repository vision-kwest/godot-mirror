/*************************************************************************/
/*  typedefs.h                                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2015 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <stddef.h>
/**
 * Basic definitions and simple functions to be used everywhere..
 */

#include "platform_config.h"

#ifndef _STR
#define _STR(m_x) #m_x
#define _MKSTR(m_x) _STR(m_x)
#endif
// have to include version.h for this to work, include it in the .cpp not the .h
#define VERSION_MKSTRING _MKSTR(VERSION_MAJOR)"." _MKSTR(VERSION_MINOR)"." _MKSTR(VERSION_STATUS)"." _MKSTR(VERSION_REVISION)
#define VERSION_FULL_NAME _MKSTR(VERSION_NAME)" v" VERSION_MKSTRING


#ifndef _ALWAYS_INLINE_

#if defined(__GNUC__) && (__GNUC__ >= 4 )
#    define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
#elif defined(__llvm__)
#    define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#       define _ALWAYS_INLINE_ __forceinline
#else
#    define _ALWAYS_INLINE_ inline
#endif

#endif

#ifndef _FORCE_INLINE_

#ifdef DEBUG_ENABLED

#define _FORCE_INLINE_ inline

#else

#define _FORCE_INLINE_ _ALWAYS_INLINE_

#endif

#endif

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT 16
#endif


//custom, gcc-safe offsetof, because gcc complains a lot.
template<class T>
T *_nullptr() { T*t=NULL; return t; }

#define OFFSET_OF(st, m) \
((size_t) ( (char *)&(_nullptr<st>()->m) - (char *)0 ))
/**
 * Some platforms (devices) not define NULL
 */

#ifndef NULL
#define NULL 0
#endif

/**
 * Windows defines a lot of badly stuff we'll never ever use. undefine it.
 */

#ifdef _WIN32
#	undef min // override standard definition
#	undef max // override standard definition
#	undef ERROR // override (really stupid) wingdi.h standard definition
#	undef DELETE // override (another really stupid) winnt.h standard definition
#	undef MessageBox // override winuser.h standard definition
#	undef MIN // override standard definition
#	undef MAX // override standard definition
#	undef CLAMP // override standard definition
#	undef Error
#	undef OK
#endif

#include "error_macros.h"
#include "error_list.h"

#include "int_types.h"

/** Generic ABS function, for math uses please use Math::abs */

#ifndef ABS
#define ABS(m_v) ((m_v<0)?(-(m_v)):(m_v))
#endif

#ifndef SGN
#define SGN(m_v) ((m_v<0)?(-1.0):(+1.0))
#endif

#ifndef MIN
#define MIN(m_a,m_b) (((m_a)<(m_b))?(m_a):(m_b))
#endif

#ifndef MAX
#define MAX(m_a,m_b) (((m_a)>(m_b))?(m_a):(m_b))
#endif

#ifndef CLAMP
#define CLAMP(m_a,m_min,m_max) (((m_a)<(m_min))?(m_min):(((m_a)>(m_max))?m_max:m_a))
#endif

/** Generic swap template */
#ifndef SWAP

#define SWAP(m_x,m_y) __swap_tmpl(m_x,m_y)
template<class T>
inline void __swap_tmpl(T &x, T &y ) {

	T aux=x;
	x=y;
	y=aux;
}

#endif //swap

#define HEX2CHR( m_hex ) ( (m_hex>='0' && m_hex<='9')?(m_hex-'0'):\
	((m_hex>='A' && m_hex<='F')?(10+m_hex-'A'):\
	((m_hex>='a' && m_hex<='f')?(10+m_hex-'a'):0)))




/** Function to find the nearest (bigger) power of 2 to an integer */

static _FORCE_INLINE_ unsigned int nearest_power_of_2(unsigned int x) {

	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return ++x;
}

/** Function to find the nearest (bigger) power of 2 to an integer */

static inline unsigned int nearest_shift(unsigned int p_number) {

	for (int i=30;i>=0;i--) {

		if (p_number&(1<<i))
			return i+1;
	}

	return 0;
}

/** get a shift value from a power of 2 */
static inline int get_shift_from_power_of_2( unsigned int p_pixel ) {
	// return a GL_TEXTURE_SIZE_ENUM


	for (unsigned int i=0;i<32;i++) {

		if (p_pixel==(unsigned int)(1<<i))
			return i;
	}

	return -1;
}

/** Swap 32 bits value for endianness */
static inline uint32_t BSWAP32(uint32_t x) {
	return((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24));
}

/** When compiling with RTTI, we can add an "extra"
 * layer of safeness in many operations, so dynamic_cast
 * is used besides casting by enum.
 */

template<class T>
struct Comparator {

	inline bool operator()(const T& p_a, const T& p_b) const { return (p_a<p_b); }

};


void _global_lock();
void _global_unlock();

struct _GlobalLock {
	
	_GlobalLock() { _global_lock(); }
	~_GlobalLock() { _global_unlock(); }
};

#define GLOBAL_LOCK_FUNCTION _GlobalLock _global_lock_;
#ifdef NO_SAFE_CAST

#define SAFE_CAST static_cast

#else

#define SAFE_CAST dynamic_cast

#endif

#define MT_SAFE

#define __STRX(m_index) #m_index
#define __STR(m_index) __STRX(m_index)


#endif  /* typedefs.h */

