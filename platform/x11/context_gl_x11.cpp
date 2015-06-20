/*************************************************************************/
/*  context_gl_x11.cpp                                                   */
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
#include "context_gl_x11.h"

#ifdef X11_ENABLED
#if defined(OPENGL_ENABLED) || defined(LEGACYGL_ENABLED)
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <GL/glx.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB		0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB		0x2092

typedef GLXContext (*GLXCREATECONTEXTATTRIBSARBPROC)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

struct ContextGL_X11_Private { 

	::GLXContext glx_context;
};


void ContextGL_X11::release_current() {

	glXMakeCurrent(x11_display, None, NULL);
}

void ContextGL_X11::make_current() {

	glXMakeCurrent(x11_display, x11_window, p->glx_context);
}
void ContextGL_X11::swap_buffers() {

	glXSwapBuffers(x11_display,x11_window);
}
/*
static GLWrapperFuncPtr wrapper_get_proc_address(const char* p_function) {

	//print_line(String()+"getting proc of: "+p_function);
	GLWrapperFuncPtr func=(GLWrapperFuncPtr)glXGetProcAddress( (const GLubyte*) p_function);
	if (!func) {
		print_line("Couldn't find function: "+String(p_function));
	}

	return func;

}*/

Error ContextGL_X11::initialize() {

	
	GLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = NULL;
	
//	const char *extensions = glXQueryExtensionsString(x11_display, DefaultScreen(x11_display));
	
	glXCreateContextAttribsARB = (GLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
	
	ERR_FAIL_COND_V( !glXCreateContextAttribsARB, ERR_UNCONFIGURED );


	static int visual_attribs[] = {
	    GLX_RENDER_TYPE, GLX_RGBA_BIT,
	    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	    GLX_DOUBLEBUFFER, true,
	    GLX_RED_SIZE, 1,
	    GLX_GREEN_SIZE, 1,
	    GLX_BLUE_SIZE, 1,
	    GLX_DEPTH_SIZE, 24,
	    None 
	};

	int fbcount;
	GLXFBConfig *fbc = glXChooseFBConfig(x11_display, DefaultScreen(x11_display), visual_attribs, &fbcount);
	ERR_FAIL_COND_V(!fbc,ERR_UNCONFIGURED);
	
	XVisualInfo *vi = glXGetVisualFromFBConfig(x11_display, fbc[0]);

	XSetWindowAttributes swa;

	swa.colormap = XCreateColormap(x11_display, RootWindow(x11_display, vi->screen), vi->visual, AllocNone);
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;

	/*
	char* windowid = getenv("GODOT_WINDOWID");
	if (windowid) {

		//freopen("/home/punto/stdout", "w", stdout);
		//reopen("/home/punto/stderr", "w", stderr);
		x11_window = atol(windowid);
	} else {
	*/
		x11_window = XCreateWindow(x11_display, RootWindow(x11_display, vi->screen), 0, 0, OS::get_singleton()->get_video_mode().width, OS::get_singleton()->get_video_mode().height, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel|CWColormap|CWEventMask, &swa);
		ERR_FAIL_COND_V(!x11_window,ERR_UNCONFIGURED);
		XMapWindow(x11_display, x11_window);
		while(true) {
			// wait for mapnotify (window created)
			XEvent e;
			XNextEvent(x11_display, &e);
			if (e.type == MapNotify)
				break;
		}
		//};


	if (!opengl_3_context) {
		//oldstyle context:
		p->glx_context = glXCreateContext(x11_display, vi, 0, GL_TRUE);
	} else {
		static int context_attribs[] = {
			GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
			None
		};
	
		p->glx_context = glXCreateContextAttribsARB(x11_display, fbc[0], NULL, true, context_attribs);
		ERR_FAIL_COND_V(!p->glx_context,ERR_UNCONFIGURED);
	}

	glXMakeCurrent(x11_display, x11_window, p->glx_context);

	/*
	glWrapperInit(wrapper_get_proc_address);
	glFlush();
	
	glXSwapBuffers(x11_display,x11_window);
*/
	//glXMakeCurrent(x11_display, None, NULL);

	return OK;
}

int ContextGL_X11::get_window_width() {

	XWindowAttributes xwa;
	XGetWindowAttributes(x11_display,x11_window,&xwa);
	
	return xwa.width;
}
int ContextGL_X11::get_window_height() {
	XWindowAttributes xwa;
	XGetWindowAttributes(x11_display,x11_window,&xwa);
	
	return xwa.height;

}


ContextGL_X11::ContextGL_X11(::Display *p_x11_display,::Window &p_x11_window,const OS::VideoMode& p_default_video_mode,bool p_opengl_3_context) : x11_window(p_x11_window) {

	default_video_mode=p_default_video_mode;
	x11_display=p_x11_display;
	
	opengl_3_context=p_opengl_3_context;
	
	double_buffer=false;
	direct_render=false;
	glx_minor=glx_major=0;
	p = memnew( ContextGL_X11_Private );
	p->glx_context=0;
}


ContextGL_X11::~ContextGL_X11() {

	memdelete( p );
}


#endif
#endif
