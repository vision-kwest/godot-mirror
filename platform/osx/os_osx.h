/*************************************************************************/
/*  os_osx.h                                                             */
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
#ifndef OS_OSX_H
#define OS_OSX_H


#include "os/input.h"
#include "drivers/unix/os_unix.h"
#include "main/input_default.h"
#include "servers/visual_server.h"
#include "servers/visual/visual_server_wrap_mt.h"
#include "servers/visual/rasterizer.h"
#include "servers/physics_server.h"
#include "servers/audio/audio_server_sw.h"
#include "servers/audio/sample_manager_sw.h"
#include "servers/spatial_sound/spatial_sound_server_sw.h"
#include "servers/spatial_sound_2d/spatial_sound_2d_server_sw.h"
#include "drivers/rtaudio/audio_driver_rtaudio.h"
#include "drivers/alsa/audio_driver_alsa.h"
#include "servers/physics_2d/physics_2d_server_sw.h"
#include "servers/physics_2d/physics_2d_server_wrap_mt.h"
#include "platform/osx/audio_driver_osx.h"
#include <ApplicationServices/ApplicationServices.h>

//bitch
#undef CursorShape
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class OS_OSX : public OS_Unix {
public:
	bool force_quit;
	Rasterizer *rasterizer;
	VisualServer *visual_server;
	VideoMode current_videomode;
	List<String> args;
	MainLoop *main_loop;
	unsigned int event_id;

	PhysicsServer *physics_server;
	Physics2DServer *physics_2d_server;

	IP_Unix *ip_unix;

	AudioDriverOSX audio_driver_osx;
	AudioServerSW *audio_server;
	SampleManagerMallocSW *sample_manager;
	SpatialSoundServerSW *spatial_sound_server;
	SpatialSound2DServerSW *spatial_sound_2d_server;

	InputDefault *input;

	/* objc */

	CGEventSourceRef eventSource;

	void process_events();

	void* framework;
//          pthread_key_t   current;
	bool mouse_grab;
	Point2 mouse_pos;
	uint32_t last_id;

	id delegate;
	id window_delegate;
	id window_object;
	id window_view;
	id autoreleasePool;
	id cursor;
	id pixelFormat;
	id context;

	CursorShape cursor_shape;
	MouseMode mouse_mode;

	bool minimized;
	bool maximized;
	bool zoomed;
	Vector<Rect2> screens;
	int current_screen;
	Rect2 restore_rect;
protected:

	virtual int get_video_driver_count() const;
	virtual const char * get_video_driver_name(int p_driver) const;
	virtual VideoMode get_default_video_mode() const;

	virtual void initialize_core();
	virtual void initialize(const VideoMode& p_desired,int p_video_driver,int p_audio_driver);
	virtual void finalize();

	virtual void set_main_loop( MainLoop * p_main_loop );
	virtual void delete_main_loop();

public:


	static OS_OSX* singleton;

	void wm_minimized(bool p_minimized);

	virtual String get_name();

	virtual void set_cursor_shape(CursorShape p_shape);

	virtual void set_mouse_show(bool p_show);
	virtual void set_mouse_grab(bool p_grab);
	virtual bool is_mouse_grab_enabled() const;
	virtual void warp_mouse_pos(const Point2& p_to);
	virtual Point2 get_mouse_pos() const;
	virtual int get_mouse_button_state() const;
	virtual void set_window_title(const String& p_title);
	
	virtual Size2 get_window_size() const;

	virtual void set_icon(const Image& p_icon);

	virtual MainLoop *get_main_loop() const;

	virtual bool can_draw() const;

	virtual void set_clipboard(const String& p_text);
	virtual String get_clipboard() const;

	virtual void release_rendering_thread();
	virtual void make_rendering_thread();
	virtual void swap_buffers();

	Error shell_open(String p_uri);
	void push_input(const InputEvent& p_event);

	String get_locale() const;

	virtual void set_video_mode(const VideoMode& p_video_mode,int p_screen=0);
	virtual VideoMode get_video_mode(int p_screen=0) const;
	virtual void get_fullscreen_mode_list(List<VideoMode> *p_list,int p_screen=0) const;

	virtual String get_executable_path() const;

	virtual LatinKeyboardVariant get_latin_keyboard_variant() const;

	virtual void move_window_to_foreground();

	virtual int get_screen_count() const;
	virtual int get_current_screen() const;
	virtual void set_current_screen(int p_screen);
	virtual Point2 get_screen_position(int p_screen=0) const;
	virtual Point2 get_window_position() const;
	virtual void set_window_position(const Point2& p_position);
	virtual void set_window_size(const Size2 p_size);
	virtual void set_window_fullscreen(bool p_enabled);
	virtual bool is_window_fullscreen() const;
	virtual void set_window_resizable(bool p_enabled);
	virtual bool is_window_resizable() const;
	virtual void set_window_minimized(bool p_enabled);
	virtual bool is_window_minimized() const;
	virtual void set_window_maximized(bool p_enabled);
	virtual bool is_window_maximized() const;
	Size2 get_screen_size(int p_screen);


	void run();

    void set_mouse_mode(MouseMode p_mode);
    MouseMode get_mouse_mode() const;

	OS_OSX();
};

#endif
