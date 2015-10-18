#ifndef OS_HAIKU_H
#define OS_HAIKU_H

#include "drivers/unix/os_unix.h"
#include "servers/visual_server.h"
#include "servers/visual/rasterizer.h"
#include "servers/physics_server.h"
#include "servers/physics_2d/physics_2d_server_sw.h"
#include "servers/audio/audio_server_sw.h"
#include "servers/audio/sample_manager_sw.h"
#include "servers/spatial_sound/spatial_sound_server_sw.h"
#include "servers/spatial_sound_2d/spatial_sound_2d_server_sw.h"
#include "main/input_default.h"

#include "audio_driver_media_kit.h"
#include "context_gl_haiku.h"
#include "haiku_application.h"
#include "haiku_direct_window.h"


class OS_Haiku : public OS_Unix {
private:
	HaikuApplication* app;
	HaikuDirectWindow* window;
	MainLoop* main_loop;
	InputDefault* input;
	Rasterizer* rasterizer;
	VisualServer* visual_server;
	VideoMode current_video_mode;
	PhysicsServer* physics_server;
	Physics2DServer* physics_2d_server;
	AudioServerSW* audio_server;
	SampleManagerMallocSW* sample_manager;
	SpatialSoundServerSW* spatial_sound_server;
	SpatialSound2DServerSW* spatial_sound_2d_server;

#ifdef MEDIA_KIT_ENABLED
	AudioDriverMediaKit driver_media_kit;
#endif

#if defined(OPENGL_ENABLED) || defined(LEGACYGL_ENABLED)
	ContextGL_Haiku* context_gl;
#endif

	virtual void delete_main_loop();

protected:
	virtual int get_video_driver_count() const;
	virtual const char* get_video_driver_name(int p_driver) const;
	virtual VideoMode get_default_video_mode() const;

	virtual void initialize(const VideoMode& p_desired, int p_video_driver, int p_audio_driver);
	virtual void finalize();

	virtual void set_main_loop(MainLoop* p_main_loop);

public:
	OS_Haiku();
	void run();

	virtual String get_name();

	virtual MainLoop* get_main_loop() const;

	virtual bool can_draw() const;
	virtual void release_rendering_thread();
	virtual void make_rendering_thread();
	virtual void swap_buffers();

	virtual Point2 get_mouse_pos() const;
	virtual int get_mouse_button_state() const;
	virtual void set_cursor_shape(CursorShape p_shape);

	virtual int get_screen_count() const;
	virtual int get_current_screen() const;
	virtual void set_current_screen(int p_screen);
	virtual Point2 get_screen_position(int p_screen=0) const;
	virtual Size2 get_screen_size(int p_screen=0) const;
	virtual void set_window_title(const String& p_title);
	virtual Size2 get_window_size() const;
	virtual void set_window_size(const Size2 p_size);
	virtual Point2 get_window_position() const;
	virtual void set_window_position(const Point2& p_position);
	virtual void set_window_fullscreen(bool p_enabled);
	virtual bool is_window_fullscreen() const;
	virtual void set_window_resizable(bool p_enabled);
	virtual bool is_window_resizable() const;
	virtual void set_window_minimized(bool p_enabled);
	virtual bool is_window_minimized() const;
	virtual void set_window_maximized(bool p_enabled);
	virtual bool is_window_maximized() const;

	virtual void set_video_mode(const VideoMode& p_video_mode, int p_screen=0);
	virtual VideoMode get_video_mode(int p_screen=0) const;
	virtual void get_fullscreen_mode_list(List<VideoMode> *p_list, int p_screen=0) const;
	virtual String get_executable_path() const;
};

#endif
