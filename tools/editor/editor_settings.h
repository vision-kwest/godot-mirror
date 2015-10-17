/*************************************************************************/
/*  editor_settings.h                                                    */
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
#ifndef EDITOR_SETTINGS_H
#define EDITOR_SETTINGS_H

#include "object.h"

#include "resource.h"
#include "os/thread_safe.h"

class EditorPlugin;

class EditorSettings : public Resource {

	OBJ_TYPE( EditorSettings, Resource );
private:
	_THREAD_SAFE_CLASS_

public:
	struct Plugin {

		EditorPlugin *instance;
		String path;
		String name;
		String author;
		String version;
		String description;
		bool installs;
		String script;
		Vector<String> install_files;
	};
private:
	Map<String,Plugin> plugins;

	struct VariantContainer {
		int order;
		Variant variant;
		bool hide_from_editor;
		VariantContainer(){ order=0; hide_from_editor=false; }
		VariantContainer(const Variant& p_variant, int p_order) { variant=p_variant; order=p_order; hide_from_editor=false; }
	};

	HashMap<String,PropertyInfo> hints;
	int last_order;
	HashMap<String,VariantContainer> props;
	String resource_path;

	bool _set(const StringName& p_name, const Variant& p_value);
	bool _get(const StringName& p_name,Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

	static Ref<EditorSettings> singleton;

	String config_file_path;
	String settings_path;

	Ref<Resource> clipboard;


	EditorPlugin *_load_plugin_editor(const String& p_path);
	Error _load_plugin(const String& p_path,Plugin& plugin);

	void _load_defaults();

	String project_config_path;

	Vector<String> favorite_dirs;
	Vector<String> recent_dirs;

protected:

	static void _bind_methods();
public:

	enum {
		NOTIFICATION_EDITOR_SETTINGS_CHANGED=10000
	};


	bool has(String p_var) const;
	static EditorSettings *get_singleton();
	void erase(String p_var);
	String get_settings_path() const;
	String get_project_settings_path() const;

	const Map<String,Plugin>& get_plugins() const { return plugins; }

	void scan_plugins();
	void enable_plugins();
	void setup_network();

	void raise_order(const String& p_name);
	static void create();
	static void save();
	static void destroy();

	void notify_changes();

	void set_plugin_enabled(const String& p_plugin,bool p_enabled);
	bool is_plugin_enabled(const String& p_plugin);

	void load_installed_plugin(const String& p_plugin);

	void set_resource_clipboard(const Ref<Resource>& p_resource) { clipboard=p_resource; }
	Ref<Resource> get_resource_clipboard() const { return clipboard; }

	void add_property_hint(const PropertyInfo& p_hint);

	void set_favorite_dirs(const Vector<String>& p_favorite_dirs);
	Vector<String> get_favorite_dirs() const;

	void set_recent_dirs(const Vector<String>& p_recent_dirs);
	Vector<String> get_recent_dirs() const;

	void load_favorites();

	EditorSettings();
	~EditorSettings();

};

//not a macro any longer

#define EDITOR_DEF(m_var,m_val) _EDITOR_DEF(m_var,Variant(m_val))
Variant _EDITOR_DEF( const String& p_var, const Variant& p_default);

#endif // EDITOR_SETTINGS_H
