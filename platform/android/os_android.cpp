/*************************************************************************/
/*  os_android.cpp                                                       */
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
#include "os_android.h"
#include "drivers/gles2/rasterizer_gles2.h"

#include "core/io/file_access_buffered_fa.h"
#include "drivers/unix/file_access_unix.h"
#include "drivers/unix/dir_access_unix.h"

#include "servers/visual/visual_server_raster.h"
#include "servers/visual/visual_server_wrap_mt.h"
#include "main/main.h"

#include "file_access_android.h"

#include "core/globals.h"

#ifdef ANDROID_NATIVE_ACTIVITY
#include "file_access_android.h"
#include "dir_access_android.h"
#else
#include "file_access_jandroid.h"
#include "dir_access_jandroid.h"
#endif

int OS_Android::get_video_driver_count() const {

	return 1;
}
const char * OS_Android::get_video_driver_name(int p_driver) const {

	return "GLES2";
}

OS::VideoMode OS_Android::get_default_video_mode() const {

	return OS::VideoMode();
}

int OS_Android::get_audio_driver_count() const {

	return 1;
}

const char * OS_Android::get_audio_driver_name(int p_driver) const {

	return "Android";
}

void OS_Android::initialize_core() {

	OS_Unix::initialize_core();

#ifdef ANDROID_NATIVE_ACTIVITY

	FileAccess::make_default<FileAccessAndroid>(FileAccess::ACCESS_RESOURCES);
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_USERDATA);
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_FILESYSTEM);
	//FileAccessBufferedFA<FileAccessUnix>::make_default();
	DirAccess::make_default<DirAccessAndroid>(DirAccess::ACCESS_RESOURCES);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_USERDATA);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_FILESYSTEM);

#else

	if (use_apk_expansion)
		FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_RESOURCES);
	else {
#ifdef USE_JAVA_FILE_ACCESS
		FileAccess::make_default<FileAccessBufferedFA<FileAccessJAndroid> >(FileAccess::ACCESS_RESOURCES);
#else
		//FileAccess::make_default<FileAccessBufferedFA<FileAccessAndroid> >(FileAccess::ACCESS_RESOURCES);
		FileAccess::make_default<FileAccessAndroid>(FileAccess::ACCESS_RESOURCES);
#endif
	}
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_USERDATA);
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_FILESYSTEM);
	//FileAccessBufferedFA<FileAccessUnix>::make_default();
	if (use_apk_expansion)
		DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_RESOURCES);
	else
		DirAccess::make_default<DirAccessJAndroid>(DirAccess::ACCESS_RESOURCES);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_USERDATA);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_FILESYSTEM);

#endif

}

void OS_Android::set_opengl_extensions(const char* p_gl_extensions) {

	ERR_FAIL_COND(!p_gl_extensions);
	gl_extensions=p_gl_extensions;
}

void OS_Android::initialize(const VideoMode& p_desired,int p_video_driver,int p_audio_driver) {


	use_gl2=p_video_driver!=1;


	if (gfx_init_func)
		gfx_init_func(gfx_init_ud,use_gl2);

	AudioDriverManagerSW::add_driver(&audio_driver_android);


	if (true) {
		RasterizerGLES2 *rasterizer_gles22=memnew( RasterizerGLES2(false,use_reload_hooks,false,use_reload_hooks ) );
		if (gl_extensions)
			rasterizer_gles22->set_extensions(gl_extensions);
		rasterizer = rasterizer_gles22;
	} else {
		//rasterizer = memnew( RasterizerGLES1(use_reload_hooks, use_reload_hooks) );

	}

	visual_server = memnew( VisualServerRaster(rasterizer) );
	if (get_render_thread_mode() != RENDER_THREAD_UNSAFE) {

		visual_server = memnew(VisualServerWrapMT(visual_server, false));
	};
	visual_server->init();
	visual_server->cursor_set_visible(false, 0);

	AudioDriverManagerSW::get_driver(p_audio_driver)->set_singleton();

	if (AudioDriverManagerSW::get_driver(p_audio_driver)->init()!=OK) {

		ERR_PRINT("Initializing audio failed.");
	}

	sample_manager = memnew( SampleManagerMallocSW );
	audio_server = memnew( AudioServerSW(sample_manager) );

	audio_server->set_mixer_params(AudioMixerSW::INTERPOLATION_LINEAR,true);
	audio_server->init();

	spatial_sound_server = memnew( SpatialSoundServerSW );
	spatial_sound_server->init();

	spatial_sound_2d_server = memnew( SpatialSound2DServerSW );
	spatial_sound_2d_server->init();

	//
	physics_server = memnew( PhysicsServerSW );
	physics_server->init();
	//physics_2d_server = memnew( Physics2DServerSW );
	physics_2d_server = Physics2DServerWrapMT::init_server<Physics2DServerSW>();
	physics_2d_server->init();

	input = memnew( InputDefault );

}

void OS_Android::set_main_loop( MainLoop * p_main_loop ) {

	main_loop=p_main_loop;
	input->set_main_loop(p_main_loop);
#if 0

	print_line("preGS");
	FileAccess *f=memnew( FileAccessAndroid );
	print("made f %p\n",f);
	Error err = f->open("AndroidManifest.xml",FileAccess::READ);
	if (err) {

		print("************NO FILE!!\n");
	} else {
		print("************YES FILE!!\n");
	}
	f->close();
	print_line("end");


	AAssetDir* aad = AAssetManager_openDir(FileAccessAndroid::asset_manager,".");

	if (aad) {

		print_line("DIR OPEN OK");

		const char *fn= AAssetDir_getNextFileName(aad);

		while(fn) {

			print_line("FNAME: "+String(fn));
			fn= AAssetDir_getNextFileName(aad);
		}

		AAssetDir_close(aad);
	} else {

		print_line("DIR NO OPEN");
	}

#endif

}

void OS_Android::delete_main_loop() {

	memdelete( main_loop );
}

void OS_Android::finalize() {

	memdelete(input);
}


void OS_Android::vprint(const char* p_format, va_list p_list, bool p_stderr) {

	__android_log_vprint(p_stderr?ANDROID_LOG_ERROR:ANDROID_LOG_INFO,"godot",p_format,p_list);
}

void OS_Android::print(const char *p_format, ... ) {

	va_list argp;
	va_start(argp, p_format);
	__android_log_vprint(ANDROID_LOG_INFO,"godot",p_format,argp);
	va_end(argp);

}

void OS_Android::alert(const String& p_alert) {

	print("ALERT: %s\n",p_alert.utf8().get_data());
}


void OS_Android::set_mouse_show(bool p_show) {

	//android has no mouse...
}

void OS_Android::set_mouse_grab(bool p_grab) {

	//it really has no mouse...!
}

bool OS_Android::is_mouse_grab_enabled() const {

	//*sigh* technology has evolved so much since i was a kid..
	return false;
}
Point2 OS_Android::get_mouse_pos() const {

	return Point2();
}
int OS_Android::get_mouse_button_state() const {

	return 0;
}
void OS_Android::set_window_title(const String& p_title) {


}

//interesting byt not yet
//void set_clipboard(const String& p_text);
//String get_clipboard() const;

void OS_Android::set_video_mode(const VideoMode& p_video_mode,int p_screen) {


}

OS::VideoMode OS_Android::get_video_mode(int p_screen) const {

	return default_videomode;
}
void OS_Android::get_fullscreen_mode_list(List<VideoMode> *p_list,int p_screen) const {

	p_list->push_back(default_videomode);
}

Size2 OS_Android::get_window_size() const {

	return Vector2(default_videomode.width,default_videomode.height);
}

String OS_Android::get_name() {

	return "Android";
}

MainLoop *OS_Android::get_main_loop() const {

	return main_loop;
}

bool OS_Android::can_draw() const {

	return true; //always?
}

void OS_Android::set_cursor_shape(CursorShape p_shape) {

	//android really really really has no mouse.. how amazing..
}

void OS_Android::main_loop_begin() {

	if (main_loop)
		main_loop->init();
}
bool OS_Android::main_loop_iterate() {

	if (!main_loop)
		return false;
	return Main::iteration();
}

void OS_Android::main_loop_end() {

	if (main_loop)
		main_loop->finish();

}

void OS_Android::main_loop_focusout() {

	if (main_loop)
		main_loop->notification(MainLoop::NOTIFICATION_WM_FOCUS_OUT);
	audio_driver_android.set_pause(true);

}

void OS_Android::main_loop_focusin(){

	if (main_loop)
		main_loop->notification(MainLoop::NOTIFICATION_WM_FOCUS_IN);
	audio_driver_android.set_pause(false);

}


void OS_Android::process_event(InputEvent p_event) {

	p_event.ID = last_id++;
	input->parse_input_event(p_event);
};


void OS_Android::process_touch(int p_what,int p_pointer, const Vector<TouchPos>& p_points) {

//	print_line("ev: "+itos(p_what)+" pnt: "+itos(p_pointer)+" pointc: "+itos(p_points.size()));

	switch(p_what) {
		case 0: { //gesture begin

			if (touch.size()) {
				//end all if exist
				InputEvent ev;
				ev.type=InputEvent::MOUSE_BUTTON;
				ev.ID=last_id++;
				ev.mouse_button.button_index=BUTTON_LEFT;
				ev.mouse_button.button_mask=BUTTON_MASK_LEFT;
				ev.mouse_button.pressed=false;
				ev.mouse_button.x=touch[0].pos.x;
				ev.mouse_button.y=touch[0].pos.y;
				ev.mouse_button.global_x=touch[0].pos.x;
				ev.mouse_button.global_y=touch[0].pos.y;
				input->parse_input_event(ev);


				for(int i=0;i<touch.size();i++) {

					InputEvent ev;
					ev.type=InputEvent::SCREEN_TOUCH;
					ev.ID=last_id++;
					ev.screen_touch.index=touch[i].id;
					ev.screen_touch.pressed=false;
					ev.screen_touch.x=touch[i].pos.x;
					ev.screen_touch.y=touch[i].pos.y;
					input->parse_input_event(ev);

				}
			}

			touch.resize(p_points.size());
			for(int i=0;i<p_points.size();i++) {
				touch[i].id=p_points[i].id;
				touch[i].pos=p_points[i].pos;
			}

			{
				//send mouse
				InputEvent ev;
				ev.type=InputEvent::MOUSE_BUTTON;
				ev.ID=last_id++;
				ev.mouse_button.button_index=BUTTON_LEFT;
				ev.mouse_button.button_mask=BUTTON_MASK_LEFT;
				ev.mouse_button.pressed=true;
				ev.mouse_button.x=touch[0].pos.x;
				ev.mouse_button.y=touch[0].pos.y;
				ev.mouse_button.global_x=touch[0].pos.x;
				ev.mouse_button.global_y=touch[0].pos.y;
				last_mouse=touch[0].pos;
				input->parse_input_event(ev);
			}


			//send touch
			for(int i=0;i<touch.size();i++) {

				InputEvent ev;
				ev.type=InputEvent::SCREEN_TOUCH;
				ev.ID=last_id++;
				ev.screen_touch.index=touch[i].id;
				ev.screen_touch.pressed=true;
				ev.screen_touch.x=touch[i].pos.x;
				ev.screen_touch.y=touch[i].pos.y;
				input->parse_input_event(ev);
			}

		} break;
		case 1: { //motion


			if (p_points.size()) {
				//send mouse, should look for point 0?
				InputEvent ev;
				ev.type=InputEvent::MOUSE_MOTION;
				ev.ID=last_id++;
				ev.mouse_motion.button_mask=BUTTON_MASK_LEFT;
				ev.mouse_motion.x=p_points[0].pos.x;
				ev.mouse_motion.y=p_points[0].pos.y;
				input->set_mouse_pos(Point2(ev.mouse_motion.x,ev.mouse_motion.y));
				ev.mouse_motion.speed_x=input->get_mouse_speed().x;
				ev.mouse_motion.speed_y=input->get_mouse_speed().y;
				ev.mouse_motion.relative_x=p_points[0].pos.x-last_mouse.x;
				ev.mouse_motion.relative_y=p_points[0].pos.y-last_mouse.y;
				last_mouse=p_points[0].pos;
				input->parse_input_event(ev);
			}

			ERR_FAIL_COND(touch.size()!=p_points.size());

			for(int i=0;i<touch.size();i++) {

				int idx=-1;
				for(int j=0;j<p_points.size();j++) {

					if (touch[i].id==p_points[j].id) {
						idx=j;
						break;
					}

				}

				ERR_CONTINUE(idx==-1);

				if (touch[i].pos==p_points[idx].pos)
					continue; //no move unncesearily

				InputEvent ev;
				ev.type=InputEvent::SCREEN_DRAG;
				ev.ID=last_id++;
				ev.screen_drag.index=touch[i].id;
				ev.screen_drag.x=p_points[idx].pos.x;
				ev.screen_drag.y=p_points[idx].pos.y;
				ev.screen_drag.relative_x=p_points[idx].pos.x - touch[i].pos.x;
				ev.screen_drag.relative_y=p_points[idx].pos.y - touch[i].pos.y;
				input->parse_input_event(ev);
				touch[i].pos=p_points[idx].pos;
			}


		} break;
		case 2: { //release



			if (touch.size()) {
				//end all if exist
				InputEvent ev;
				ev.type=InputEvent::MOUSE_BUTTON;
				ev.ID=last_id++;
				ev.mouse_button.button_index=BUTTON_LEFT;
				ev.mouse_button.button_mask=BUTTON_MASK_LEFT;
				ev.mouse_button.pressed=false;
				ev.mouse_button.x=touch[0].pos.x;
				ev.mouse_button.y=touch[0].pos.y;
				ev.mouse_button.global_x=touch[0].pos.x;
				ev.mouse_button.global_y=touch[0].pos.y;
				input->parse_input_event(ev);


				for(int i=0;i<touch.size();i++) {

					InputEvent ev;
					ev.type=InputEvent::SCREEN_TOUCH;
					ev.ID=last_id++;
					ev.screen_touch.index=touch[i].id;
					ev.screen_touch.pressed=false;
					ev.screen_touch.x=touch[i].pos.x;
					ev.screen_touch.y=touch[i].pos.y;
					input->parse_input_event(ev);

				}
				touch.clear();
			}

		} break;
		case 3: { // add tuchi





			ERR_FAIL_INDEX(p_pointer,p_points.size());

			TouchPos tp=p_points[p_pointer];
			touch.push_back(tp);

			InputEvent ev;
			ev.type=InputEvent::SCREEN_TOUCH;
			ev.ID=last_id++;
			ev.screen_touch.index=tp.id;
			ev.screen_touch.pressed=true;
			ev.screen_touch.x=tp.pos.x;
			ev.screen_touch.y=tp.pos.y;
			input->parse_input_event(ev);

		} break;
		case 4: {


			for(int i=0;i<touch.size();i++) {
				if (touch[i].id==p_pointer) {

					InputEvent ev;
					ev.type=InputEvent::SCREEN_TOUCH;
					ev.ID=last_id++;
					ev.screen_touch.index=touch[i].id;
					ev.screen_touch.pressed=false;
					ev.screen_touch.x=touch[i].pos.x;
					ev.screen_touch.y=touch[i].pos.y;
					input->parse_input_event(ev);
					touch.remove(i);
					i--;
				}
			}

		} break;

	}

}

void OS_Android::process_accelerometer(const Vector3& p_accelerometer) {

	input->set_accelerometer(p_accelerometer);
}

bool OS_Android::has_touchscreen_ui_hint() const {

	return true;
}

bool OS_Android::has_virtual_keyboard() const {

	return true;
};

void OS_Android::show_virtual_keyboard(const String& p_existing_text,const Rect2& p_screen_rect) {

	if (show_virtual_keyboard_func) {
		show_virtual_keyboard_func(p_existing_text);
	} else {

		ERR_PRINT("Virtual keyboard not available");
	};
};

void OS_Android::hide_virtual_keyboard() {

	if (hide_virtual_keyboard_func) {

		hide_virtual_keyboard_func();
	} else {

		ERR_PRINT("Virtual keyboard not available");
	};
};


void OS_Android::init_video_mode(int p_video_width,int p_video_height) {

	default_videomode.width=p_video_width;
	default_videomode.height=p_video_height;
	default_videomode.fullscreen=true;
	default_videomode.resizable=false;
}

void OS_Android::main_loop_request_quit() {

	if (main_loop)
		main_loop->notification(MainLoop::NOTIFICATION_WM_QUIT_REQUEST);
}

void OS_Android::set_display_size(Size2 p_size) {

	default_videomode.width=p_size.x;
	default_videomode.height=p_size.y;
}

void OS_Android::reload_gfx() {

	if (gfx_init_func)
		gfx_init_func(gfx_init_ud,use_gl2);
	if (rasterizer)
		rasterizer->reload_vram();
}

Error OS_Android::shell_open(String p_uri) {

	if (open_uri_func)
		return open_uri_func(p_uri)?ERR_CANT_OPEN:OK;
	return ERR_UNAVAILABLE;
};

String OS_Android::get_resource_dir() const {

	return "/"; //android has it's own filesystem for resources inside the APK
}

String OS_Android::get_locale() const {

	if (get_locale_func)
		return get_locale_func();
	return OS_Unix::get_locale();
}

String OS_Android::get_model_name() const {

    if (get_model_func)
        return get_model_func();
    return OS_Unix::get_model_name();
}


void OS_Android::set_need_reload_hooks(bool p_needs_them) {

	use_reload_hooks=p_needs_them;
}

String OS_Android::get_data_dir() const {

	if (get_data_dir_func)
		return get_data_dir_func();
	return ".";
	//return Globals::get_singleton()->get_singleton_object("GodotOS")->call("get_data_dir");
};


void OS_Android::set_screen_orientation(ScreenOrientation p_orientation) {


	if (set_screen_orientation_func)
		set_screen_orientation_func(p_orientation);
}


String OS_Android::get_unique_ID() const {

	if (get_unique_id_func)
		return get_unique_id_func();
	return OS::get_unique_ID();
}

Error OS_Android::native_video_play(String p_path, float p_volume) {
	if (video_play_func)
		video_play_func(p_path);
    return OK;
}

bool OS_Android::native_video_is_playing() {
	if (video_is_playing_func)
		return video_is_playing_func();
    return false;
}

void OS_Android::native_video_pause() {
	if (video_pause_func)
		video_pause_func();
}

String OS_Android::get_system_dir(SystemDir p_dir) const {

	if (get_system_dir_func)
		return get_system_dir_func(p_dir);
	return String(".");
}

void OS_Android::native_video_stop() {
	if (video_stop_func)
		video_stop_func();
}

OS_Android::OS_Android(GFXInitFunc p_gfx_init_func,void*p_gfx_init_ud, OpenURIFunc p_open_uri_func, GetDataDirFunc p_get_data_dir_func,GetLocaleFunc p_get_locale_func,GetModelFunc p_get_model_func, ShowVirtualKeyboardFunc p_show_vk, HideVirtualKeyboardFunc p_hide_vk,  SetScreenOrientationFunc p_screen_orient,GetUniqueIDFunc p_get_unique_id,GetSystemDirFunc p_get_sdir_func, VideoPlayFunc p_video_play_func, VideoIsPlayingFunc p_video_is_playing_func, VideoPauseFunc p_video_pause_func, VideoStopFunc p_video_stop_func,bool p_use_apk_expansion) {


	use_apk_expansion=p_use_apk_expansion;
	default_videomode.width=800;
	default_videomode.height=600;
	default_videomode.fullscreen=true;
	default_videomode.resizable=false;

	gfx_init_func=p_gfx_init_func;
	gfx_init_ud=p_gfx_init_ud;
	main_loop=NULL;
	last_id=1;
	gl_extensions=NULL;
	rasterizer=NULL;
	use_gl2=false;

	open_uri_func=p_open_uri_func;
	get_data_dir_func=p_get_data_dir_func;
	get_locale_func=p_get_locale_func;
	get_model_func=p_get_model_func;
	get_unique_id_func=p_get_unique_id;
	get_system_dir_func=p_get_sdir_func;
    
	video_play_func = p_video_play_func;
	video_is_playing_func = p_video_is_playing_func;
	video_pause_func = p_video_pause_func;
	video_stop_func = p_video_stop_func;

	show_virtual_keyboard_func = p_show_vk;
	hide_virtual_keyboard_func = p_hide_vk;

	set_screen_orientation_func=p_screen_orient;
	use_reload_hooks=false;

}

OS_Android::~OS_Android() {


}
