/*************************************************************************/
/*  audio_stream_ogg_vorbis.h                                            */
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
#ifndef AUDIO_STREAM_OGG_VORBIS_H
#define AUDIO_STREAM_OGG_VORBIS_H

#include "scene/resources/audio_stream_resampled.h"
#include "vorbis/vorbisfile.h"
#include "os/file_access.h"
#include "io/resource_loader.h"
#include "os/thread_safe.h"

class AudioStreamOGGVorbis : public AudioStreamResampled {

	OBJ_TYPE(AudioStreamOGGVorbis,AudioStreamResampled);
	_THREAD_SAFE_CLASS_


	enum {
		MIN_MIX=1024
	};

	FileAccess *f;

	ov_callbacks _ov_callbacks;
	float length;
	static size_t _ov_read_func(void *p_dst,size_t p_data, size_t p_count, void *_f);
	static int _ov_seek_func(void *_f,ogg_int64_t offs, int whence);
	static int _ov_close_func(void *_f);
	static long _ov_tell_func(void *_f);


	virtual bool _can_mix() const;

	String file;
	int64_t frames_mixed;

	bool stream_loaded;
	volatile bool playing;
	OggVorbis_File vf;
	int stream_channels;
	int stream_srate;
	int current_section;

	volatile bool setting_up;
	bool paused;
	bool loops;
	int repeats;

	Error _load_stream();
	void _clear_stream();
	void _close_file();


public:


	void set_file(const String& p_file);


	virtual void play();
	virtual void stop();
	virtual bool is_playing() const;

	virtual void set_paused(bool p_paused);
	virtual bool is_paused(bool p_paused) const;

	virtual void set_loop(bool p_enable);
	virtual bool has_loop() const;

	virtual float get_length() const;

	virtual String get_stream_name() const;

	virtual int get_loop_count() const;

	virtual float get_pos() const;
	virtual void seek_pos(float p_time);

	virtual UpdateMode get_update_mode() const;
	virtual void update();

	AudioStreamOGGVorbis();
	~AudioStreamOGGVorbis();
};

class ResourceFormatLoaderAudioStreamOGGVorbis : public ResourceFormatLoader {
public:
	virtual RES load(const String &p_path,const String& p_original_path="");
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String& p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};


#endif // AUDIO_STREAM_OGG_H
