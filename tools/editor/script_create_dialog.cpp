/*************************************************************************/
/*  script_create_dialog.cpp                                             */
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
#include "script_create_dialog.h"
#include "script_language.h"
#include "globals.h"
#include "io/resource_saver.h"
#include "os/file_access.h"
#include "editor_file_system.h"

void ScriptCreateDialog::config(const String& p_base_name,const String&p_base_path) {

	class_name->set_text("");
	parent_name->set_text(p_base_name);
	if (p_base_path!="")  {
		initial_bp=p_base_path.basename();
		file_path->set_text(initial_bp+"."+ScriptServer::get_language( language_menu->get_selected() )->get_extension());
	} else {
		initial_bp="";
		file_path->set_text("");
	}
	_class_name_changed("");
	_path_changed(file_path->get_text());
}

bool ScriptCreateDialog::_validate(const String& p_string) {

	if (p_string.length()==0)
		return false;


	for(int i=0;i<p_string.length();i++) {

		if (i==0) {
			if (p_string[0]>='0' && p_string[0]<='9')
				return false; // no start with number plz
		}

		bool valid_char = (p_string[i]>='0' && p_string[i]<='9') || (p_string[i]>='a' && p_string[i]<='z') || (p_string[i]>='A' && p_string[i]<='Z') || p_string[i]=='_';

		if (!valid_char)
			return false;

	}

	return true;
}

void ScriptCreateDialog::_class_name_changed(const String& p_name) {

	if (!_validate(parent_name->get_text())) {
		error_label->set_text("Invaild parent class name");
		error_label->add_color_override("font_color",Color(1,0.4,0.0,0.8));
	} else if (class_name->is_editable()) {
		if (class_name->get_text()=="") {
		error_label->set_text("Valid Chars: a-z A-Z 0-9 _");
		error_label->add_color_override("font_color",Color(1,1,1,0.6));
		} else if (!_validate(class_name->get_text())) {
			error_label->set_text("Invalid class name");
			error_label->add_color_override("font_color",Color(1,0.2,0.2,0.8));
		} else {
			error_label->set_text("Valid Name");
			error_label->add_color_override("font_color",Color(0,1.0,0.8,0.8));
		}
	} else {

		error_label->set_text("N/A");
		error_label->add_color_override("font_color",Color(0,1.0,0.8,0.8));
	}
}

void ScriptCreateDialog::ok_pressed() {

	if (class_name->is_editable() && !_validate(class_name->get_text())) {

		alert->set_text("Class Name is Invalid!");
		alert->popup_centered_minsize();
		return;
	}
	if (!_validate(parent_name->get_text())) {
		alert->set_text("Parent Class Name is Invalid!");
		alert->popup_centered_minsize();

		return;

	}


	String cname;
	if (class_name->is_editable())
		cname=class_name->get_text();



	String text = ScriptServer::get_language( language_menu->get_selected() )->get_template(cname,parent_name->get_text());
	Script *script = ScriptServer::get_language( language_menu->get_selected() )->create_script();
	script->set_source_code(text);
	if (cname!="")
		script->set_name(cname);


	Ref<Script> scr(script);

	if (!internal->is_pressed()) {


		String lpath = Globals::get_singleton()->localize_path(file_path->get_text());
		script->set_path(lpath);
		if (!path_valid) {

			alert->set_text("Path is Invalid!");
			alert->popup_centered_minsize();
			return;

		}
		Error err = ResourceSaver::save(lpath,scr,ResourceSaver::FLAG_CHANGE_PATH);		
		if (err!=OK) {

			alert->set_text("Could not create script in filesystem: "+String(""));
			alert->popup_centered_minsize();
			return;
		}
		scr->set_path(lpath);
	//EditorFileSystem::get_singleton()->update_file(lpath,scr->get_type());


	}

	hide();
	emit_signal("script_created",scr);

}

void ScriptCreateDialog::_lang_changed(int l) {

	l=language_menu->get_selected();
	if (ScriptServer::get_language( l )->has_named_classes()) {
		class_name->set_editable(true);
	} else {
		class_name->set_editable(false);
	}
	if (file_path->get_text().basename()==initial_bp) {
		file_path->set_text(initial_bp+"."+ScriptServer::get_language( l )->get_extension());
		_path_changed(file_path->get_text());
	}
	_class_name_changed(class_name->get_text());

}

void ScriptCreateDialog::_built_in_pressed() {

	if (internal->is_pressed()) {
		path_vb->hide();
	} else {
		path_vb->show();
	}

}

void ScriptCreateDialog::_browse_path() {

	file_browse->set_mode(EditorFileDialog::MODE_SAVE_FILE);
	file_browse->clear_filters();
	List<String> extensions;

	int l=language_menu->get_selected();
	ScriptServer::get_language( l )->get_recognized_extensions(&extensions);

	for(List<String>::Element *E=extensions.front();E;E=E->next()) {
		file_browse->add_filter("*."+E->get());
	}

	file_browse->set_current_path(file_path->get_text());	
	file_browse->popup_centered_ratio();

}

void ScriptCreateDialog::_file_selected(const String& p_file) {

	String p = Globals::get_singleton()->localize_path(p_file);
	file_path->set_text(p);
	_path_changed(p);

}

void ScriptCreateDialog::_path_changed(const String& p_path) {

	path_valid=false;
	String p =p_path;

	if (p=="")	 {

		path_error_label->set_text("Path is Empty");
		path_error_label->add_color_override("font_color",Color(1,0.4,0.0,0.8));
		return;

	}

	p = Globals::get_singleton()->localize_path(p);
	if (!p.begins_with("res://")) {

		path_error_label->set_text("Path is not local");
		path_error_label->add_color_override("font_color",Color(1,0.4,0.0,0.8));
		return;
	}

	if (p.find("/") || p.find("\\")) {
		DirAccess *d = DirAccess::create(DirAccess::ACCESS_RESOURCES);

		if (d->change_dir(p.get_base_dir())!=OK) {

			path_error_label->set_text("Base Path Invalid");
			path_error_label->add_color_override("font_color",Color(1,0.4,0.0,0.8));
			memdelete(d);
			return;

		}
		memdelete(d);
	}



	FileAccess *f = FileAccess::create(FileAccess::ACCESS_RESOURCES);

	if (f->file_exists(p)) {

		path_error_label->set_text("File Exists");
		path_error_label->add_color_override("font_color",Color(1,0.4,0.0,0.8));
		memdelete(f);
		return;
	}

	memdelete(f);

	String extension=p.extension();
	List<String> extensions;

	int l=language_menu->get_selected();
	ScriptServer::get_language( l )->get_recognized_extensions(&extensions);

	bool found=false;
	for(List<String>::Element *E=extensions.front();E;E=E->next()) {
		if (E->get().nocasecmp_to(extension)==0) {
			found=true;
			break;
		}
	}

	if (!found) {

		path_error_label->set_text("Invalid Extension");
		path_error_label->add_color_override("font_color",Color(1,0.4,0.0,0.8));
		return;
	}


	path_error_label->set_text("Path is Valid");
	path_error_label->add_color_override("font_color",Color(0,1.0,0.8,0.8));

	path_valid=true;

}


void ScriptCreateDialog::_bind_methods() {

	ObjectTypeDB::bind_method("_class_name_changed",&ScriptCreateDialog::_class_name_changed);
	ObjectTypeDB::bind_method("_lang_changed",&ScriptCreateDialog::_lang_changed);
	ObjectTypeDB::bind_method("_built_in_pressed",&ScriptCreateDialog::_built_in_pressed);
	ObjectTypeDB::bind_method("_browse_path",&ScriptCreateDialog::_browse_path);
	ObjectTypeDB::bind_method("_file_selected",&ScriptCreateDialog::_file_selected);
	ObjectTypeDB::bind_method("_path_changed",&ScriptCreateDialog::_path_changed);
	ADD_SIGNAL(MethodInfo("script_created",PropertyInfo(Variant::OBJECT,"script",PROPERTY_HINT_RESOURCE_TYPE,"Script")));
}

ScriptCreateDialog::ScriptCreateDialog() {

	/* SNAP DIALOG */

	VBoxContainer *vb = memnew( VBoxContainer );
	add_child(vb);
	set_child_rect(vb);


	class_name = memnew( LineEdit );
	VBoxContainer *vb2 = memnew( VBoxContainer );
	vb2->add_child(class_name);
	class_name->connect("text_changed", this,"_class_name_changed");
	error_label = memnew(Label);
	error_label->set_text("valid chars: a-z A-Z 0-9 _");
	error_label->set_align(Label::ALIGN_CENTER);
	vb2->add_child(error_label);
	vb->add_margin_child("Class Name:",vb2);

	parent_name = memnew( LineEdit );
	vb->add_margin_child("Inherits:",parent_name);
	parent_name->connect("text_changed", this,"_class_name_changed");

	language_menu = memnew( OptionButton );
	vb->add_margin_child("Language",language_menu);

	for(int i=0;i<ScriptServer::get_language_count();i++) {

		language_menu->add_item(ScriptServer::get_language(i)->get_name());
	}

	language_menu->select(0);
	language_menu->connect("item_selected",this,"_lang_changed");

	//parent_name->set_text();

	vb2 = memnew( VBoxContainer );
	path_vb = memnew( VBoxContainer );
	vb2->add_child(path_vb);

	HBoxContainer *hbc = memnew( HBoxContainer );
	file_path = memnew( LineEdit );
	file_path->connect("text_changed",this,"_path_changed");
	hbc->add_child(file_path);
	file_path->set_h_size_flags(SIZE_EXPAND_FILL);
	Button *b = memnew( Button );
	b->set_text(" .. ");
	b->connect("pressed",this,"_browse_path");
	hbc->add_child(b);
	path_vb->add_child(hbc);
	path_error_label = memnew( Label );
	path_vb->add_child( path_error_label );
	path_error_label->set_text("Error!");
	path_error_label->set_align(Label::ALIGN_CENTER);


	internal = memnew( CheckButton );
	internal->set_text("Built-In Script");
	vb2->add_child(internal);
	internal->connect("pressed",this,"_built_in_pressed");

	vb->add_margin_child("Path:",vb2);

	set_size(Size2(200,150));
	set_hide_on_ok(false);
	set_title("Create Script for Node");

	file_browse = memnew( EditorFileDialog );
	file_browse->connect("file_selected",this,"_file_selected");
	add_child(file_browse);
	get_ok()->set_text("Create");
	alert = memnew( AcceptDialog );
	add_child(alert);
	_lang_changed(0);
}
