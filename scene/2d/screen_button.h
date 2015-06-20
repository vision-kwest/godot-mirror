/*************************************************************************/
/*  screen_button.h                                                      */
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
#ifndef SCREEN_BUTTON_H
#define SCREEN_BUTTON_H

#include "scene/2d/node_2d.h"
#include "scene/resources/texture.h"
#include "scene/resources/bit_mask.h"

class TouchScreenButton : public Node2D {

	OBJ_TYPE(TouchScreenButton,Node2D);
public:

	enum VisibilityMode {
		VISIBILITY_ALWAYS,
		VISIBILITY_TOUCHSCREEN_ONLY
	};

private:
	Ref<Texture> texture;
	Ref<Texture> texture_pressed;
	Ref<BitMap> bitmask;

	StringName action;
	bool passby_press;
	int finger_pressed;
	int action_id;

	VisibilityMode visibility;

	void _input(const InputEvent& p_Event);

protected:

	void _notification(int p_what);
	static void _bind_methods();
public:


	void set_texture(const Ref<Texture>& p_texture);
	Ref<Texture> get_texture() const;

	void set_texture_pressed(const Ref<Texture>& p_texture_pressed);
	Ref<Texture> get_texture_pressed() const;

	void set_bitmask(const Ref<BitMap>& p_bitmask);
	Ref<BitMap> get_bitmask() const;

	void set_action(const String& p_action);
	String get_action() const;

	void set_passby_press(bool p_enable);
	bool is_passby_press_enabled() const;

	void set_visibility_mode(VisibilityMode p_mode);
	VisibilityMode get_visibility_mode() const;

	bool is_pressed() const;

	Rect2 get_item_rect() const;


	TouchScreenButton();
};

VARIANT_ENUM_CAST(TouchScreenButton::VisibilityMode);

#endif // SCREEN_BUTTON_H
