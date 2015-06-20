/*************************************************************************/
/*  progress_dialog.h                                                    */
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
#ifndef PROGRESS_DIALOG_H
#define PROGRESS_DIALOG_H

#include "scene/gui/popup.h"
#include "scene/gui/box_container.h"
#include "scene/gui/progress_bar.h"
#include "scene/gui/label.h"


class BackgroundProgress : public HBoxContainer {

	OBJ_TYPE(BackgroundProgress,HBoxContainer);

	_THREAD_SAFE_CLASS_

	struct Task {

		HBoxContainer *hb;
		ProgressBar *progress;

	};

	Map<String,Task> tasks;
	Map<String,int> updates;
	void _update();

protected:

	void _add_task(const String& p_task,const String& p_label, int p_steps);
	void _task_step(const String& p_task, int p_step=-1);
	void _end_task(const String& p_task);

	static void _bind_methods();

public:
	void add_task(const String& p_task,const String& p_label, int p_steps);
	void task_step(const String& p_task, int p_step=-1);
	void end_task(const String& p_task);

	BackgroundProgress() {}
};



class ProgressDialog : public Popup {

	OBJ_TYPE( ProgressDialog, Popup );
	struct Task {

		String task;
		VBoxContainer *vb;
		ProgressBar *progress;
		Label *state;

	};

	Map<String,Task> tasks;
	VBoxContainer *main;

	void _popup();
protected:

	void _notification(int p_what);
public:

	void add_task(const String& p_task,const String& p_label, int p_steps);
	void task_step(const String& p_task,const String& p_state, int p_step=-1);
	void end_task(const String& p_task);


	ProgressDialog();
};

#endif // PROGRESS_DIALOG_H
