/*************************************************************************/
/*  input.cpp                                                            */
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
#include "input.h"
#include "input_map.h"
#include "os/os.h"
#include "globals.h"
Input *Input::singleton=NULL;

Input *Input::get_singleton() {

	return singleton;
}

void Input::set_mouse_mode(MouseMode p_mode) {
	ERR_FAIL_INDEX(p_mode,3);
	OS::get_singleton()->set_mouse_mode((OS::MouseMode)p_mode);
}

Input::MouseMode Input::get_mouse_mode() const {

	return (MouseMode)OS::get_singleton()->get_mouse_mode();
}

void Input::_bind_methods() {

	ObjectTypeDB::bind_method(_MD("is_key_pressed","scancode"),&Input::is_key_pressed);
	ObjectTypeDB::bind_method(_MD("is_mouse_button_pressed","button"),&Input::is_mouse_button_pressed);
	ObjectTypeDB::bind_method(_MD("is_joy_button_pressed","device","button"),&Input::is_joy_button_pressed);
	ObjectTypeDB::bind_method(_MD("is_action_pressed","action"),&Input::is_action_pressed);
	ObjectTypeDB::bind_method(_MD("get_joy_axis","device","axis"),&Input::get_joy_axis);
	ObjectTypeDB::bind_method(_MD("get_joy_name","device"),&Input::get_joy_name);
	ObjectTypeDB::bind_method(_MD("get_accelerometer"),&Input::get_accelerometer);
	//ObjectTypeDB::bind_method(_MD("get_mouse_pos"),&Input::get_mouse_pos); - this is not the function you want
	ObjectTypeDB::bind_method(_MD("get_mouse_speed"),&Input::get_mouse_speed);
	ObjectTypeDB::bind_method(_MD("get_mouse_button_mask"),&Input::get_mouse_button_mask);
	ObjectTypeDB::bind_method(_MD("set_mouse_mode","mode"),&Input::set_mouse_mode);
	ObjectTypeDB::bind_method(_MD("get_mouse_mode"),&Input::get_mouse_mode);
	ObjectTypeDB::bind_method(_MD("warp_mouse_pos","to"),&Input::warp_mouse_pos);
	ObjectTypeDB::bind_method(_MD("action_press"),&Input::action_press);
	ObjectTypeDB::bind_method(_MD("action_release"),&Input::action_release);
	ObjectTypeDB::bind_method(_MD("set_custom_mouse_cursor","image:Texture","hotspot"),&Input::set_custom_mouse_cursor,DEFVAL(Vector2()));

	BIND_CONSTANT( MOUSE_MODE_VISIBLE );
	BIND_CONSTANT( MOUSE_MODE_HIDDEN );
	BIND_CONSTANT( MOUSE_MODE_CAPTURED );

	ADD_SIGNAL( MethodInfo("joy_connection_changed", PropertyInfo(Variant::INT, "index"), PropertyInfo(Variant::BOOL, "connected")) );
}

void Input::get_argument_options(const StringName& p_function,int p_idx,List<String>*r_options) const {
#ifdef TOOLS_ENABLED

	String pf=p_function;
	if (p_idx==0 && (pf=="is_action_pressed" || pf=="action_press" || pf=="action_release")) {

		List<PropertyInfo> pinfo;
		Globals::get_singleton()->get_property_list(&pinfo);

		for(List<PropertyInfo>::Element *E=pinfo.front();E;E=E->next()) {
			const PropertyInfo &pi=E->get();

			if (!pi.name.begins_with("input/"))
				continue;

			String name = pi.name.substr(pi.name.find("/")+1,pi.name.length());
			r_options->push_back("\""+name+"\"");

		}
	}
#endif

}

Input::Input() {

	singleton=this;
}


//////////////////////////////////////////////////////////

