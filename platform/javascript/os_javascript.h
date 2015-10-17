/*************************************************************************/
/*  os_javascript.h                                                      */
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
#ifndef OS_JAVASCRIPT_H
#define OS_JAVASCRIPT_H

#include "os/input.h"
#include "drivers/unix/os_unix.h"
#include "os/main_loop.h"
#include "servers/physics/physics_server_sw.h"
#include "servers/spatial_sound/spatial_sound_server_sw.h"
#include "servers/spatial_sound_2d/spatial_sound_2d_server_sw.h"
#include "servers/audio/audio_server_sw.h"
#include "servers/physics_2d/physics_2d_server_sw.h"
#include "servers/visual/rasterizer.h"
#include "audio_server_javascript.h"
#include "audio_driver_javascript.h"
#include "main/input_default.h"

typedef void (*GFXInitFunc)(void *ud,bool gl2,int w, int h, bool fs);
typedef int (*OpenURIFunc)(const String&);
typedef String (*GetDataDirFunc)();
typedef String (*GetLocaleFunc)();

class OS_JavaScript : public OS_Unix {
public:

	struct TouchPos {
		int id;
		Point2 pos;
	};

private:

	Vector<TouchPos> touch;

	Point2 last_mouse;
	unsigned int last_id;
	GFXInitFunc gfx_init_func;
	void*gfx_init_ud;

	bool use_gl2;

	int64_t time_to_save_sync;
	int64_t last_sync_time;

	Rasterizer *rasterizer;
	VisualServer *visual_server;
	AudioServerJavascript *audio_server;
	//SampleManagerMallocSW *sample_manager;
	SpatialSoundServerSW *spatial_sound_server;
	SpatialSound2DServerSW *spatial_sound_2d_server;
	PhysicsServer *physics_server;
	Physics2DServer *physics_2d_server;
	AudioDriverJavaScript audio_driver_javascript;
	const char* gl_extensions;

	InputDefault *input;
	VideoMode default_videomode;
	MainLoop * main_loop;

	OpenURIFunc open_uri_func;
	GetDataDirFunc get_data_dir_func;
	GetLocaleFunc get_locale_func;

	static void _close_notification_funcs(const String& p_file,int p_flags);

public:

	// functions used by main to initialize/deintialize the OS
	virtual int get_video_driver_count() const;
	virtual const char * get_video_driver_name(int p_driver) const;

	virtual VideoMode get_default_video_mode() const;

	virtual int get_audio_driver_count() const;
	virtual const char * get_audio_driver_name(int p_driver) const;

	virtual void initialize_core();
	virtual void initialize(const VideoMode& p_desired,int p_video_driver,int p_audio_driver);

	virtual void set_main_loop( MainLoop * p_main_loop );
	virtual void delete_main_loop();

	virtual void finalize();


	typedef int64_t ProcessID;

	//static OS* get_singleton();

	virtual void vprint(const char* p_format, va_list p_list, bool p_stderr=false);
	virtual void print(const char *p_format, ... );
	virtual void alert(const String& p_alert);


	virtual void set_mouse_show(bool p_show);
	virtual void set_mouse_grab(bool p_grab);
	virtual bool is_mouse_grab_enabled() const;
	virtual Point2 get_mouse_pos() const;
	virtual int get_mouse_button_state() const;
	virtual void set_window_title(const String& p_title);

	//virtual void set_clipboard(const String& p_text);
	//virtual String get_clipboard() const;

	virtual void set_video_mode(const VideoMode& p_video_mode,int p_screen=0);
	virtual VideoMode get_video_mode(int p_screen=0) const;
	virtual void get_fullscreen_mode_list(List<VideoMode> *p_list,int p_screen=0) const;

	virtual Size2 get_window_size() const;
	virtual String get_name();
	virtual MainLoop *get_main_loop() const;

	virtual bool can_draw() const;

	virtual void set_cursor_shape(CursorShape p_shape);

	void main_loop_begin();
	bool main_loop_iterate();
	void main_loop_request_quit();
	void main_loop_end();
	void main_loop_focusout();
	void main_loop_focusin();

	virtual bool has_touchscreen_ui_hint() const;

	void set_opengl_extensions(const char* p_gl_extensions);
	void set_display_size(Size2 p_size);

	void reload_gfx();

	virtual Error shell_open(String p_uri);
	virtual String get_data_dir() const;
	virtual String get_resource_dir() const;
	virtual String get_locale() const;

	void process_accelerometer(const Vector3& p_accelerometer);
	void process_touch(int p_what,int p_pointer, const Vector<TouchPos>& p_points);
	void push_input(const InputEvent& p_ev);
	OS_JavaScript(GFXInitFunc p_gfx_init_func,void*p_gfx_init_ud, OpenURIFunc p_open_uri_func, GetDataDirFunc p_get_data_dir_func,GetLocaleFunc p_get_locale_func);
	~OS_JavaScript();

};

#endif
