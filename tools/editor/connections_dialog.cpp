/*************************************************************************/
/*  connections_dialog.cpp                                               */
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
#include "connections_dialog.h"


#include "scene/gui/label.h"
 

#include "print_string.h"
#include "editor_settings.h"
#include "editor_node.h"

class ConnectDialogBinds : public Object {

	OBJ_TYPE( ConnectDialogBinds, Object );
public:

	Vector<Variant> params;

	bool _set(const StringName& p_name, const Variant& p_value) {

		String name=p_name;

		if (name.begins_with("bind/")) {
			int which = name.get_slice("/",1).to_int()-1;
			ERR_FAIL_INDEX_V(which,params.size(),false);
			params[which]=p_value;
		} else
			return false;

		return true;
	}


	bool _get(const StringName& p_name,Variant &r_ret) const {

		String name=p_name;

		if (name.begins_with("bind/")) {
			int which = name.get_slice("/",1).to_int()-1;
			ERR_FAIL_INDEX_V(which,params.size(),false);
			r_ret = params[which];
		} else
			return false;

		return true;
	}

	void _get_property_list(List<PropertyInfo> *p_list) const {

		for(int i=0;i<params.size();i++) {
			p_list->push_back( PropertyInfo( params[i].get_type(), "bind/"+itos(i+1)) );
		}
	}

	void notify_changed() {

		_change_notify();
	}

	ConnectDialogBinds() {


	}
};

void ConnectDialog::_notification(int p_what) {
	
	if (p_what==NOTIFICATION_DRAW) {
		
		RID ci = get_canvas_item();
		get_stylebox("panel","PopupMenu")->draw(ci,Rect2(Point2(),get_size()));		
	}	
}

void ConnectDialog::_tree_node_selected() {
	
	//dst_method_list->get_popup()->clear();
	Node *current=tree->get_selected();

	if (!current) {
		make_callback->hide();
		return;
	}


	if (current->get_script().is_null())
		make_callback->hide();
	else
		make_callback->show();
#if 0
	List<MethodInfo> methods;
	current->get_method_list(&methods);
	for (List<MethodInfo>::Element *E=methods.front();E;E=E->next()) {

		if (E->get().name.length() && E->get().name[0]=='_')
			continue; // hidden method, not show!

		if (ObjectTypeDB::has_method(node->get_type(),"Node") || ObjectTypeDB::has_method(node->get_type(),"Control",true))
			continue; //avoid too much unnecesary stuff

		String method=E->get().name+"(";
		for(int i=0;i<E->get().arguments.size();i++) {

			if (i!=0)
				method+=", ";
			method+=Variant::get_type_name(E->get().arguments[i].type);
			if (E->get().arguments[i].name.length()) {
				method+=" ";
				method+=E->get().arguments[i].name;
			}
		}
		method+=")";

		//dst_method_list->get_popup()->add_item(method);
	}
#endif
	dst_path->set_text(node->get_path_to(current));


}

void ConnectDialog::_dst_method_list_selected(int p_idx) {
	
	//dst_method->set_text( dst_method_list->get_popup()->get_item_text(p_idx));
}

void ConnectDialog::edit(Node *p_node) {
	
	node=p_node;
	
	//dst_method_list->get_popup()->clear();
	
	
	tree->set_selected(NULL);
	tree->set_marked(node,true);
	dst_path->set_text("");
	dst_method->set_text("");
	deferred->set_pressed(false);
	cdbinds->params.clear();
	cdbinds->notify_changed();
}

void ConnectDialog::ok_pressed() {
	
	if (dst_method->get_text()=="") {

		error->set_text("Method in target Node must be specified!");
		error->popup_centered_minsize();
		return;
	}
	emit_signal("connected");
	hide();

}
void ConnectDialog::_cancel_pressed() {
	
	hide();
}


NodePath ConnectDialog::get_dst_path() const {
	
	return dst_path->get_text();
}


bool ConnectDialog::get_deferred() const {

	return deferred->is_pressed();
}

StringName ConnectDialog::get_dst_method() const {
	
	String txt=dst_method->get_text();
	if (txt.find("(")!=-1)
		txt=txt.left( txt.find("(")).strip_edges();
	return txt;	
}


Vector<Variant> ConnectDialog::get_binds() const {


	return cdbinds->params;
}

void ConnectDialog::_add_bind() {

	if (cdbinds->params.size() >= VARIANT_ARG_MAX)
		return;
	Variant::Type vt = (Variant::Type)type_list->get_item_ID(type_list->get_selected());

	Variant value;

	switch(vt) {

		case Variant::BOOL: value = false ; break;
		case Variant::INT: value = 0; break;
		case Variant::REAL: value = 0.0; break;
		case Variant::STRING: value = ""; break;
		case Variant::VECTOR2: value = Vector2(); break;
		case Variant::RECT2: value = Rect2(); break;
		case Variant::VECTOR3: value = Vector3(); break;
		case Variant::PLANE: value = Plane(); break;
		case Variant::QUAT: value = Quat(); break;
		case Variant::_AABB: value = AABB(); break;
		case Variant::MATRIX3: value = Matrix3(); break;
		case Variant::TRANSFORM: value = Transform(); break;
		case Variant::COLOR: value = Color(); break;
		case Variant::IMAGE: value = Image(); break;

		default: { ERR_FAIL(); } break;
	}

	ERR_FAIL_COND(value.get_type()==Variant::NIL);

	cdbinds->params.push_back(value);
	cdbinds->notify_changed();

}

void ConnectDialog::_remove_bind() {

	String st = bind_editor->get_selected_path();
	if (st=="")
		return;
	int idx = st.get_slice("/",1).to_int()-1;

	ERR_FAIL_INDEX(idx,cdbinds->params.size());
	cdbinds->params.remove(idx);
	cdbinds->notify_changed();

}

void ConnectDialog::set_dst_node(Node* p_node) {

	tree->set_selected(p_node);
}

void ConnectDialog::set_dst_method(const StringName& p_method) {

	dst_method->set_text(p_method);
}

void ConnectDialog::_bind_methods() {
	
	//ObjectTypeDB::bind_method("_ok",&ConnectDialog::_ok_pressed);
	ObjectTypeDB::bind_method("_cancel",&ConnectDialog::_cancel_pressed);
	//ObjectTypeDB::bind_method("_dst_method_list_selected",&ConnectDialog::_dst_method_list_selected);
	ObjectTypeDB::bind_method("_tree_node_selected",&ConnectDialog::_tree_node_selected);

	ObjectTypeDB::bind_method("_add_bind",&ConnectDialog::_add_bind);
	ObjectTypeDB::bind_method("_remove_bind",&ConnectDialog::_remove_bind);

	ADD_SIGNAL( MethodInfo("connected") );
}

ConnectDialog::ConnectDialog() {
	
	int margin = get_constant("margin","Dialogs");
	int button_margin = get_constant("button_margin","Dialogs");
	
	
	Label * label = memnew( Label );
	label->set_pos( Point2( 8,11) );
	label->set_text("Connect To Node:");
	

	add_child(label);	
	label = memnew( Label );
	label->set_anchor( MARGIN_LEFT, ANCHOR_RATIO );
	label->set_pos( Point2( 0.5,11) );
	label->set_text("Binds (Extra Params):");
	add_child(label);


	tree = memnew(SceneTreeEditor(false));
	tree->set_anchor( MARGIN_RIGHT, ANCHOR_RATIO );
	tree->set_anchor( MARGIN_BOTTOM, ANCHOR_END );
	tree->set_begin( Point2( 15,32) );
	tree->set_end( Point2( 0.5,127 ) );

	add_child(tree);

	bind_editor = memnew( PropertyEditor );
	bind_editor->set_anchor( MARGIN_RIGHT, ANCHOR_END );
	bind_editor->set_anchor( MARGIN_LEFT, ANCHOR_RATIO );
	bind_editor->set_anchor( MARGIN_BOTTOM, ANCHOR_END );
	bind_editor->set_begin( Point2( 0.51,42) );
	bind_editor->set_end( Point2( 15,127 ) );
	bind_editor->get_top_label()->hide();

	add_child(bind_editor);

	type_list = memnew( OptionButton );
	type_list->set_anchor( MARGIN_RIGHT, ANCHOR_RATIO );
	type_list->set_anchor( MARGIN_LEFT, ANCHOR_RATIO );
	type_list->set_begin( Point2( 0.51,32) );
	type_list->set_end( Point2( 0.75,33 ) );
	add_child(type_list);


	type_list->add_item("bool",Variant::BOOL);
	type_list->add_item("int",Variant::INT);
	type_list->add_item("real",Variant::REAL);
	type_list->add_item("string",Variant::STRING);
	//type_list->add_separator();
	type_list->add_item("Vector2",Variant::VECTOR2);
	type_list->add_item("Rect2",Variant::RECT2);
	type_list->add_item("Vector3",Variant::VECTOR3);
	type_list->add_item("Plane",Variant::PLANE);
	type_list->add_item("Quat",Variant::QUAT);
	type_list->add_item("AABB",Variant::_AABB);
	type_list->add_item("Matrix3",Variant::MATRIX3);
	type_list->add_item("Transform",Variant::TRANSFORM);
	//type_list->add_separator();
	type_list->add_item("Color",Variant::COLOR);
	type_list->add_item("Image",Variant::IMAGE);
	type_list->select(0);

	Button *add_bind = memnew( Button );
	add_bind->set_anchor( MARGIN_RIGHT, ANCHOR_RATIO );
	add_bind->set_anchor( MARGIN_LEFT, ANCHOR_RATIO );
	add_bind->set_begin( Point2( 0.76,32) );
	add_bind->set_end( Point2( 0.84,33 ) );
	add_bind->set_text("Add");
	add_child(add_bind);
	add_bind->connect("pressed",this,"_add_bind");

	Button *del_bind = memnew( Button );
	del_bind->set_anchor( MARGIN_RIGHT, ANCHOR_END );
	del_bind->set_anchor( MARGIN_LEFT, ANCHOR_RATIO );
	del_bind->set_begin( Point2( 0.85,32) );
	del_bind->set_end( Point2( 15,33 ) );
	del_bind->set_text("Remove");
	add_child(del_bind);
	del_bind->connect("pressed",this,"_remove_bind");


	label = memnew( Label );
	label->set_anchor( MARGIN_TOP, ANCHOR_END );
	label->set_begin( Point2( 8,124) );
	label->set_end( Point2( 15,99) );
	label->set_text("Path To Node:");
	
	add_child(label);

	dst_path = memnew(LineEdit);
	dst_path->set_anchor( MARGIN_TOP, ANCHOR_END );
	dst_path->set_anchor( MARGIN_RIGHT, ANCHOR_END );
	dst_path->set_anchor( MARGIN_BOTTOM, ANCHOR_END );
	dst_path->set_begin( Point2( 15,105) );
	dst_path->set_end( Point2( 15,80 ) );

	add_child(dst_path);
	
	label = memnew( Label );
	label->set_anchor( MARGIN_TOP, ANCHOR_END );
	label->set_anchor( MARGIN_RIGHT, ANCHOR_END );
	label->set_anchor( MARGIN_BOTTOM, ANCHOR_END );	
	label->set_begin( Point2( 8,78 ) );
	label->set_end( Point2( 15,52 ) );
	label->set_text("Method In Node:");
	add_child(label);


	HBoxContainer *dstm_hb = memnew( HBoxContainer );
	dstm_hb->set_anchor( MARGIN_TOP, ANCHOR_END );
	dstm_hb->set_anchor( MARGIN_RIGHT, ANCHOR_END );
	dstm_hb->set_anchor( MARGIN_BOTTOM, ANCHOR_END );
	dstm_hb->set_begin( Point2( 15,59) );
	dstm_hb->set_end( Point2( 15,39 ) );
	add_child(dstm_hb);

	dst_method = memnew(LineEdit);
	dst_method->set_h_size_flags(SIZE_EXPAND_FILL);
	dstm_hb->add_child(dst_method);



	/*dst_method_list = memnew( MenuButton );
	dst_method_list->set_text("List..");
	dst_method_list->set_anchor( MARGIN_RIGHT, ANCHOR_END );
	dst_method_list->set_anchor( MARGIN_LEFT, ANCHOR_END );
	dst_method_list->set_anchor( MARGIN_TOP, ANCHOR_END );
	dst_method_list->set_anchor( MARGIN_BOTTOM, ANCHOR_END );		
	dst_method_list->set_begin( Point2( 70,59) );
	dst_method_list->set_end( Point2( 15,39  ) );
	*/
	//add_child(dst_method_list);

	make_callback = memnew( CheckButton );
	make_callback->set_toggle_mode(true);
	make_callback->set_pressed( EDITOR_DEF("text_editor/create_signal_callbacks",true));
	make_callback->set_text("Make Function  ");
	dstm_hb->add_child(make_callback);

	deferred = memnew( CheckButton );
	deferred->set_toggle_mode(true);
	deferred->set_pressed(true);
	deferred->set_text("Deferred");
	dstm_hb->add_child(deferred);

	
/*
	realtime = memnew( CheckButton );
	realtime->set_anchor( MARGIN_TOP, ANCHOR_END );
	realtime->set_anchor( MARGIN_BOTTOM, ANCHOR_END );
	realtime->set_anchor( MARGIN_RIGHT, ANCHOR_END );
	realtime->set_begin( Point2( 120, button_margin-10 ) );
	realtime->set_end( Point2( 80, margin ) );	
	realtime->set_text("Realtime");
	add_child(realtime);
*/
	


	
//	dst_method_list->get_popup()->connect("item_pressed", this,"_dst_method_list_selected");
	tree->connect("node_selected", this,"_tree_node_selected");

	set_as_toplevel(true);

	cdbinds = memnew( ConnectDialogBinds );
	bind_editor->edit(cdbinds);

	error = memnew( ConfirmationDialog );
	add_child(error);
	error->get_ok()->set_text("Close");
	get_ok()->set_text("Connect");
//	error->get_cancel()->set_text("Close");


	
	
}


ConnectDialog::~ConnectDialog()
{
	memdelete( cdbinds );
}



void ConnectionsDialog::_notification(int p_what) {
	
	if (p_what==NOTIFICATION_DRAW) {
		
		RID ci = get_canvas_item();
		get_stylebox("panel","PopupMenu")->draw(ci,Rect2(Point2(),get_size()));		
	}	
}

void ConnectionsDialog::_close() {
	
	hide();	
}

void ConnectionsDialog::_connect() {
	
	TreeItem *it = tree->get_selected();
	ERR_FAIL_COND(!it);
	String signal=it->get_metadata(0).operator Dictionary()["name"];

	NodePath dst_path=connect_dialog->get_dst_path();
	Node *target = node->get_node(dst_path);
	ERR_FAIL_COND(!target);

	StringName dst_method=connect_dialog->get_dst_method();
	bool defer=connect_dialog->get_deferred();
	Vector<Variant> binds = connect_dialog->get_binds();
	StringArray args =  it->get_metadata(0).operator Dictionary()["args"];

	undo_redo->create_action("Connect '"+signal+"' to '"+String(dst_method)+"'");
	undo_redo->add_do_method(node,"connect",signal,target,dst_method,binds,CONNECT_PERSIST | (defer?CONNECT_DEFERRED:0));
	undo_redo->add_undo_method(node,"disconnect",signal,target,dst_method);
	undo_redo->add_do_method(this,"update_tree");
	undo_redo->add_undo_method(this,"update_tree");
	undo_redo->commit_action();


	if (connect_dialog->get_make_callback()) {

		print_line("request connect");
		editor->emit_signal("script_add_function_request",target,dst_method,args);
		hide();
	}

	update_tree();

}




void ConnectionsDialog::ok_pressed() {
	

	TreeItem *item = tree->get_selected();
	if (!item) {
		//no idea how this happened, but disable
		get_ok()->set_disabled(true);
		return;
	}
	if (item->get_parent()==tree->get_root() || item->get_parent()->get_parent()==tree->get_root()) {
		//a signal - connect
		String signal=item->get_metadata(0).operator Dictionary()["name"];
		String signalname=signal;
		String midname=node->get_name();
		for(int i=0;i<midname.length();i++) {
			CharType c = midname[i];
			if  ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_') {
				//all good
			} else if (c==' ') {
				c='_';
			} else {
				midname.remove(i);
				i--;
				continue;
			}

			midname[i]=c;
		}

		connect_dialog->edit(node);
		connect_dialog->popup_centered_ratio();
		connect_dialog->set_dst_method("_on_"+midname+"_"+signal);
		connect_dialog->set_dst_node(node->get_owner()?node->get_owner():node);

	} else {
		//a slot- disconnect
		Connection c=item->get_metadata(0);
		ERR_FAIL_COND(c.source!=node); //shouldn't happen but...bugcheck

		undo_redo->create_action("Create Subscription");
		undo_redo->add_do_method(node,"disconnect",c.signal,c.target,c.method);
		undo_redo->add_undo_method(node,"connect",c.signal,c.target,c.method,Vector<Variant>(),c.flags);
		undo_redo->add_do_method(this,"update_tree");
		undo_redo->add_undo_method(this,"update_tree");
		undo_redo->commit_action();

		c.source->disconnect(c.signal,c.target,c.method);
		update_tree();
	}
}
/*
void ConnectionsDialog::_remove() {
	
	if (!tree->get_selected())
		return;
	
	TreeItem *selected=tree->get_selected();
	if (!selected)
		return;
	
	Dictionary meta=selected->get_metadata(0);
	
	remove_confirm->set_text(String()+"Remove Connection \""+meta["from_event"].operator String()+"\" ?");
	remove_confirm->popup_centered(Size2(340,80));
}
*/
/*
void ConnectionsDialog::_remove_confirm() {
	
	if (!tree->get_selected())
		return;
	TreeItem *selected=tree->get_selected();
	if (!selected)
		return;
	
	Dictionary meta=selected->get_metadata(0);
	
	undo_redo->create_action("Remove Subscription");
	undo_redo->add_do_method(node,"unsubscribe_path_event",meta["from_event"].operator String(),meta["from_path"].operator NodePath(),meta["to_method"].operator String());
	undo_redo->add_undo_method(node,"subscribe_path_event_persist",meta["from_event"].operator String(),meta["from_path"].operator NodePath(),meta["to_method"].operator String(),Array(),false);
	undo_redo->add_do_method(this,"update_tree");
	undo_redo->add_undo_method(this,"update_tree");
	undo_redo->commit_action();

}
*/

struct _ConnectionsDialogMethodInfoSort {

	_FORCE_INLINE_ bool operator()(const MethodInfo& a, const MethodInfo& b) const {
		return a.name < b.name;
	}
};

void ConnectionsDialog::update_tree() {
	
	if (!is_visible())
		return; //don't update if not visible, of course
	tree->clear();
	
	if (!node)
		return;
	

	TreeItem *root=tree->create_item();

	List<MethodInfo> node_signals;

	node->get_signal_list(&node_signals);

	//node_signals.sort_custom<_ConnectionsDialogMethodInfoSort>();
	bool did_script=false;
	StringName base = node->get_type();

	while(base) {

		List<MethodInfo> node_signals;
		Ref<Texture> icon;
		String name;

		if (!did_script) {

			Ref<Script> scr = node->get_script();
			if (scr.is_valid()) {
				scr->get_script_signal_list(&node_signals);
				if (scr->get_path().is_resource_file())
					name=scr->get_path().get_file();
				else
					name=scr->get_type();

				if (has_icon(scr->get_type(),"EditorIcons")) {
					icon=get_icon(scr->get_type(),"EditorIcons");
				}
			}

		} else {

			ObjectTypeDB::get_signal_list(base,&node_signals,true);
			if (has_icon(base,"EditorIcons")) {
				icon=get_icon(base,"EditorIcons");
			}
			name=base;
		}


		TreeItem *pitem = NULL;

		if (node_signals.size()) {
			pitem=tree->create_item(root);
			pitem->set_text(0,name);
			pitem->set_icon(0,icon);
			pitem->set_selectable(0,false);
			pitem->set_editable(0,false);
			pitem->set_custom_bg_color(0,get_color("prop_subsection","Editor"));
			node_signals.sort();
		}

		for(List<MethodInfo>::Element *E=node_signals.front();E;E=E->next()) {


			MethodInfo &mi =E->get();

			String signaldesc;
			signaldesc=mi.name+"(";
			StringArray argnames;
			if (mi.arguments.size()) {
				signaldesc+=" ";
				for(int i=0;i<mi.arguments.size();i++) {

					PropertyInfo &pi = mi.arguments[i];

					if (i>0)
						signaldesc+=", ";
					String tname="var";
					if (pi.type!=Variant::NIL) {
						tname=Variant::get_type_name(pi.type);
					}
					signaldesc+=tname+" "+(pi.name==""?String("arg "+itos(i)):pi.name);
					argnames.push_back(pi.name);

				}
				signaldesc+=" ";
			}

			signaldesc+=")";

			TreeItem *item=tree->create_item(pitem);
			item->set_text(0,signaldesc);
			Dictionary sinfo;
			sinfo["name"]=mi.name;
			sinfo["args"]=argnames;
			item->set_metadata(0,sinfo);
			item->set_icon(0,get_icon("Signal","EditorIcons"));

			List<Object::Connection> connections;
			node->get_signal_connection_list(mi.name,&connections);

			for(List<Object::Connection>::Element *F=connections.front();F;F=F->next()) {

				Object::Connection&c = F->get();
				if (!(c.flags&CONNECT_PERSIST))
					continue;

				Node *target = c.target->cast_to<Node>();
				if (!target)
					continue;

				String path = String(node->get_path_to(target))+" :: "+c.method+"()";
				if (c.flags&CONNECT_DEFERRED)
					path+=" (deferred)";
				if (c.binds.size()) {

					path+=" binds( ";
					for(int i=0;i<c.binds.size();i++) {

						if (i>0)
							path+=", ";
						path+=c.binds[i].operator String();
					}
					path+=" )";
				}

				TreeItem *item2=tree->create_item(item);
				item2->set_text(0,path);
				item2->set_metadata(0,c);
				item2->set_icon(0,get_icon("Slot","EditorIcons"));


			}
		}

		if (!did_script) {
			did_script=true;
		} else {
			base=ObjectTypeDB::type_inherits_from(base);
		}
	}

	get_ok()->set_text("Connect");
	get_ok()->set_disabled(true);

}

void ConnectionsDialog::set_node(Node* p_node) {
	
	node=p_node;
	update_tree();
}

void ConnectionsDialog::_something_selected() {

	TreeItem *item = tree->get_selected();
	if (!item) {
		//no idea how this happened, but disable
		get_ok()->set_text("Connect..");
		get_ok()->set_disabled(true);

	} else if (item->get_parent()==tree->get_root() || item->get_parent()->get_parent()==tree->get_root()) {
		//a signal - connect
		get_ok()->set_text("Connect..");
		get_ok()->set_disabled(false);

	} else {
		//a slot- disconnect
		get_ok()->set_text("Disconnect");
		get_ok()->set_disabled(false);
	}

}

void ConnectionsDialog::_bind_methods() {
	

	ObjectTypeDB::bind_method("_connect",&ConnectionsDialog::_connect);
	ObjectTypeDB::bind_method("_something_selected",&ConnectionsDialog::_something_selected);
	ObjectTypeDB::bind_method("_close",&ConnectionsDialog::_close);
//	ObjectTypeDB::bind_method("_remove_confirm",&ConnectionsDialog::_remove_confirm);
	ObjectTypeDB::bind_method("update_tree",&ConnectionsDialog::update_tree);

	
}

ConnectionsDialog::ConnectionsDialog(EditorNode *p_editor) {
	
	editor=p_editor;
	set_title("Edit Connections..");
	set_hide_on_ok(false);

	VBoxContainer *vbc = memnew( VBoxContainer );
	add_child(vbc);
	set_child_rect(vbc);


	
	tree = memnew( Tree );
	tree->set_columns(1);
	tree->set_select_mode(Tree::SELECT_ROW);
	tree->set_hide_root(true);
	vbc->add_margin_child("Connections:",tree,true);
	
//	add_child(tree);
		
	connect_dialog = memnew( ConnectDialog );
	connect_dialog->set_as_toplevel(true);
	add_child(connect_dialog);
	
	remove_confirm = memnew( ConfirmationDialog );
	remove_confirm->set_as_toplevel(true);
	add_child(remove_confirm);
		
	/*	
	node_only->set_anchor( MARGIN_TOP, ANCHOR_END );
	node_only->set_anchor( MARGIN_BOTTOM, ANCHOR_END );
	node_only->set_anchor( MARGIN_RIGHT, ANCHOR_END );
	
	node_only->set_begin( Point2( 20,51) );
	node_only->set_end( Point2( 10,44) );
	*/


	remove_confirm->connect("confirmed", this,"_remove_confirm");
	connect_dialog->connect("connected", this,"_connect");
	tree->connect("item_selected", this,"_something_selected");
	get_cancel()->set_text("Close");
	
}


ConnectionsDialog::~ConnectionsDialog()
{
}


