/*************************************************************************/
/*  quick_open.h                                                         */
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
#ifndef EDITOR_QUICK_OPEN_H
#define EDITOR_QUICK_OPEN_H

#include "scene/gui/dialogs.h"
#include "scene/gui/tree.h"
#include "editor_file_system.h"
class EditorQuickOpen : public ConfirmationDialog {

	OBJ_TYPE(EditorQuickOpen,ConfirmationDialog )

	LineEdit *search_box;
	Tree *search_options;
	StringName base_type;
	StringName ei;
	StringName ot;
	bool add_directories;


	void _update_search();

	void _sbox_input(const InputEvent& p_ie);
	void _parse_fs(EditorFileSystemDirectory *efsd);


	void _confirmed();
	void _text_changed(const String& p_newtext);

protected:

	void _notification(int p_what);
	static void _bind_methods();
public:

	StringName get_base_type() const;

	void popup(const StringName& p_base,bool p_dontclear=false,bool p_add_dirs=false);
	EditorQuickOpen();
};

#endif // EDITOR_QUICK_OPEN_H
