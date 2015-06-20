/*************************************************************************/
/*  editor_data.h                                                        */
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
#ifndef EDITOR_DATA_H
#define EDITOR_DATA_H

#include "tools/editor/editor_plugin.h"
#include "scene/resources/texture.h"
#include "list.h"
#include "undo_redo.h"
#include "pair.h"
#include "default_saver.h"

class EditorHistory {

	enum {

		HISTORY_MAX=64
	};

	struct Obj {

		RES res;
		ObjectID object;
		String property;
	};

	struct History {

		Vector<Obj> path;
		int level;
	};

	Vector<History> history;
	int current;

	//Vector<EditorPlugin*> editor_plugins;

	struct PropertyData {

		String name;
		Variant value;
	};


	void _cleanup_history();

	void _add_object(ObjectID p_object,const String& p_property,int p_level_change);

public:

	void add_object(ObjectID p_object);
	void add_object(ObjectID p_object,const String& p_subprop);
	void add_object(ObjectID p_object,int p_relevel);

	bool next();
	bool previous();
	ObjectID get_current();

	int get_path_size() const;
	ObjectID get_path_object(int p_index) const;
	String get_path_property(int p_index) const;

	void clear();

	EditorHistory();
};

class EditorData {

public:
	struct CustomType {

		String name;
		Ref<Script> script;
		Ref<Texture> icon;
	};
private:

	Vector<EditorPlugin*> editor_plugins;

	struct PropertyData {

		String name;
		Variant value;
	};
	Map<String,Vector<CustomType> > custom_types;

	List<PropertyData> clipboard;
	UndoRedo undo_redo;


	void _cleanup_history();


public:

	EditorPlugin* get_editor(Object *p_object);
	EditorPlugin* get_subeditor(Object *p_object);
	EditorPlugin* get_editor(String p_name);

	void copy_object_params(Object *p_object);
	void paste_object_params(Object *p_object);

	Dictionary get_editor_states() const;
	void set_editor_states(const Dictionary& p_states);
	void get_editor_breakpoints(List<String> *p_breakpoints);
	void clear_editor_states();
	void save_editor_external_data();
	void apply_changes_in_editors();

	void add_editor_plugin(EditorPlugin *p_plugin);
	void remove_editor_plugin(EditorPlugin *p_plugin);

	UndoRedo &get_undo_redo();

	void save_editor_global_states();
	void restore_editor_global_states();

	void add_custom_type(const String& p_type, const String& p_inherits,const Ref<Script>& p_script,const Ref<Texture>& p_icon );
	void remove_custom_type(const String& p_type);
	const Map<String,Vector<CustomType> >& get_custom_types() const { return custom_types; }

	EditorData();
};



class EditorSelection : public Object {

	OBJ_TYPE(EditorSelection,Object);
public:

	Map<Node*,Object*> selection;

	bool changed;
	bool nl_changed;

	void _node_removed(Node *p_node);

	List<Object*> editor_plugins;
	List<Node*> selected_node_list;

	void _update_nl();
protected:

	static void _bind_methods();
public:

	void add_node(Node *p_node);
	void remove_node(Node *p_node);
	bool is_selected(Node *) const;

	template<class T>
	T* get_node_editor_data(Node *p_node) {
		if (!selection.has(p_node))
			return NULL;
		Object *obj = selection[p_node];
		if (!obj)
			return NULL;
		return obj->cast_to<T>();
	}

	void add_editor_plugin(Object *p_object);

	void update();
	void clear();


	List<Node*>& get_selected_node_list();
	Map<Node*,Object*>& get_selection() { return selection; }


	EditorSelection();
	~EditorSelection();
};


#endif // EDITOR_DATA_H
