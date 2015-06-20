/*************************************************************************/
/*  video_stream.h                                                       */
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
#ifndef VIDEO_STREAM_H
#define VIDEO_STREAM_H

#include "audio_stream_resampled.h"
#include "scene/resources/texture.h"


class VideoStream : public Resource {

	OBJ_TYPE(VideoStream,Resource);

protected:
	static void _bind_methods();

public:

	virtual void stop()=0;
	virtual void play()=0;

	virtual bool is_playing() const=0;

	virtual void set_paused(bool p_paused)=0;
	virtual bool is_paused(bool p_paused) const=0;

	virtual void set_loop(bool p_enable)=0;
	virtual bool has_loop() const=0;

	virtual float get_length() const=0;

	virtual float get_pos() const=0;
	virtual void seek_pos(float p_time)=0;

	virtual int get_pending_frame_count() const=0;
	virtual void pop_frame(Ref<ImageTexture> p_tex)=0;
	virtual Image peek_frame() const=0;

	virtual void set_audio_track(int p_idx) =0;

	virtual void update(float p_time)=0;

	VideoStream();
};

#endif

