/*************************************************************************/
/*  margin_container.cpp                                                 */
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
#include "margin_container.h"


Size2 MarginContainer::get_minimum_size() const {

	int margin = get_constant("margin");

	Size2 max;

	for(int i=0;i<get_child_count();i++) {

		Control *c = get_child(i)->cast_to<Control>();
		if (!c)
			continue;
		if (c->is_set_as_toplevel())
			continue;
		if (c->is_hidden())
			continue;

		Size2 s = c->get_combined_minimum_size();
		if (s.width > max.width)
			max.width = s.width;
		if (s.height > max.height)
			max.height = s.height;
	}

	max.width += margin;
	return max;


}

void MarginContainer::_notification(int p_what) {

	if (p_what==NOTIFICATION_SORT_CHILDREN) {

		int margin = get_constant("margin");

		Size2 s = get_size();

		for(int i=0;i<get_child_count();i++) {

			Control *c = get_child(i)->cast_to<Control>();
			if (!c)
				continue;
			if (c->is_set_as_toplevel())
				continue;
			int w;
			if (c->get_h_size_flags() & Control::SIZE_EXPAND)
				w=s.width-margin;
			else
				w=c->get_combined_minimum_size().width;

			fit_child_in_rect(c,Rect2(margin,0,s.width-margin,s.height));
		}

	}
}

MarginContainer::MarginContainer()
{
}
