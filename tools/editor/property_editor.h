/*************************************************************************/
/*  property_editor.h                                                    */
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
#ifndef PROPERTY_EDITOR_H
#define PROPERTY_EDITOR_H

#include "scene/gui/tree.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "tools/editor/editor_file_dialog.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/color_picker.h"
#include "scene/gui/menu_button.h"
#include "scene/gui/texture_frame.h"
#include "scene/gui/text_edit.h"
#include "scene/gui/check_button.h"
#include "scene_tree_editor.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class CustomPropertyEditor : public Popup {

	OBJ_TYPE( CustomPropertyEditor, Popup );

	enum {
		MAX_VALUE_EDITORS=12,
		MAX_ACTION_BUTTONS=5,
		OBJ_MENU_LOAD=0,
		OBJ_MENU_EDIT=1,
		OBJ_MENU_CLEAR=2,
		OBJ_MENU_MAKE_UNIQUE=3,
		OBJ_MENU_COPY=4,
		OBJ_MENU_PASTE=5,
		OBJ_MENU_REIMPORT=6,
		TYPE_BASE_ID=100

	};

	enum {
		EASING_LINEAR,
		EASING_EASE_IN,
		EASING_EASE_OUT,
		EASING_ZERO,
		EASING_IN_OUT,
		EASING_OUT_IN
	};


	PopupMenu *menu;
	SceneTreeDialog *scene_tree;
	EditorFileDialog *file;
	ConfirmationDialog *error;
	String name;
	Variant::Type type;
	Variant v;
	int hint;
	String hint_text;
	LineEdit *value_editor[MAX_VALUE_EDITORS];
	Label *value_label[MAX_VALUE_EDITORS];
	HScrollBar *scroll[4];
	Button *action_buttons[MAX_ACTION_BUTTONS];
	MenuButton *type_button;
	Vector<String> inheritors_array;
	TextureFrame *texture_preview;
	ColorPicker *color_picker;
	TextEdit *text_edit;
	bool read_only;
	Button *checks20[20];
	SpinBox *spinbox;
	HSlider *slider;


	Control *easing_draw;

	Object* owner;

	bool updating;

	void _text_edit_changed();
	void _file_selected(String p_file);
	void _scroll_modified(double p_value);
	void _modified(String p_string);
	void _range_modified(double p_value);
	void _focus_enter();
	void _focus_exit();
	void _action_pressed(int p_which);
	void _type_create_selected(int p_idx);

	void _color_changed(const Color& p_color);
	void _draw_easing();
	void _menu_option(int p_which);

	void _drag_easing(const InputEvent& p_ev);

	void _node_path_selected(NodePath p_path);
	void show_value_editors(int p_amount);
	void config_value_editors(int p_amount, int p_columns,int p_label_w,const List<String>& p_strings);
	void config_action_buttons(const List<String>& p_strings);

protected:

	void _notification(int p_what);
	static void _bind_methods();

public:
	Variant get_variant() const;
	String get_name() const;

	void set_read_only(bool p_read_only) { read_only=p_read_only; }

	bool edit(Object* p_owner,const String& p_name,Variant::Type p_type, const Variant& p_variant,int p_hint,String p_hint_text);

	CustomPropertyEditor();
};

class PropertyEditor : public Control {

	OBJ_TYPE( PropertyEditor, Control );

	Tree *tree;
	Label *top_label;
	//Object *object;

	Object* obj;

	Array _prop_edited_name;
	StringName _prop_edited;

	bool capitalize_paths;
	bool changing;
	bool update_tree_pending;
	bool autoclear;
	bool keying;
	bool read_only;
	bool show_categories;
	float refresh_countdown;
	bool use_doc_hints;

	HashMap<String,String> pending;
	String selected_property;

	Map<StringName,Map<StringName,String> > descr_cache;
	Map<StringName,String > class_descr_cache;
	
	CustomPropertyEditor *custom_editor;

	void _resource_edit_request();
	void _custom_editor_edited();
	void _custom_editor_request(bool p_arrow);

	void _item_selected();
	void _item_edited();
	TreeItem *get_parent_node(String p_path,HashMap<String,TreeItem*>& item_paths,TreeItem *root);

	void set_item_text(TreeItem *p_item, int p_type, const String& p_name, int p_hint=PROPERTY_HINT_NONE, const String& p_hint_text="");

	TreeItem *find_item(TreeItem *p_item,const String& p_name);


	virtual void _changed_callback(Object *p_changed,const char * p_what);
	virtual void _changed_callbacks(Object *p_changed,const String& p_callback);


	void _edit_button(Object *p_item, int p_column, int p_button);

	void _node_removed(Node *p_node);
	void _edit_set(const String& p_name, const Variant& p_value);
	void _draw_flags(Object *ti,const Rect2& p_rect);

	bool _might_be_in_instance();
	bool _get_instanced_node_original_property(const StringName& p_prop,Variant& value);
	bool _is_property_different(const Variant& p_current, const Variant& p_orig,int p_usage=0);

	void _refresh_item(TreeItem *p_item);
	void _set_range_def(Object *p_item, String prop, float p_frame);

	UndoRedo *undo_redo;
protected:

	void _notification(int p_what);
	static void _bind_methods();
public:

	void set_undo_redo(UndoRedo *p_undo_redo) { undo_redo=p_undo_redo; }

	String get_selected_path() const;

	Tree *get_scene_tree();
	Label* get_top_label();
	void hide_top_label();
	void update_tree();
	void update_property(const String& p_prop);

	void refresh();

	void edit(Object* p_object);

	void set_keying(bool p_active);
	void set_read_only(bool p_read_only) { read_only=p_read_only; custom_editor->set_read_only(p_read_only);}

	void set_capitalize_paths(bool p_capitalize);
	void set_autoclear(bool p_enable);

	void set_show_categories(bool p_show);
	void set_use_doc_hints(bool p_enable) { use_doc_hints=p_enable; }
	
	PropertyEditor();	
	~PropertyEditor();

};

#endif
