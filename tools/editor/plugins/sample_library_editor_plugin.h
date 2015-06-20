/*************************************************************************/
/*  sample_library_editor_plugin.h                                       */
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
#ifndef SAMPLE_LIBRARY_EDITOR_PLUGIN_H
#define SAMPLE_LIBRARY_EDITOR_PLUGIN_H



#include "tools/editor/editor_plugin.h"
#include "tools/editor/editor_node.h"
#include "scene/audio/sample_player.h"
#include "scene/resources/sample.h"
#include "scene/gui/tree.h"
#include "scene/gui/file_dialog.h"
#include "scene/gui/dialogs.h"


class SampleLibraryEditor : public Panel {

	OBJ_TYPE(SampleLibraryEditor, Panel );



	SamplePlayer *player;
	Ref<SampleLibrary> sample_library;
	Button *stop;
	Button *play;
	Button *load;
	Button *_delete;
	Tree *tree;

	EditorFileDialog *file;

	ConfirmationDialog *dialog;


	void _play_pressed();
	void _stop_pressed();
	void _load_pressed();
	void _file_load_request(const DVector<String>& p_path);
	void _delete_pressed();
	void _delete_confirm_pressed();
	void _update_library();
	void _item_edited();

	UndoRedo *undo_redo;

	void _button_pressed(Object *p_item,int p_column, int p_id);

protected:
	void _notification(int p_what);
	void _input_event(InputEvent p_event);
	static void _bind_methods();
public:

	void set_undo_redo(UndoRedo *p_undo_redo) {undo_redo=p_undo_redo; }

	void edit(Ref<SampleLibrary> p_sample);
	SampleLibraryEditor();
};

class SampleLibraryEditorPlugin : public EditorPlugin {

	OBJ_TYPE( SampleLibraryEditorPlugin, EditorPlugin );

	SampleLibraryEditor *sample_library_editor;
	EditorNode *editor;

public:

	virtual String get_name() const { return "SampleLibrary"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_node);
	virtual bool handles(Object *p_node) const;
	virtual void make_visible(bool p_visible);

	SampleLibraryEditorPlugin(EditorNode *p_node);
	~SampleLibraryEditorPlugin();

};

#endif // SAMPLE_LIBRARY_EDITOR_PLUGIN_H
