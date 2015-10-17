#ifndef POLYGON_2D_EDITOR_PLUGIN_H
#define POLYGON_2D_EDITOR_PLUGIN_H

#include "tools/editor/editor_plugin.h"
#include "tools/editor/editor_node.h"
#include "scene/2d/polygon_2d.h"
#include "scene/gui/tool_button.h"
#include "scene/gui/button_group.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class CanvasItemEditor;

class Polygon2DEditor : public HBoxContainer {

	OBJ_TYPE(Polygon2DEditor, HBoxContainer );

	UndoRedo *undo_redo;
	enum Mode {

		MODE_CREATE,
		MODE_EDIT,
		MODE_EDIT_UV,
		UVEDIT_POLYGON_TO_UV,
		UVEDIT_UV_TO_POLYGON,
		UVEDIT_UV_CLEAR

	};

	enum UVMode {
		UV_MODE_EDIT_POINT,
		UV_MODE_MOVE,
		UV_MODE_ROTATE,
		UV_MODE_SCALE,
		UV_MODE_MAX
	};

	Mode mode;

	UVMode uv_mode;
	AcceptDialog *uv_edit;
	ToolButton *uv_button[4];
	ToolButton *b_snap_enable;
	ToolButton *b_snap_grid;
	Control *uv_edit_draw;
	HSlider *uv_zoom;
	SpinBox *uv_zoom_value;
	HScrollBar *uv_hscroll;
	VScrollBar *uv_vscroll;
	MenuButton *uv_menu;
	TextureFrame *uv_icon_zoom;

	Vector2 uv_draw_ofs;
	float uv_draw_zoom;
	DVector<Vector2> uv_prev;
	int uv_drag_index;
	bool uv_drag;
	UVMode uv_move_current;
	Vector2 uv_drag_from;
	bool updating_uv_scroll;



	AcceptDialog *error;

	ToolButton *button_create;
	ToolButton *button_edit;
	ToolButton *button_uv;

	CanvasItemEditor *canvas_item_editor;
	EditorNode *editor;
	Panel *panel;
	Polygon2D *node;
	MenuButton *options;

	int edited_point;
	Vector2 edited_point_pos;
	Vector<Vector2> pre_move_edit;
	Vector<Vector2> wip;
	bool wip_active;

	bool use_snap;
	bool snap_show_grid;
	Vector2 snap_offset;
	Vector2 snap_step;

	void _uv_scroll_changed(float);
	void _uv_input(const InputEvent& p_input);
	void _uv_draw();
	void _uv_mode(int p_mode);
	void _wip_close();
	void _canvas_draw();
	void _menu_option(int p_option);

	void _set_use_snap(bool p_use);
	void _set_show_grid(bool p_show);
	void _set_snap_off_x(float p_val);
	void _set_snap_off_y(float p_val);
	void _set_snap_step_x(float p_val);
	void _set_snap_step_y(float p_val);

protected:
	void _notification(int p_what);
	void _node_removed(Node *p_node);
	static void _bind_methods();

	Vector2 snap_point(Vector2 p_target) const;

public:

	bool forward_input_event(const InputEvent& p_event);
	void edit(Node *p_collision_polygon);
	Polygon2DEditor(EditorNode *p_editor);
};

class Polygon2DEditorPlugin : public EditorPlugin {

	OBJ_TYPE( Polygon2DEditorPlugin, EditorPlugin );

	Polygon2DEditor *collision_polygon_editor;
	EditorNode *editor;

public:

	virtual bool forward_input_event(const InputEvent& p_event) { return collision_polygon_editor->forward_input_event(p_event); }

	virtual String get_name() const { return "Polygon2D"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_node);
	virtual bool handles(Object *p_node) const;
	virtual void make_visible(bool p_visible);

	Polygon2DEditorPlugin(EditorNode *p_node);
	~Polygon2DEditorPlugin();

};

#endif // POLYGON_2D_EDITOR_PLUGIN_H
