/*************************************************************************/
/*  animation_player_editor_plugin.h                                     */
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
#ifndef ANIMATION_PLAYER_EDITOR_PLUGIN_H
#define ANIMATION_PLAYER_EDITOR_PLUGIN_H

#include "tools/editor/editor_plugin.h"
#include "tools/editor/editor_node.h"
#include "scene/animation/animation_player.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/texture_button.h"
#include "scene/gui/slider.h"
#include "scene/gui/spin_box.h"


/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class AnimationPlayerEditor : public VBoxContainer {

	OBJ_TYPE(AnimationPlayerEditor, VBoxContainer );

	EditorNode *editor;
	AnimationPlayer *player;

	enum {
		TOOL_COPY_ANIM,
		TOOL_PASTE_ANIM,
		TOOL_EDIT_RESOURCE
	};

	OptionButton *animation;
	Button *stop;
	Button *play;
	Button *play_from;
	Button *play_bw;
	Button *play_bw_from;

//	Button *pause;
	Button *add_anim;
	Button *autoplay;
	Button *rename_anim;
	Button *duplicate_anim;
	Button *edit_anim;
	Button *resource_edit_anim;
	Button *load_anim;
	Button *blend_anim;
	Button *remove_anim;
	MenuButton *tool_anim;
	TextureButton *pin;
	Label *nodename;
	SpinBox *frame;
	HSlider *seek;
	LineEdit *scale;
	LineEdit *name;
	Label *name_title;
	UndoRedo *undo_redo;
	Ref<Texture> autoplay_icon;
	bool last_active;

	EditorFileDialog *file;

	struct BlendEditor {

		AcceptDialog * dialog;
		Tree *tree;
		LineEdit *next;

	} blend_editor;


	ConfirmationDialog *name_dialog;
	ConfirmationDialog *error_dialog;
	bool renaming;


	bool updating;
	bool updating_blends;

	void _select_anim_by_name(const String& p_anim);
	void _play_pressed();
	void _play_from_pressed();
	void _play_bw_pressed();
	void _play_bw_from_pressed();
	void _autoplay_pressed();
	void _stop_pressed();
	void _pause_pressed();
	void _animation_selected(int p_which);
	void _animation_new();
	void _animation_rename();
	void _animation_name_edited();
	void _animation_load();
	void _animation_remove();
	void _animation_blend();
	void _animation_edit();
	void _animation_duplicate();
	void _animation_resource_edit();
	void _scale_changed(const String& p_scale);
	void _file_selected(String p_file);
	void _seek_frame_changed(const String& p_frame);
	void _seek_value_changed(float p_value);
	void _blend_editor_next_changed(const String& p_string);

	void _list_changed();
	void _update_animation();
	void _update_player();
	void _blend_edited();


	void _hide_anim_editors();

	void _animation_player_changed(Object *p_pl);

	void _animation_key_editor_seek(float p_pos);
	void _animation_key_editor_anim_len_changed(float p_new);
	void _unhandled_key_input(const InputEvent& p_ev);
	void _animation_tool_menu(int p_option);

	AnimationPlayerEditor();
protected:

	void _notification(int p_what);
	void _input_event(InputEvent p_event);
	void _node_removed(Node *p_node);
	static void _bind_methods();
public:

	Dictionary get_state() const;
	void set_state(const Dictionary& p_state);


	void ensure_visibility();

	void set_undo_redo(UndoRedo *p_undo_redo) { undo_redo=p_undo_redo; }
	void edit(AnimationPlayer *p_player);
	AnimationPlayerEditor(EditorNode *p_editor);
};

class AnimationPlayerEditorPlugin : public EditorPlugin {

	OBJ_TYPE( AnimationPlayerEditorPlugin, EditorPlugin );

	AnimationPlayerEditor *anim_editor;
	EditorNode *editor;

public:

	virtual Dictionary get_state() const { return anim_editor->get_state(); }
	virtual void set_state(const Dictionary& p_state)  { anim_editor->set_state(p_state); }

	virtual String get_name() const { return "Anim"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_node);
	virtual bool handles(Object *p_node) const;
	virtual void make_visible(bool p_visible);

	AnimationPlayerEditorPlugin(EditorNode *p_node);
	~AnimationPlayerEditorPlugin();

};

#endif // ANIMATION_PLAYER_EDITOR_PLUGIN_H
