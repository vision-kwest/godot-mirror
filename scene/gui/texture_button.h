/*************************************************************************/
/*  texture_button.h                                                     */
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
#ifndef TEXTURE_BUTTON_H
#define TEXTURE_BUTTON_H

#include "scene/gui/base_button.h"
#include "scene/resources/bit_mask.h"
class TextureButton : public BaseButton {

	OBJ_TYPE( TextureButton, BaseButton );

	Ref<Texture> normal;
	Ref<Texture> pressed;
	Ref<Texture> hover;
	Ref<Texture> disabled;
	Ref<Texture> focused;
	Ref<BitMap> click_mask;
	Size2 scale;
	Color modulate;


protected:

	virtual bool has_point(const Point2& p_point) const;
	virtual Size2 get_minimum_size() const;
	void _notification(int p_what);
	static void _bind_methods();

public:

	void set_normal_texture(const Ref<Texture>& p_normal);
	void set_pressed_texture(const Ref<Texture>& p_pressed);
	void set_hover_texture(const Ref<Texture>& p_hover);
	void set_disabled_texture(const Ref<Texture>& p_disabled);
	void set_focused_texture(const Ref<Texture>& p_focused);
	void set_click_mask(const Ref<BitMap>& p_image);

	Ref<Texture> get_normal_texture() const;
	Ref<Texture> get_pressed_texture() const;
	Ref<Texture> get_hover_texture() const;
	Ref<Texture> get_disabled_texture() const;
	Ref<Texture> get_focused_texture() const;
	Ref<BitMap> get_click_mask() const;

	void set_scale(Size2 p_scale);
	Size2 get_scale() const;

	void set_modulate(const Color& p_modulate);
	Color get_modulate() const;

	TextureButton();
};

#endif // TEXTURE_BUTTON_H
