/*************************************************************************/
/*  quick_open.cpp                                                       */
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
#include "quick_open.h"
#include "os/keyboard.h"


void EditorQuickOpen::popup(const StringName &p_base, bool p_dontclear, bool p_add_dirs) {

	add_directories=p_add_dirs;
	popup_centered_ratio(0.6);
	if (p_dontclear)
		search_box->select_all();
	else
		search_box->clear();
	search_box->grab_focus();
	base_type=p_base;
	_update_search();


}


void EditorQuickOpen::_text_changed(const String& p_newtext) {

	_update_search();
}

void EditorQuickOpen::_sbox_input(const InputEvent& p_ie) {

	if (p_ie.type==InputEvent::KEY && (
		p_ie.key.scancode == KEY_UP ||
		p_ie.key.scancode == KEY_DOWN ||
		p_ie.key.scancode == KEY_PAGEUP ||
		p_ie.key.scancode == KEY_PAGEDOWN ) ) {

		search_options->call("_input_event",p_ie);
		search_box->accept_event();
	}

}

void EditorQuickOpen::_parse_fs(EditorFileSystemDirectory *efsd) {

	if (!add_directories) {
		for(int i=0;i<efsd->get_subdir_count();i++) {

			_parse_fs(efsd->get_subdir(i));
		}
	}

	TreeItem *root = search_options->get_root();

	if (add_directories) {
		String path = efsd->get_path();
		if (!path.ends_with("/"))
			path+="/";
		if (path!="res://") {
			path=path.substr(6,path.length());
			if (path.findn(search_box->get_text())!=-1) {
				TreeItem *ti = search_options->create_item(root);
				ti->set_text(0,path);
				Ref<Texture> icon = get_icon("folder","FileDialog");
				ti->set_icon(0,icon);
			}
		}
	}
	for(int i=0;i<efsd->get_file_count();i++) {

		String file = efsd->get_file_path(i);
		file=file.substr(6,file.length());
		if (ObjectTypeDB::is_type(efsd->get_file_type(i),base_type) && (search_box->get_text()=="" || file.findn(search_box->get_text())!=-1)) {

			TreeItem *ti = search_options->create_item(root);
			ti->set_text(0,file);
			Ref<Texture> icon = get_icon( (has_icon(efsd->get_file_type(i),ei)?efsd->get_file_type(i):ot),ei);
			ti->set_icon(0,icon);
			if (root->get_children()==ti)
				ti->select(0);

		}
	}


	if (add_directories) {
		for(int i=0;i<efsd->get_subdir_count();i++) {

			_parse_fs(efsd->get_subdir(i));
		}
	}

}

void EditorQuickOpen::_update_search() {


	search_options->clear();
	TreeItem *root = search_options->create_item();
	_parse_fs(EditorFileSystem::get_singleton()->get_filesystem());

	get_ok()->set_disabled(root->get_children()==NULL);

}

void EditorQuickOpen::_confirmed() {

	TreeItem *ti = search_options->get_selected();
	if (!ti)
		return;
	emit_signal("quick_open","res://"+ti->get_text(0));
	hide();
}

void EditorQuickOpen::_notification(int p_what) {

	if (p_what==NOTIFICATION_ENTER_TREE) {

		connect("confirmed",this,"_confirmed");
	}
}


StringName EditorQuickOpen::get_base_type() const {

	return base_type;
}

void EditorQuickOpen::_bind_methods() {

	ObjectTypeDB::bind_method(_MD("_text_changed"),&EditorQuickOpen::_text_changed);
	ObjectTypeDB::bind_method(_MD("_confirmed"),&EditorQuickOpen::_confirmed);
	ObjectTypeDB::bind_method(_MD("_sbox_input"),&EditorQuickOpen::_sbox_input);

	ADD_SIGNAL(MethodInfo("quick_open",PropertyInfo(Variant::STRING,"respath")));

}


EditorQuickOpen::EditorQuickOpen() {


	VBoxContainer *vbc = memnew( VBoxContainer );
	add_child(vbc);
	set_child_rect(vbc);
	search_box = memnew( LineEdit );
	vbc->add_margin_child("Search:",search_box);
	search_box->connect("text_changed",this,"_text_changed");
	search_box->connect("input_event",this,"_sbox_input");
	search_options = memnew( Tree );
	vbc->add_margin_child("Matches:",search_options,true);
	get_ok()->set_text("Open");
	get_ok()->set_disabled(true);
	register_text_enter(search_box);
	set_hide_on_ok(false);
	search_options->connect("item_activated",this,"_confirmed");
	search_options->set_hide_root(true);
	ei="EditorIcons";
	ot="Object";
	add_directories=false;
}
