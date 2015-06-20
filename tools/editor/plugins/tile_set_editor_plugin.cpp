/*************************************************************************/
/*  tile_set_editor_plugin.cpp                                           */
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
#include "tile_set_editor_plugin.h"
#include "scene/2d/sprite.h"
#include "scene/2d/physics_body_2d.h"
void TileSetEditor::edit(const Ref<TileSet>& p_tileset) {

	tileset=p_tileset;
}

void TileSetEditor::_import_scene(Node *scene, Ref<TileSet> p_library, bool p_merge) {

	if (!p_merge)
		p_library->clear();

	for(int i=0;i<scene->get_child_count();i++) {


		Node *child = scene->get_child(i);


		if (!child->cast_to<Sprite>()) {
			if (child->get_child_count()>0) {
				child=child->get_child(0);
				if (!child->cast_to<Sprite>()) {
					continue;
				}

			} else
				continue;


		}

		Sprite *mi = child->cast_to<Sprite>();
		Ref<Texture> texture=mi->get_texture();
		Ref<CanvasItemMaterial> material=mi->get_material();

		if (texture.is_null())
			continue;

		int id = p_library->find_tile_by_name(mi->get_name());
		if (id<0) {

			id=p_library->get_last_unused_tile_id();
			p_library->create_tile(id);
			p_library->tile_set_name(id,mi->get_name());
		}



		p_library->tile_set_texture(id,texture);
		p_library->tile_set_material(id,material);

		Vector2 phys_offset;

		if (mi->is_centered()) {
			Size2 s;
			if (mi->is_region()) {
				s=mi->get_region_rect().size;
			} else {
				s=texture->get_size();
			}
			phys_offset+=-s/2;
		}
		if (mi->is_region()) {
			p_library->tile_set_region(id,mi->get_region_rect());
		}

		Vector<Ref<Shape2D> >collisions;
		Ref<NavigationPolygon> nav_poly;
		Ref<OccluderPolygon2D> occluder;

		for(int j=0;j<mi->get_child_count();j++) {

			Node *child2 = mi->get_child(j);

			if (child2->cast_to<NavigationPolygonInstance>())
				nav_poly=child2->cast_to<NavigationPolygonInstance>()->get_navigation_polygon();

			if (child2->cast_to<LightOccluder2D>())
				occluder=child2->cast_to<LightOccluder2D>()->get_occluder_polygon();

			if (!child2->cast_to<StaticBody2D>())
				continue;
			StaticBody2D *sb = child2->cast_to<StaticBody2D>();
			if (sb->get_shape_count()==0)
				continue;
			Ref<Shape2D> collision=sb->get_shape(0);
			if (collision.is_valid()) {
				collisions.push_back(collision);
			}
		}

		if (collisions.size()) {

			p_library->tile_set_shapes(id,collisions);
			p_library->tile_set_shape_offset(id,-phys_offset);
		} else {
			p_library->tile_set_shape_offset(id,Vector2());

		}

		p_library->tile_set_texture_offset(id,mi->get_offset());
		p_library->tile_set_navigation_polygon(id,nav_poly);
		p_library->tile_set_light_occluder(id,occluder);
		p_library->tile_set_occluder_offset(id,-phys_offset);
		p_library->tile_set_navigation_polygon_offset(id,-phys_offset);


	}
}

void TileSetEditor::_menu_confirm() {

	switch(option) {

		case MENU_OPTION_REMOVE_ITEM: {

			tileset->remove_tile(to_erase);
		} break;
		case MENU_OPTION_MERGE_FROM_SCENE:
		case MENU_OPTION_CREATE_FROM_SCENE: {


			EditorNode *en = editor;
			Node * scene = en->get_edited_scene();
			if (!scene)
				break;

			_import_scene(scene,tileset,option==MENU_OPTION_MERGE_FROM_SCENE);


		} break;
	}
}

void TileSetEditor::_menu_cbk(int p_option) {

	option=p_option;
	switch(p_option) {

		case MENU_OPTION_ADD_ITEM: {

			tileset->create_tile(tileset->get_last_unused_tile_id());
		} break;
		case MENU_OPTION_REMOVE_ITEM: {

			String p = editor->get_property_editor()->get_selected_path();
			if (p.begins_with("/TileSet") && p.get_slice_count("/")>=2) {

				to_erase = p.get_slice("/",2).to_int();
				cd->set_text("Remove Item "+itos(to_erase)+"?");
				cd->popup_centered(Size2(300,60));
			}
		} break;
		case MENU_OPTION_CREATE_FROM_SCENE: {

			cd->set_text("Create from scene?");
			cd->popup_centered(Size2(300,60));
		} break;
		case MENU_OPTION_MERGE_FROM_SCENE: {

			cd->set_text("Merge from scene?");
			cd->popup_centered(Size2(300,60));
		} break;
	}
}



Error TileSetEditor::update_library_file(Node *p_base_scene, Ref<TileSet> ml,bool p_merge) {

	_import_scene(p_base_scene,ml,p_merge);
	return OK;

}

void TileSetEditor::_bind_methods() {

	ObjectTypeDB::bind_method("_menu_cbk",&TileSetEditor::_menu_cbk);
	ObjectTypeDB::bind_method("_menu_confirm",&TileSetEditor::_menu_confirm);
}

TileSetEditor::TileSetEditor(EditorNode *p_editor) {

	Panel *panel = memnew( Panel );
	panel->set_area_as_parent_rect();
	add_child(panel);
	MenuButton * options = memnew( MenuButton );
	panel->add_child(options);
	options->set_pos(Point2(1,1));
	options->set_text("Theme");
	options->get_popup()->add_item("Add Item",MENU_OPTION_ADD_ITEM);
	options->get_popup()->add_item("Remove Selected Item",MENU_OPTION_REMOVE_ITEM);
	options->get_popup()->add_separator();
	options->get_popup()->add_item("Create from Scene",MENU_OPTION_CREATE_FROM_SCENE);
	options->get_popup()->add_item("Merge from Scene",MENU_OPTION_MERGE_FROM_SCENE);
	options->get_popup()->connect("item_pressed", this,"_menu_cbk");
	editor=p_editor;
	cd = memnew(ConfirmationDialog);
	add_child(cd);
	cd->get_ok()->connect("pressed", this,"_menu_confirm");

}

void TileSetEditorPlugin::edit(Object *p_node) {

	if (p_node && p_node->cast_to<TileSet>()) {
		tileset_editor->edit( p_node->cast_to<TileSet>() );
		tileset_editor->show();
	} else
		tileset_editor->hide();
}

bool TileSetEditorPlugin::handles(Object *p_node) const{

	return p_node->is_type("TileSet");
}

void TileSetEditorPlugin::make_visible(bool p_visible){

	if (p_visible)
		tileset_editor->show();
	else
		tileset_editor->hide();
}

TileSetEditorPlugin::TileSetEditorPlugin(EditorNode *p_node) {

	tileset_editor = memnew( TileSetEditor(p_node) );

	p_node->get_viewport()->add_child(tileset_editor);
	tileset_editor->set_area_as_parent_rect();
	tileset_editor->set_anchor( MARGIN_RIGHT, Control::ANCHOR_END );
	tileset_editor->set_anchor( MARGIN_BOTTOM, Control::ANCHOR_BEGIN );
	tileset_editor->set_end( Point2(0,22) );
	tileset_editor->hide();

}
