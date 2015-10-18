/*************************************************************************/
/*  os_x11.cpp                                                           */
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
#include "servers/visual/visual_server_raster.h"
#include "drivers/gles2/rasterizer_gles2.h"
#include "os_x11.h"
#include "key_mapping_x11.h"
#include <stdio.h>
#include <stdlib.h>
#include "print_string.h"
#include "servers/physics/physics_server_sw.h"


#include "X11/Xutil.h"

#include "X11/Xatom.h"
#include "X11/extensions/Xinerama.h"
// ICCCM
#define WM_NormalState		1L	// window normal state
#define WM_IconicState		3L	// window minimized
// EWMH
#define _NET_WM_STATE_REMOVE	0L	// remove/unset property
#define _NET_WM_STATE_ADD	1L	// add/set property
#define _NET_WM_STATE_TOGGLE	2L	// toggle property

#include "main/main.h"



#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef __linux__
#include <linux/joystick.h>
#endif

//stupid linux.h
#ifdef KEY_TAB
#undef KEY_TAB
#endif


#include <X11/Xatom.h>
//#include "os/pc_joystick_map.h"

#undef CursorShape

int OS_X11::get_video_driver_count() const {

	return 1;
}
const char * OS_X11::get_video_driver_name(int p_driver) const {

	return "GLES2";
}
OS::VideoMode OS_X11::get_default_video_mode() const {

	return OS::VideoMode(800,600,false);
}

int OS_X11::get_audio_driver_count() const {

    return AudioDriverManagerSW::get_driver_count();
}

const char *OS_X11::get_audio_driver_name(int p_driver) const {

    AudioDriverSW* driver = AudioDriverManagerSW::get_driver(p_driver);
    ERR_FAIL_COND_V( !driver, "" );
    return AudioDriverManagerSW::get_driver(p_driver)->get_name();
}

void OS_X11::initialize(const VideoMode& p_desired,int p_video_driver,int p_audio_driver) {

	last_button_state=0;
	dpad_last[0]=0;
	dpad_last[1]=0;

	xmbstring=NULL;
	event_id=0;
	x11_window=0;
	last_click_ms=0;
	args=OS::get_singleton()->get_cmdline_args();
	current_videomode=p_desired;
	main_loop=NULL;
	last_timestamp=0;
	last_mouse_pos_valid=false;
	last_keyrelease_time=0;

	if (get_render_thread_mode()==RENDER_SEPARATE_THREAD) {
		XInitThreads();
	}
	
	/** XLIB INITIALIZATION **/
	x11_display = XOpenDisplay(NULL);
	
	char * modifiers = XSetLocaleModifiers ("@im=none");
	ERR_FAIL_COND( modifiers == NULL );
	
	xim = XOpenIM (x11_display, NULL, NULL, NULL);
	

	if (xim == NULL) {
		WARN_PRINT("XOpenIM failed");
		xim_style=0L;
	} else {
		::XIMStyles *xim_styles=NULL;
		xim_style=0L;
		char *imvalret=NULL;
		imvalret = XGetIMValues(xim, XNQueryInputStyle, &xim_styles, NULL);
		if (imvalret != NULL || xim_styles == NULL) {
			fprintf (stderr, "Input method doesn't support any styles\n");
		}
		
		if (xim_styles) {
			xim_style = 0L;
			for (int i=0;i<xim_styles->count_styles;i++) {
				
				if (xim_styles->supported_styles[i] ==
				    (XIMPreeditNothing | XIMStatusNothing)) {
					    
					    xim_style = xim_styles->supported_styles[i];
					    break;
				    }
			}
			
			XFree (xim_styles);
		}
	}

	/*
	char* windowid = getenv("GODOT_WINDOWID");
	if (windowid) {

		//freopen("/home/punto/stdout", "w", stdout);
		//reopen("/home/punto/stderr", "w", stderr);
		x11_window = atol(windowid);

		XWindowAttributes xwa;
		XGetWindowAttributes(x11_display,x11_window,&xwa);

		current_videomode.width = xwa.width;
		current_videomode.height = xwa.height;
	};
	*/

	// maybe contextgl wants to be in charge of creating the window
	//print_line("def videomode "+itos(current_videomode.width)+","+itos(current_videomode.height));
#if defined(OPENGL_ENABLED) || defined(LEGACYGL_ENABLED)

	context_gl = memnew( ContextGL_X11( x11_display, x11_window,current_videomode, false ) );
	context_gl->initialize();

	rasterizer = memnew( RasterizerGLES2 );

#endif
	visual_server = memnew( VisualServerRaster(rasterizer) );

	if (get_render_thread_mode()!=RENDER_THREAD_UNSAFE) {

		visual_server =memnew(VisualServerWrapMT(visual_server,get_render_thread_mode()==RENDER_SEPARATE_THREAD));
	}

#if 1
	// NEW_WM_API
	// borderless fullscreen window mode
	if (current_videomode.fullscreen) {
	// needed for lxde/openbox, possibly others
		Hints hints;
		Atom property;
		hints.flags = 2;
		hints.decorations = 0;
		property = XInternAtom(x11_display, "_MOTIF_WM_HINTS", True);
		XChangeProperty(x11_display, x11_window, property, property, 32, PropModeReplace, (unsigned char *)&hints, 5);
		XMapRaised(x11_display, x11_window);
		XWindowAttributes xwa;
		XGetWindowAttributes(x11_display, DefaultRootWindow(x11_display), &xwa);
		XMoveResizeWindow(x11_display, x11_window, 0, 0, xwa.width, xwa.height);

		// code for netwm-compliants
		XEvent xev;
		Atom wm_state = XInternAtom(x11_display, "_NET_WM_STATE", False);
		Atom fullscreen = XInternAtom(x11_display, "_NET_WM_STATE_FULLSCREEN", False);

		memset(&xev, 0, sizeof(xev));
		xev.type = ClientMessage;
		xev.xclient.window = x11_window;
		xev.xclient.message_type = wm_state;
		xev.xclient.format = 32;
		xev.xclient.data.l[0] = 1;
		xev.xclient.data.l[1] = fullscreen;
		xev.xclient.data.l[2] = 0;

		XSendEvent(x11_display, DefaultRootWindow(x11_display), False, SubstructureNotifyMask, &xev);
	}

	// disable resizable window
	if (!current_videomode.resizable) {
		XSizeHints *xsh;
		xsh = XAllocSizeHints();
		xsh->flags = PMinSize | PMaxSize;
		XWindowAttributes xwa;
		if (current_videomode.fullscreen) {
			XGetWindowAttributes(x11_display,DefaultRootWindow(x11_display),&xwa);
		} else {
			XGetWindowAttributes(x11_display,x11_window,&xwa);
		}
		xsh->min_width = xwa.width; 
		xsh->max_width = xwa.width;
		xsh->min_height = xwa.height;
		xsh->max_height = xwa.height;
		XSetWMNormalHints(x11_display, x11_window, xsh);
		XFree(xsh);
	}
#else
	capture_idle = 0;
	minimized = false;
	maximized = false;

	if (current_videomode.fullscreen) {
		//set_wm_border(false);
		set_wm_fullscreen(true);
	}
	if (!current_videomode.resizable) {
		int screen = get_current_screen();
		Size2i screen_size = get_screen_size(screen);
		set_window_size(screen_size);
		set_window_resizable(false);
	}
#endif


	AudioDriverManagerSW::get_driver(p_audio_driver)->set_singleton();

	audio_driver_index=p_audio_driver;
	if (AudioDriverManagerSW::get_driver(p_audio_driver)->init()!=OK) {

		bool success=false;
		audio_driver_index=-1;
		for(int i=0;i<AudioDriverManagerSW::get_driver_count();i++) {
			if (i==p_audio_driver)
				continue;
			AudioDriverManagerSW::get_driver(i)->set_singleton();
			if (AudioDriverManagerSW::get_driver(i)->init()==OK) {
				success=true;
				print_line("Audio Driver Failed: "+String(AudioDriverManagerSW::get_driver(p_audio_driver)->get_name()));
				print_line("Using alternate audio driver: "+String(AudioDriverManagerSW::get_driver(i)->get_name()));
				audio_driver_index=i;
				break;
			}
		}
		if (!success) {
			ERR_PRINT("Initializing audio failed.");
		}

	}

	sample_manager = memnew( SampleManagerMallocSW );
	audio_server = memnew( AudioServerSW(sample_manager) );
	audio_server->init();
	spatial_sound_server = memnew( SpatialSoundServerSW );
	spatial_sound_server->init();
	spatial_sound_2d_server = memnew( SpatialSound2DServerSW );
	spatial_sound_2d_server->init();

	
	ERR_FAIL_COND(!visual_server);
	ERR_FAIL_COND(x11_window==0);

	XSetWindowAttributes new_attr;

	new_attr.event_mask=KeyPressMask | KeyReleaseMask | ButtonPressMask |
			   ButtonReleaseMask | EnterWindowMask |
			   LeaveWindowMask | PointerMotionMask |
			   Button1MotionMask |
			   Button2MotionMask | Button3MotionMask |
			   Button4MotionMask | Button5MotionMask |
			   ButtonMotionMask | KeymapStateMask |
			   ExposureMask | VisibilityChangeMask |
			   StructureNotifyMask |
			   SubstructureNotifyMask | SubstructureRedirectMask |
			   FocusChangeMask | PropertyChangeMask |
			   ColormapChangeMask | OwnerGrabButtonMask;

	XChangeWindowAttributes(x11_display, x11_window,CWEventMask,&new_attr);
	
	XClassHint* classHint;

	/* set the titlebar name */
	XStoreName(x11_display, x11_window, "Godot");

	/* set the name and class hints for the window manager to use */
	classHint = XAllocClassHint();
	if (classHint) {
		classHint->res_name = (char *)"Godot";
		classHint->res_class = (char *)"Godot";
	}
	XSetClassHint(x11_display, x11_window, classHint);
	XFree(classHint);

	wm_delete = XInternAtom(x11_display, "WM_DELETE_WINDOW", true);	
	XSetWMProtocols(x11_display, x11_window, &wm_delete, 1);
		

	if (xim && xim_style) {
		
		xic = XCreateIC (xim,XNInputStyle, xim_style,XNClientWindow,x11_window,XNFocusWindow, x11_window, (char*)NULL);
	} else {
		
		xic=NULL;
		WARN_PRINT("XCreateIC couldn't create xic");

	}	

	XcursorSetTheme(x11_display,"default");
	cursor_size = XcursorGetDefaultSize(x11_display);
	cursor_theme = XcursorGetTheme(x11_display);

	if (!cursor_theme) {
		print_line("not found theme");
		cursor_theme="default";
	}

	for(int i=0;i<CURSOR_MAX;i++) {

		cursors[i]=None;
	}

	current_cursor=CURSOR_ARROW;

	if (cursor_theme) {
		//print_line("cursor theme: "+String(cursor_theme));
		for(int i=0;i<CURSOR_MAX;i++) {

			static const char *cursor_file[]={
				"left_ptr",
				"xterm",
				"hand2",
				"cross",
				"watch",
				"left_ptr_watch",
				"fleur",
				"hand1",
				"X_cursor",
				"sb_v_double_arrow",
				"sb_h_double_arrow",
				"size_bdiag",
				"size_fdiag",
				"hand1",
				"sb_v_double_arrow",
				"sb_h_double_arrow",
				"question_arrow"
			};

			XcursorImage *img = XcursorLibraryLoadImage(cursor_file[i],cursor_theme,cursor_size);
			if (img) {
				cursors[i]=XcursorImageLoadCursor(x11_display,img);
				//print_line("found cursor: "+String(cursor_file[i])+" id "+itos(cursors[i]));
			} else {
				if (OS::is_stdout_verbose())
					print_line("failed cursor: "+String(cursor_file[i]));
			}
		}

	}


	{
		Pixmap cursormask;
		 XGCValues xgc;
		 GC gc;
		 XColor col;
		 Cursor cursor;

		 cursormask = XCreatePixmap(x11_display, RootWindow(x11_display,DefaultScreen(x11_display)), 1, 1, 1);
		 xgc.function = GXclear;
		 gc = XCreateGC(x11_display, cursormask, GCFunction, &xgc);
		XFillRectangle(x11_display, cursormask, gc, 0, 0, 1, 1);
		col.pixel = 0;
		col.red = 0;
		col.flags = 4;
		cursor = XCreatePixmapCursor(x11_display,
			      cursormask, cursormask,
			      &col, &col, 0, 0);
		 XFreePixmap(x11_display, cursormask);
		 XFreeGC(x11_display, gc);



		 if (cursor == None)
		 {
			 ERR_PRINT("FAILED CREATING CURSOR");
		 }

		 null_cursor=cursor;
	}
	set_cursor_shape(CURSOR_BUSY);


	visual_server->init();
	//
	physics_server = memnew( PhysicsServerSW );
	physics_server->init();
	//physics_2d_server = memnew( Physics2DServerSW );
	physics_2d_server = Physics2DServerWrapMT::init_server<Physics2DServerSW>();
	physics_2d_server->init();

	input = memnew( InputDefault );

	probe_joystick();

	_ensure_data_dir();

	net_wm_icon = XInternAtom(x11_display, "_NET_WM_ICON", False);



	//printf("got map notify\n");
		
}
void OS_X11::finalize() {

	if(main_loop)
		memdelete(main_loop);
	main_loop=NULL;

	spatial_sound_server->finish();
	memdelete(spatial_sound_server);
	spatial_sound_2d_server->finish();
	memdelete(spatial_sound_2d_server);

	//if (debugger_connection_console) {
//		memdelete(debugger_connection_console);
//}

	audio_server->finish();
	memdelete(audio_server);
	memdelete(sample_manager);

	visual_server->finish();
	memdelete(visual_server);
	memdelete(rasterizer);
	
	physics_server->finish();
	memdelete(physics_server);

	physics_2d_server->finish();
	memdelete(physics_2d_server);

	memdelete(input);

#if defined(OPENGL_ENABLED) || defined(LEGACYGL_ENABLED)
	memdelete(context_gl);
#endif
	

	XCloseDisplay(x11_display);
	if (xmbstring)
		memfree(xmbstring);
		
	args.clear();
}


void OS_X11::set_mouse_mode(MouseMode p_mode) {

	if (p_mode==mouse_mode)
		return;

	if (mouse_mode==MOUSE_MODE_CAPTURED)
		XUngrabPointer(x11_display, CurrentTime);
	if (mouse_mode!=MOUSE_MODE_VISIBLE && p_mode==MOUSE_MODE_VISIBLE)
		XUndefineCursor(x11_display,x11_window);
	if (p_mode!=MOUSE_MODE_VISIBLE && mouse_mode==MOUSE_MODE_VISIBLE) {
		XDefineCursor(x11_display,x11_window,null_cursor);
	}

	mouse_mode=p_mode;

	if (mouse_mode==MOUSE_MODE_CAPTURED) {

		while(true) {
			//flush pending motion events

			if (XPending(x11_display) > 0) {
				XEvent event;
				XPeekEvent(x11_display, &event);
				if (event.type==MotionNotify) {
					XNextEvent(x11_display,&event);
				} else {
					break;
				}
			} else {
				break;
			}
		}

		if (XGrabPointer(x11_display, x11_window, True,
				    ButtonPressMask | ButtonReleaseMask |
				    PointerMotionMask, GrabModeAsync, GrabModeAsync,
				    x11_window, None, CurrentTime) !=
		       GrabSuccess)  {
			ERR_PRINT("NO GRAB");
		}

		center.x = current_videomode.width/2;
		center.y = current_videomode.height/2;
		XWarpPointer(x11_display, None, x11_window,
			      0,0,0,0, (int)center.x, (int)center.y);

		input->set_mouse_pos(center);
	} else {
		do_mouse_warp=false;
	}
}

void OS_X11::warp_mouse_pos(const Point2& p_to) {

	if (mouse_mode==MOUSE_MODE_CAPTURED) {

		last_mouse_pos=p_to;
	} else {

		/*XWindowAttributes xwa;
		XGetWindowAttributes(x11_display, x11_window, &xwa);
		printf("%d %d\n", xwa.x, xwa.y); needed? */

		XWarpPointer(x11_display, None, x11_window,
			      0,0,0,0, (int)p_to.x , (int)p_to.y);
	}

}

OS::MouseMode OS_X11::get_mouse_mode() const {

	return mouse_mode;
}



int OS_X11::get_mouse_button_state() const {
	return last_button_state;
}

Point2 OS_X11::get_mouse_pos() const {

	return last_mouse_pos;
}

void OS_X11::set_window_title(const String& p_title) {

	XStoreName(x11_display,x11_window,p_title.utf8().get_data());
}

void OS_X11::set_video_mode(const VideoMode& p_video_mode,int p_screen) {


}
OS::VideoMode OS_X11::get_video_mode(int p_screen) const {

	return current_videomode;
}
void OS_X11::get_fullscreen_mode_list(List<VideoMode> *p_list,int p_screen) const {


}

//#ifdef NEW_WM_API
#if 0
// Just now not needed. Can be used for a possible OS.set_border(bool) method
void OS_X11::set_wm_border(bool p_enabled) {
	// needed for lxde/openbox, possibly others
	Hints hints;
	Atom property;
	hints.flags = 2;
	hints.decorations = p_enabled ? 1L : 0L;
	property = XInternAtom(x11_display, "_MOTIF_WM_HINTS", True);
	XChangeProperty(x11_display, x11_window, property, property, 32, PropModeReplace, (unsigned char *)&hints, 5);
	XMapRaised(x11_display, x11_window);
	//XMoveResizeWindow(x11_display, x11_window, 0, 0, 800, 800);
}
#endif

void OS_X11::set_wm_fullscreen(bool p_enabled) {
	// Using EWMH -- Extened Window Manager Hints
	XEvent xev;
	Atom wm_state = XInternAtom(x11_display, "_NET_WM_STATE", False);
	Atom wm_fullscreen = XInternAtom(x11_display, "_NET_WM_STATE_FULLSCREEN", False);

	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = x11_window;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = p_enabled ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
	xev.xclient.data.l[1] = wm_fullscreen;
	xev.xclient.data.l[2] = 0;

	XSendEvent(x11_display, DefaultRootWindow(x11_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
}

int OS_X11::get_screen_count() const {
	// Using Xinerama Extension
	int event_base, error_base;
	const Bool ext_okay = XineramaQueryExtension(x11_display, &event_base, &error_base);
	if( !ext_okay ) return 0;
	
	int count;
	XineramaScreenInfo* xsi = XineramaQueryScreens(x11_display, &count);
	XFree(xsi);
	return count;
}

int OS_X11::get_current_screen() const {
        int x,y;
        Window child;
        XTranslateCoordinates( x11_display, x11_window, DefaultRootWindow(x11_display), 0, 0, &x, &y, &child);

	int count = get_screen_count();
	for(int i=0; i<count; i++) {
		Point2i pos = get_screen_position(i);
		Size2i size = get_screen_size(i);
		if( (x >= pos.x && x <pos.x + size.width) && (y >= pos.y && y < pos.y + size.height) )
			return i;	
	}
	return 0;
}

void OS_X11::set_current_screen(int p_screen) {
	int count = get_screen_count();
	if(p_screen >= count) return;
		
	if( current_videomode.fullscreen ) {
		Point2i position = get_screen_position(p_screen);
		Size2i size = get_screen_size(p_screen);

	        XMoveResizeWindow(x11_display, x11_window, position.x, position.y, size.x, size.y);
	}
	else {
		if( p_screen != get_current_screen() ) {
			Point2i position = get_screen_position(p_screen);
			XMoveWindow(x11_display, x11_window, position.x, position.y);
		}
	}
}

Point2 OS_X11::get_screen_position(int p_screen) const {

	// Using Xinerama Extension
	int event_base, error_base;
	const Bool ext_okay = XineramaQueryExtension(x11_display, &event_base, &error_base);
	if( !ext_okay ) {
		return Point2i(0,0);
	}
	
	int count;
	XineramaScreenInfo* xsi = XineramaQueryScreens(x11_display, &count);
	if( p_screen >= count ) {
		return Point2i(0,0);
	}
	
	Point2i position = Point2i(xsi[p_screen].x_org, xsi[p_screen].y_org);

	XFree(xsi);

	return position;
}

Size2 OS_X11::get_screen_size(int p_screen) const {
	// Using Xinerama Extension
	int event_base, error_base;
	const Bool ext_okay = XineramaQueryExtension(x11_display, &event_base, &error_base);
	if( !ext_okay ) return Size2i(0,0);
	
	int count;
	XineramaScreenInfo* xsi = XineramaQueryScreens(x11_display, &count);
	if( p_screen >= count ) return Size2i(0,0);
	
	Size2i size = Point2i(xsi[p_screen].width, xsi[p_screen].height);
	XFree(xsi);
	return size;
}
	

Point2 OS_X11::get_window_position() const {
	int x,y;
	Window child;
	XTranslateCoordinates( x11_display, x11_window, DefaultRootWindow(x11_display), 0, 0, &x, &y, &child);

	int screen = get_current_screen();
	Point2i screen_position = get_screen_position(screen);

	return Point2i(x-screen_position.x, y-screen_position.y);		
}

void OS_X11::set_window_position(const Point2& p_position) {
	// Using EWMH -- Extended Window Manager Hints
	// to get the size of the decoration 
#if 0
	Atom property = XInternAtom(x11_display,"_NET_FRAME_EXTENTS", True);
	Atom type;
	int format;
	unsigned long len;
	unsigned long remaining;
	unsigned char *data = NULL;
	int result;

	result = XGetWindowProperty(
		x11_display,
		x11_window,
		property,
		0,
		32,
		False,
		AnyPropertyType,
		&type,
		&format,
		&len,
		&remaining,
		&data
	);	

	long left = 0L;
	long top = 0L;

	if( result == Success ) {
		long *extends = (long *) data;
	
		left = extends[0];
		top = extends[2];
	
		XFree(data);
	}

	int screen = get_current_screen();
	Point2i screen_position = get_screen_position(screen);

	left -= screen_position.x;
	top -= screen_position.y;

	XMoveWindow(x11_display,x11_window,p_position.x - left,p_position.y - top);
#else
	XMoveWindow(x11_display,x11_window,p_position.x,p_position.y);
#endif
}

Size2 OS_X11::get_window_size() const {
	XWindowAttributes xwa;
	XGetWindowAttributes(x11_display, x11_window, &xwa);
	return Size2i(xwa.width, xwa.height);
}

void OS_X11::set_window_size(const Size2 p_size) {
	XResizeWindow(x11_display, x11_window, p_size.x, p_size.y);
}

void OS_X11::set_window_fullscreen(bool p_enabled) {
	set_wm_fullscreen(p_enabled);
	current_videomode.fullscreen = p_enabled;
}

bool OS_X11::is_window_fullscreen() const {
	return current_videomode.fullscreen;
}

void OS_X11::set_window_resizable(bool p_enabled) {
	XSizeHints *xsh;
	xsh = XAllocSizeHints();
	xsh->flags = p_enabled ? 0L : PMinSize | PMaxSize;
	if(!p_enabled) {
		XWindowAttributes xwa;
		XGetWindowAttributes(x11_display,x11_window,&xwa);
		xsh->min_width = xwa.width; 
		xsh->max_width = xwa.width;
		xsh->min_height = xwa.height;
		xsh->max_height = xwa.height;
	}
	XSetWMNormalHints(x11_display, x11_window, xsh);
	XFree(xsh);
	current_videomode.resizable = p_enabled;
}

bool OS_X11::is_window_resizable() const {
	return current_videomode.resizable;
}

void OS_X11::set_window_minimized(bool p_enabled) {
        // Using ICCCM -- Inter-Client Communication Conventions Manual
        XEvent xev;
        Atom wm_change = XInternAtom(x11_display, "WM_CHANGE_STATE", False);

        memset(&xev, 0, sizeof(xev));
        xev.type = ClientMessage;
        xev.xclient.window = x11_window;
        xev.xclient.message_type = wm_change;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = p_enabled ? WM_IconicState : WM_NormalState;

        XSendEvent(x11_display, DefaultRootWindow(x11_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
	
        //XEvent xev;
	Atom wm_state     =  XInternAtom(x11_display, "_NET_WM_STATE", False);
	Atom wm_hidden    =  XInternAtom(x11_display, "_NET_WM_STATE_HIDDEN", False);

	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = x11_window;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = _NET_WM_STATE_ADD;
	xev.xclient.data.l[1] = wm_hidden;

	XSendEvent(x11_display, DefaultRootWindow(x11_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);	
}

bool OS_X11::is_window_minimized() const {
	// Using ICCCM -- Inter-Client Communication Conventions Manual
        Atom property = XInternAtom(x11_display,"WM_STATE", True);
        Atom type;
        int format;
        unsigned long len;
        unsigned long remaining;
        unsigned char *data = NULL;

        int result = XGetWindowProperty(
                x11_display,
                x11_window,
                property,
                0,
                32,
                False,
                AnyPropertyType,
                &type,
                &format,
                &len,
                &remaining,
                &data
        );

	if( result == Success ) {
		long *state = (long *) data;
		if( state[0] == WM_IconicState ) 
			return true;
	}
	return false;
}

void OS_X11::set_window_maximized(bool p_enabled) {
	// Using EWMH -- Extended Window Manager Hints 
	XEvent xev;
	Atom wm_state = XInternAtom(x11_display, "_NET_WM_STATE", False);
	Atom wm_max_horz = XInternAtom(x11_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
	Atom wm_max_vert = XInternAtom(x11_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);

	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = x11_window;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = p_enabled ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
	xev.xclient.data.l[1] = wm_max_horz;
	xev.xclient.data.l[2] = wm_max_vert;

	XSendEvent(x11_display, DefaultRootWindow(x11_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
/* sorry this does not fix it, fails on multi monitor
	XWindowAttributes xwa;
	XGetWindowAttributes(x11_display,DefaultRootWindow(x11_display),&xwa);
	current_videomode.width = xwa.width;
	current_videomode.height = xwa.height;
*/
	maximized = p_enabled;

}

bool OS_X11::is_window_maximized() const {
	// Using EWMH -- Extended Window Manager Hints
        Atom property = XInternAtom(x11_display,"_NET_WM_STATE",False );
        Atom type;
        int format;
        unsigned long len;
        unsigned long remaining;
        unsigned char *data = NULL;

        int result = XGetWindowProperty(
                x11_display,
                x11_window,
                property,
                0,
                1024,
                False,
                XA_ATOM,
                &type,
                &format,
                &len,
                &remaining,
                &data
        );

	if(result == Success) { 
		Atom *atoms = (Atom*) data;
		Atom wm_max_horz = XInternAtom(x11_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
		Atom wm_max_vert = XInternAtom(x11_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
		bool found_wm_max_horz = false;
		bool found_wm_max_vert = false;

		for( unsigned int i=0; i < len; i++ ) {
			if( atoms[i] == wm_max_horz )
				found_wm_max_horz = true;
			if( atoms[i] == wm_max_vert )
				found_wm_max_vert = true;

			if( found_wm_max_horz && found_wm_max_vert )
				return true;
		}
		XFree(atoms);
	}

	return false;
}


InputModifierState OS_X11::get_key_modifier_state(unsigned int p_x11_state) {
	
	InputModifierState state;
	
	state.shift = (p_x11_state&ShiftMask);
	state.control = (p_x11_state&ControlMask);
	state.alt = (p_x11_state&Mod1Mask /*|| p_x11_state&Mod5Mask*/); //altgr should not count as alt
	state.meta = (p_x11_state&Mod4Mask);
	
	return state;
}

unsigned int OS_X11::get_mouse_button_state(unsigned int p_x11_state) {

	unsigned int state=0;
		
	if (p_x11_state&Button1Mask) {
		
		state|=1<<0;
	}

	if (p_x11_state&Button3Mask) {
		
		state|=1<<1;
	}

	if (p_x11_state&Button2Mask) {
		
		state|=1<<2;
	}
	
	if (p_x11_state&Button4Mask) {
		
		state|=1<<3;
	}

	if (p_x11_state&Button5Mask) {
		
		state|=1<<4;
	}

	last_button_state=state;
	return state;
}
	
void OS_X11::handle_key_event(XKeyEvent *p_event, bool p_echo) {

			
	// X11 functions don't know what const is
	XKeyEvent *xkeyevent = p_event;
	
	// This code was pretty difficult to write.
	// The docs stink and every toolkit seems to
	// do it in a different way. 
	
	/* Phase 1, obtain a proper keysym */
	
	// This was also very difficult to figure out.
	// You'd expect you could just use Keysym provided by
	// XKeycodeToKeysym to obtain internationalized 
	// input.. WRONG!! 
	// you must use XLookupString (???) which not only wastes
	// cycles generating an unnecesary string, but also
	// still works in half the cases. (won't handle deadkeys)
	// For more complex input methods (deadkeys and more advanced)
	// you have to use XmbLookupString (??).
	// So.. then you have to chosse which of both results
	// you want to keep.
	// This is a real bizarreness and cpu waster.
		
	KeySym keysym_keycode=0; // keysym used to find a keycode
	KeySym keysym_unicode=0; // keysym used to find unicode
					
	// XLookupString returns keysyms usable as nice scancodes/
	char str[256+1];
	XLookupString(xkeyevent, str, 256, &keysym_keycode, NULL);
						 
 	// Meanwhile, XLookupString returns keysyms useful for unicode.
	
	
	if (!xmbstring) {
		// keep a temporary buffer for the string
		xmbstring=(char*)memalloc(sizeof(char)*8);
		xmblen=8;
	}			 
	
	if (xkeyevent->type == KeyPress && xic) {

		Status status;
		do {
			
			int mnbytes = XmbLookupString (xic, xkeyevent, xmbstring, xmblen - 1, &keysym_unicode, &status);
			xmbstring[mnbytes] = '\0';

			if (status == XBufferOverflow) {
				xmblen = mnbytes + 1;
				xmbstring = (char*)memrealloc (xmbstring, xmblen);
			} 
		} while (status == XBufferOverflow);
	} 		

	
	/* Phase 2, obtain a pigui keycode from the keysym */
	
	// KeyMappingX11 just translated the X11 keysym to a PIGUI
	// keysym, so it works in all platforms the same.

	unsigned int keycode = KeyMappingX11::get_keycode(keysym_keycode);
	
	/* Phase 3, obtain an unicode character from the keysym */
	
	// KeyMappingX11 also translates keysym to unicode.
	// It does a binary search on a table to translate
	// most properly. 
	//print_line("keysym_unicode: "+rtos(keysym_unicode));
	unsigned int unicode = keysym_unicode>0? KeyMappingX11::get_unicode_from_keysym(keysym_unicode):0;
	

	/* Phase 4, determine if event must be filtered */
	
	// This seems to be a side-effect of using XIM.
	// XEventFilter looks like a core X11 funciton,
	// but it's actually just used to see if we must
	// ignore a deadkey, or events XIM determines
	// must not reach the actual gui.
	// Guess it was a design problem of the extension

	bool keypress = xkeyevent->type == KeyPress;
	
	if (xkeyevent->type == KeyPress && xic) {
                if (XFilterEvent((XEvent*)xkeyevent, x11_window))
                	return;  
	}
	
	if (keycode==0 && unicode==0)
		return;

	/* Phase 5, determine modifier mask */
		
	// No problems here, except I had no way to
	// know Mod1 was ALT and Mod4 was META (applekey/winkey)
	// just tried Mods until i found them.

	//print_line("mod1: "+itos(xkeyevent->state&Mod1Mask)+" mod 5: "+itos(xkeyevent->state&Mod5Mask));
	
	InputModifierState state = get_key_modifier_state(xkeyevent->state);
	
	/* Phase 6, determine echo character */
	
	// Echo characters in X11 are a keyrelease and a keypress
	// one after the other with the (almot) same timestamp.
	// To detect them, i use XPeekEvent and check that their
	// difference in time is below a treshold.
	

	if (xkeyevent->type != KeyPress) {
				
		// make sure there are events pending,
		// so this call won't block.
		if (XPending(x11_display)>0) {
			XEvent peek_event;
			XPeekEvent(x11_display, &peek_event);
			
			// I'm using a treshold of 5 msecs, 
			// since sometimes there seems to be a little
			// jitter. I'm still not convinced that all this approach
			// is correct, but the xorg developers are
			// not very helpful today.
			
			::Time tresh=ABS(peek_event.xkey.time-xkeyevent->time);
			if (peek_event.type == KeyPress && tresh<5 ) {
				KeySym rk;
				XLookupString((XKeyEvent*)&peek_event, str, 256, &rk, NULL);
				if (rk==keysym_keycode) {
					XEvent event;
					XNextEvent(x11_display, &event); //erase next event
					handle_key_event( (XKeyEvent*)&event,true );
					return; //ignore current, echo next
				}
			}
				
			// use the time from peek_event so it always works
		}
	
		// save the time to check for echo when keypress happens		
	}
	
	
	/* Phase 7, send event to Window */
	
	InputEvent event;
	event.ID=++event_id;
	event.type = InputEvent::KEY;
	event.device=0;
	event.key.mod=state;
	event.key.pressed=keypress;

	if (keycode>='a' && keycode<='z')
		keycode-='a'-'A';

	event.key.scancode=keycode;
	event.key.unicode=unicode;
	event.key.echo=p_echo;

	if (event.key.scancode==KEY_BACKTAB) {
		//make it consistent accross platforms.
		event.key.scancode=KEY_TAB;
		event.key.mod.shift=true;
	}

	//printf("key: %x\n",event.key.scancode);
	input->parse_input_event( event);

	
}

void OS_X11::process_xevents() {

	//printf("checking events %i\n", XPending(x11_display));

	do_mouse_warp=false;

	while (XPending(x11_display) > 0) {
		XEvent event;
		XNextEvent(x11_display, &event);

		switch (event.type) {
		case Expose:
			Main::force_redraw();
			break;

		case NoExpose:
			minimized = true;
			break;

		case VisibilityNotify: {
			XVisibilityEvent * visibility = (XVisibilityEvent *)&event;
			minimized = (visibility->state == VisibilityFullyObscured);
		} break;
		case LeaveNotify: {

			if (main_loop && mouse_mode!=MOUSE_MODE_CAPTURED)
				main_loop->notification(MainLoop::NOTIFICATION_WM_MOUSE_EXIT);
			if (input)
				input->set_mouse_in_window(false);

		} break;
		case EnterNotify: {

			if (main_loop && mouse_mode!=MOUSE_MODE_CAPTURED)
				main_loop->notification(MainLoop::NOTIFICATION_WM_MOUSE_ENTER);
			if (input)
				input->set_mouse_in_window(true);
		} break;
		case FocusIn:
			minimized = false;
#ifdef NEW_WM_API
			if(current_videomode.fullscreen) {
				set_wm_fullscreen(true);
			}
#endif
			main_loop->notification(MainLoop::NOTIFICATION_WM_FOCUS_IN);
			if (mouse_mode==MOUSE_MODE_CAPTURED) {
				XGrabPointer(x11_display, x11_window, True,
						    ButtonPressMask | ButtonReleaseMask |
						    PointerMotionMask, GrabModeAsync, GrabModeAsync,
						    x11_window, None, CurrentTime);
			}
			break;

		case FocusOut:
#ifdef NEW_WM_API
			if(current_videomode.fullscreen) {
				set_wm_fullscreen(false);
				set_window_minimized(true);
			}
#endif
			main_loop->notification(MainLoop::NOTIFICATION_WM_FOCUS_OUT);
			if (mouse_mode==MOUSE_MODE_CAPTURED) {
				//dear X11, I try, I really try, but you never work, you do whathever you want.
				XUngrabPointer(x11_display, CurrentTime);
			}
			break;

		case ConfigureNotify:
		/* call resizeGLScene only if our window-size changed */
		
			if ((event.xconfigure.width == current_videomode.width) && 
			(event.xconfigure.height == current_videomode.height))
				break;
				
			current_videomode.width=event.xconfigure.width;
			current_videomode.height=event.xconfigure.height;
			break;
		case ButtonPress:
		case ButtonRelease: {
			
			/* exit in case of a mouse button press */
			last_timestamp=event.xbutton.time;
			if (mouse_mode==MOUSE_MODE_CAPTURED) {
				event.xbutton.x=last_mouse_pos.x;
				event.xbutton.y=last_mouse_pos.y;
			}
		
			InputEvent mouse_event;
			mouse_event.ID=++event_id;
			mouse_event.type = InputEvent::MOUSE_BUTTON;
			mouse_event.device=0;
			mouse_event.mouse_button.mod = get_key_modifier_state(event.xbutton.state);
			mouse_event.mouse_button.button_mask = get_mouse_button_state(event.xbutton.state);
			mouse_event.mouse_button.x=event.xbutton.x;
			mouse_event.mouse_button.y=event.xbutton.y;
			mouse_event.mouse_button.global_x=event.xbutton.x;
			mouse_event.mouse_button.global_y=event.xbutton.y;
			mouse_event.mouse_button.button_index=event.xbutton.button;
			if (mouse_event.mouse_button.button_index==2)
				mouse_event.mouse_button.button_index=3;
			else if (mouse_event.mouse_button.button_index==3)
				mouse_event.mouse_button.button_index=2;
				
			mouse_event.mouse_button.pressed=(event.type==ButtonPress);


			if (event.type==ButtonPress && event.xbutton.button==1) {
				
				uint64_t diff = get_ticks_usec()/1000 - last_click_ms;

				if (diff<400 && Point2(last_click_pos).distance_to(Point2(event.xbutton.x,event.xbutton.y))<5) {
					
					last_click_ms=0;
					last_click_pos = Point2(-100,-100);
					mouse_event.mouse_button.doubleclick=true;					
					mouse_event.ID=++event_id;
					
				} else {
					last_click_ms+=diff;	
					last_click_pos = Point2(event.xbutton.x,event.xbutton.y);
				}
			}	

			input->parse_input_event( mouse_event);

			
		} break;	
		case MotionNotify: {

			// FUCK YOU X11 API YOU SERIOUSLY GROSS ME OUT
			// YOU ARE AS GROSS AS LOOKING AT A PUTRID PILE
			// OF POOP STICKING OUT OF A CLOGGED TOILET
			// HOW THE FUCK I AM SUPPOSED TO KNOW WHICH ONE
			// OF THE MOTION NOTIFY EVENTS IS THE ONE GENERATED
			// BY WARPING THE MOUSE POINTER?
			// YOU ARE FORCING ME TO FILTER ONE BY ONE TO FIND IT
			// PLEASE DO ME A FAVOR AND DIE DROWNED IN A FECAL
			// MOUNTAIN BECAUSE THAT'S WHERE YOU BELONG.

			
			while(true) {
				if (mouse_mode==MOUSE_MODE_CAPTURED && event.xmotion.x==current_videomode.width/2 && event.xmotion.y==current_videomode.height/2) {
					//this is likely the warp event since it was warped here
					center=Vector2(event.xmotion.x,event.xmotion.y);
					break;
				}

				if (XPending(x11_display) > 0) {
					XEvent tevent;
					XPeekEvent(x11_display, &tevent);
					if (tevent.type==MotionNotify) {
						XNextEvent(x11_display,&event);
					} else {
						break;
					}
				} else {
					break;
				}
			}

			last_timestamp=event.xmotion.time;
		
			// Motion is also simple.
			// A little hack is in order
			// to be able to send relative motion events.
			Point2i pos( event.xmotion.x, event.xmotion.y );

			if (mouse_mode==MOUSE_MODE_CAPTURED) {
#if 1
				//Vector2 c = Point2i(current_videomode.width/2,current_videomode.height/2);
				if (pos==Point2i(current_videomode.width/2,current_videomode.height/2)) {
					//this sucks, it's a hack, etc and is a little inaccurate, etc.
					//but nothing I can do, X11 sucks.

					center=pos;
					break;
				}

				Point2i new_center = pos;
				pos = last_mouse_pos + ( pos - center );
				center=new_center;
				do_mouse_warp=true;
#else
				//Dear X11, thanks for making my life miserable

				center.x = current_videomode.width/2;
				center.y = current_videomode.height/2;
				pos = last_mouse_pos + ( pos-center );
				if (pos==last_mouse_pos)
					break;
				XWarpPointer(x11_display, None, x11_window,
					      0,0,0,0, (int)center.x, (int)center.y);
#endif
			}
			
			if (!last_mouse_pos_valid) {
				
				last_mouse_pos=pos;
				last_mouse_pos_valid=true;
			}
			
			Point2i rel = pos - last_mouse_pos;

#ifdef NEW_WM_API
			if (mouse_mode==MOUSE_MODE_CAPTURED) {
				pos.x = current_videomode.width / 2;
				pos.y = current_videomode.height / 2;
			}
#endif

			InputEvent motion_event;
			motion_event.ID=++event_id;
			motion_event.type=InputEvent::MOUSE_MOTION;
			motion_event.device=0;
			
			motion_event.mouse_motion.mod = get_key_modifier_state(event.xmotion.state);
			motion_event.mouse_motion.button_mask = get_mouse_button_state(event.xmotion.state);
			motion_event.mouse_motion.x=pos.x;
			motion_event.mouse_motion.y=pos.y;
			input->set_mouse_pos(pos);
			motion_event.mouse_motion.global_x=pos.x;
			motion_event.mouse_motion.global_y=pos.y;
			motion_event.mouse_motion.speed_x=input->get_mouse_speed().x;
			motion_event.mouse_motion.speed_y=input->get_mouse_speed().y;

			motion_event.mouse_motion.relative_x=rel.x;
			motion_event.mouse_motion.relative_y=rel.y;
						
			last_mouse_pos=pos;

			// printf("rel: %d,%d\n", rel.x, rel.y );
			
			input->parse_input_event( motion_event);
			
		} break;			
		case KeyPress: 
		case KeyRelease: {

			last_timestamp=event.xkey.time;
				
			// key event is a little complex, so
			// it will be handled in it's own function.
			handle_key_event( (XKeyEvent*)&event );
		} break;			
		case SelectionRequest: {

			XSelectionRequestEvent *req;
			XEvent e, respond;
			e = event;

			req=&(e.xselectionrequest);
			if (req->target == XA_STRING || req->target == XInternAtom(x11_display, "COMPOUND_TEXT", 0) ||
				req->target == XInternAtom(x11_display, "UTF8_STRING", 0))
			{
				CharString clip = OS::get_clipboard().utf8();
				XChangeProperty (x11_display,
					req->requestor,
					req->property,
					req->target,
					8,
					PropModeReplace,
					(unsigned char*)clip.get_data(),
					clip.length());
				respond.xselection.property=req->property;
			} else if (req->target == XInternAtom(x11_display, "TARGETS", 0)) {

				Atom data[2];
				data[0] = XInternAtom(x11_display, "UTF8_STRING", 0);
				data[1] = XA_STRING;
				XChangeProperty (x11_display, req->requestor, req->property, req->target,
						 8, PropModeReplace, (unsigned char *) &data,
						 sizeof (data));
				respond.xselection.property=req->property;

			} else {
				printf ("No String %x\n",
					(int)req->target);
				respond.xselection.property= None;
			}
			respond.xselection.type= SelectionNotify;
			respond.xselection.display= req->display;
			respond.xselection.requestor= req->requestor;
			respond.xselection.selection=req->selection;
			respond.xselection.target= req->target;
			respond.xselection.time = req->time;
			XSendEvent (x11_display, req->requestor,0,0,&respond);
			XFlush (x11_display);
		} break;


		case ClientMessage:    
		
			if ((unsigned int)event.xclient.data.l[0]==(unsigned int)wm_delete)
				main_loop->notification(MainLoop::NOTIFICATION_WM_QUIT_REQUEST);
			break;
		default:
			break;
		}
	}
	
	XFlush(x11_display);

	if (do_mouse_warp) {

		XWarpPointer(x11_display, None, x11_window,
		 	      0,0,0,0, (int)current_videomode.width/2, (int)current_videomode.height/2);

		/*	
		Window root, child;
		int root_x, root_y;
		int win_x, win_y;
		unsigned int mask;
		XQueryPointer( x11_display, x11_window, &root, &child, &root_x, &root_y, &win_x, &win_y, &mask );

		printf("Root: %d,%d\n", root_x, root_y);
		printf("Win: %d,%d\n", win_x, win_y);
		*/
	}
}

MainLoop *OS_X11::get_main_loop() const {

	return main_loop;
}

void OS_X11::delete_main_loop() {

	if (main_loop)
		memdelete(main_loop);
	main_loop=NULL;
}

void OS_X11::set_main_loop( MainLoop * p_main_loop ) {

	main_loop=p_main_loop;
	input->set_main_loop(p_main_loop);
}

bool OS_X11::can_draw() const {

	return !minimized;
};

void OS_X11::set_clipboard(const String& p_text) {

	OS::set_clipboard(p_text);

	XSetSelectionOwner(x11_display, XA_PRIMARY, x11_window, CurrentTime);
	XSetSelectionOwner(x11_display, XInternAtom(x11_display, "CLIPBOARD", 0), x11_window, CurrentTime);
};

static String _get_clipboard(Atom p_source, Window x11_window, ::Display* x11_display, String p_internal_clipboard) {

	String ret;

	Atom type;
	Atom selection = XA_PRIMARY;
	int format, result;
	unsigned long len, bytes_left, dummy;
	unsigned char *data;
	Window Sown = XGetSelectionOwner (x11_display, p_source);

	if (Sown == x11_window) {

		printf("returning internal clipboard\n");
		return p_internal_clipboard;
	};

	if (Sown != None) {
		XConvertSelection (x11_display, p_source, XA_STRING, selection,
					 x11_window, CurrentTime);
		XFlush (x11_display);
		while (true) {
			XEvent event;
			XNextEvent(x11_display, &event);
			if (event.type == SelectionNotify && event.xselection.requestor == x11_window) {
				break;
			};
		};

		//
		// Do not get any data, see how much data is there
		//
		XGetWindowProperty (x11_display, x11_window,
			selection, 	  // Tricky..
			0, 0,	  	  // offset - len
			0, 	 	  // Delete 0==FALSE
			AnyPropertyType,  //flag
			&type,		  // return type
			&format,	  // return format
			&len, &bytes_left,  //that
			&data);
		// DATA is There
		if (bytes_left > 0)
		{
			result = XGetWindowProperty (x11_display, x11_window,
				selection, 0,bytes_left,0,
				AnyPropertyType, &type,&format,
				&len, &dummy, &data);
			if (result == Success) {
				ret.parse_utf8((const char*)data);
			} else printf ("FAIL\n");
			XFree (data);
		}
	}

	return ret;

};

String OS_X11::get_clipboard() const {

	String ret;
	ret = _get_clipboard(XInternAtom(x11_display, "CLIPBOARD", 0), x11_window, x11_display, OS::get_clipboard());

	if (ret == "") {
		ret = _get_clipboard(XA_PRIMARY, x11_window, x11_display, OS::get_clipboard());
	};

	return ret;
};

String OS_X11::get_name() {

	return "X11";
}

Error OS_X11::shell_open(String p_uri) {

	Error ok;
	List<String> args;
	args.push_back(p_uri);
	ok = execute("/usr/bin/xdg-open",args,false);
	if (ok==OK)
		return OK;
	ok = execute("gnome-open",args,false);
	if (ok==OK)
		return OK;
	ok = execute("kde-open",args,false);
	return ok;
}

String OS_X11::get_system_dir(SystemDir p_dir) const {


	String xdgparam;

	switch(p_dir) {
		case SYSTEM_DIR_DESKTOP: {

			xdgparam="DESKTOP";
		} break;
		case SYSTEM_DIR_DCIM: {

			xdgparam="PICTURES";

		} break;
		case SYSTEM_DIR_DOCUMENTS: {

			xdgparam="DOCUMENTS";

		} break;
		case SYSTEM_DIR_DOWNLOADS: {

			xdgparam="DOWNLOAD";

		} break;
		case SYSTEM_DIR_MOVIES: {

			xdgparam="VIDEOS";

		} break;
		case SYSTEM_DIR_MUSIC: {

			xdgparam="MUSIC";

		} break;
		case SYSTEM_DIR_PICTURES: {

			xdgparam="PICTURES";

		} break;
		case SYSTEM_DIR_RINGTONES: {

			xdgparam="MUSIC";

		} break;
	}

	String pipe;
	List<String> arg;
	arg.push_back(xdgparam);
	Error err = const_cast<OS_X11*>(this)->execute("/usr/bin/xdg-user-dir",arg,true,NULL,&pipe);
	if (err!=OK)
		return ".";
	return pipe.strip_edges();
}


void OS_X11::close_joystick(int p_id) {

	if (p_id == -1) {
		for (int i=0; i<JOYSTICKS_MAX; i++) {

			close_joystick(i);
		};
		return;
	};


	if (joysticks[p_id].fd != -1) {
		close(joysticks[p_id].fd);
		joysticks[p_id].fd = -1;
	};
	input->joy_connection_changed(p_id, false, "");
};

void OS_X11::probe_joystick(int p_id) {
	#ifndef __FreeBSD__

	if (p_id == -1) {

		for (int i=0; i<JOYSTICKS_MAX; i++) {

			probe_joystick(i);
		};
		return;
	};

	if (joysticks[p_id].fd != -1)
		close_joystick(p_id);

	const char *joy_names[] = {
		"/dev/input/js%d",
		"/dev/js%d",
		NULL
	};

	int i=0;
	while(joy_names[i]) {

		char fname[64];
		sprintf(fname, joy_names[i], p_id);
		int fd = open(fname, O_RDONLY|O_NONBLOCK);
		if (fd != -1) {

			//fcntl( fd, F_SETFL, O_NONBLOCK );
			joysticks[p_id] = Joystick(); // this will reset the axis array
			joysticks[p_id].fd = fd;

			String name;
			char namebuf[255] = {0};
			if (ioctl(fd, JSIOCGNAME(sizeof(namebuf)), namebuf) >= 0) {
				name = namebuf;
			} else {
				name = "error";
			};

			input->joy_connection_changed(p_id, true, name);
			break; // don't try the next name
		};

		++i;
	};
	#endif
};

void OS_X11::move_window_to_foreground() {

	XRaiseWindow(x11_display,x11_window);
}

void OS_X11::process_joysticks() {
	#ifndef __FreeBSD__
	int bytes;
	js_event events[32];
	InputEvent ievent;
	for (int i=0; i<JOYSTICKS_MAX; i++) {

		if (joysticks[i].fd == -1) {
			probe_joystick(i);
			if (joysticks[i].fd == -1)
				continue;
		};
		ievent.device = i;

		while ( (bytes = read(joysticks[i].fd, &events, sizeof(events))) > 0) {

			int ev_count = bytes / sizeof(js_event);
			for (int j=0; j<ev_count; j++) {

				js_event& event = events[j];

				//printf("got event on joystick %i, %i, %i, %i, %i\n", i, joysticks[i].fd, event.type, event.number, event.value);
				if (event.type & JS_EVENT_INIT)
					continue;

				switch (event.type & ~JS_EVENT_INIT) {

				case JS_EVENT_AXIS:

					//if (joysticks[i].last_axis[event.number] != event.value) {

						/*
						if (event.number==5 || event.number==6) {

							int axis=event.number-5;
							int val = event.value;
							if (val<0)
								val=-1;
							if (val>0)
								val=+1;

							InputEvent ev;
							ev.type = InputEvent::JOYSTICK_BUTTON;
							ev.ID = ++event_id;


							if (val!=dpad_last[axis]) {

								int prev_val = dpad_last[axis];
								if (prev_val!=0) {

									ev.joy_button.pressed=false;
									ev.joy_button.pressure=0.0;
									if (event.number==5)
										ev.joy_button.button_index=JOY_DPAD_LEFT+(prev_val+1)/2;
									if (event.number==6)
										ev.joy_button.button_index=JOY_DPAD_UP+(prev_val+1)/2;

									input->parse_input_event( ev );
								}
							}

							if (val!=0) {

								ev.joy_button.pressed=true;
								ev.joy_button.pressure=1.0;
								if (event.number==5)
									ev.joy_button.button_index=JOY_DPAD_LEFT+(val+1)/2;
								if (event.number==6)
									ev.joy_button.button_index=JOY_DPAD_UP+(val+1)/2;

								input->parse_input_event( ev );
							}


							dpad_last[axis]=val;

						}
						*/
						//print_line("ev: "+itos(event.number)+" val: "+ rtos((float)event.value / (float)MAX_JOY_AXIS));
						//if (event.number >= JOY_AXIS_MAX)
						//	break;
						//ERR_FAIL_COND(event.number >= JOY_AXIS_MAX);
						ievent.type = InputEvent::JOYSTICK_MOTION;
						ievent.ID = ++event_id;
						ievent.joy_motion.axis = event.number; //_pc_joystick_get_native_axis(event.number);
						ievent.joy_motion.axis_value = (float)event.value / (float)MAX_JOY_AXIS;
						if (event.number < JOY_AXIS_MAX)
							joysticks[i].last_axis[event.number] = event.value;
						input->parse_input_event( ievent );
					//};
					break;

				case JS_EVENT_BUTTON:


					ievent.type = InputEvent::JOYSTICK_BUTTON;
					ievent.ID = ++event_id;
					ievent.joy_button.button_index = event.number; // _pc_joystick_get_native_button(event.number);
					ievent.joy_button.pressed = event.value;
					input->parse_input_event( ievent );
					break;
				};
			};
		};
		if (bytes == 0 || (bytes < 0 && errno != EAGAIN)) {
			close_joystick(i);
		};
	};
	#endif
};


void OS_X11::set_cursor_shape(CursorShape p_shape) {

	ERR_FAIL_INDEX(p_shape,CURSOR_MAX);

	if (p_shape==current_cursor)
		return;
	if (mouse_mode==MOUSE_MODE_VISIBLE) {
		if (cursors[p_shape]!=None)
			XDefineCursor(x11_display,x11_window,cursors[p_shape]);
		else if (cursors[CURSOR_ARROW]!=None)
			XDefineCursor(x11_display,x11_window,cursors[CURSOR_ARROW]);
	}


	current_cursor=p_shape;
}


void OS_X11::release_rendering_thread() {

	context_gl->release_current();

}

void OS_X11::make_rendering_thread() {

	context_gl->make_current();
}

void OS_X11::swap_buffers() {

	context_gl->swap_buffers();
}

void OS_X11::alert(const String& p_alert,const String& p_title) {

	List<String> args;
	args.push_back("-center");
	args.push_back("-title");
	args.push_back(p_title);
	args.push_back(p_alert);

	execute("/usr/bin/xmessage",args,true);
}

void OS_X11::set_icon(const Image& p_icon) {
	if (!p_icon.empty()) {
		Image img=p_icon;
		img.convert(Image::FORMAT_RGBA);

		int w = img.get_width();
		int h = img.get_height();

		// We're using long to have wordsize (32Bit build -> 32 Bits, 64 Bit build -> 64 Bits
		Vector<long> pd;

		pd.resize(2+w*h);

		pd[0]=w;
		pd[1]=h;

		DVector<uint8_t>::Read r = img.get_data().read();

		long * wr = &pd[2];
		uint8_t const * pr = r.ptr();

		for(int i=0;i<w*h;i++) {
			long v=0;
			//    A             R             G            B
			v|=pr[3] << 24 | pr[0] << 16 | pr[1] << 8 | pr[2];
			*wr++=v;
			pr += 4;
		}
		XChangeProperty(x11_display, x11_window, net_wm_icon, XA_CARDINAL, 32, PropModeReplace, (unsigned char*) pd.ptr(), pd.size());
	} else {
	    XDeleteProperty(x11_display, x11_window, net_wm_icon);
	}
	XFlush(x11_display);

}


void OS_X11::run() {

	force_quit = false;
	
	if (!main_loop)
		return;
		
	main_loop->init();
		
//	uint64_t last_ticks=get_ticks_usec();
	
//	int frames=0;
//	uint64_t frame=0;
	
	while (!force_quit) {
	
		process_xevents(); // get rid of pending events
		process_joysticks();
		if (Main::iteration()==true)
			break;
	};
	
	main_loop->finish();
}

OS_X11::OS_X11() {

#ifdef RTAUDIO_ENABLED
	AudioDriverManagerSW::add_driver(&driver_rtaudio);
#endif

#ifdef PULSEAUDIO_ENABLED
	AudioDriverManagerSW::add_driver(&driver_pulseaudio);
#endif

#ifdef ALSA_ENABLED
	AudioDriverManagerSW::add_driver(&driver_alsa);
#endif

	minimized = false;
	xim_style=0L;
	mouse_mode=MOUSE_MODE_VISIBLE;
};
