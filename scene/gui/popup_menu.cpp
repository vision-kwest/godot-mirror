/*************************************************************************/
/*  popup_menu.cpp                                                       */
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
#include "popup_menu.h"
#include "print_string.h"
#include "os/keyboard.h"
#include "translation.h"
#include "os/input.h"

String PopupMenu::_get_accel_text(uint32_t p_accel) const {

	return keycode_get_string(p_accel);
	/*
	String atxt;
	if (p_accel&KEY_MASK_SHIFT)
		atxt+="Shift+";
	if (p_accel&KEY_MASK_ALT)
		atxt+="Alt+";
	if (p_accel&KEY_MASK_CTRL)
		atxt+="Ctrl+";
	if (p_accel&KEY_MASK_META)
		atxt+="Meta+";

	p_accel&=KEY_CODE_MASK;

	atxt+=String::chr(p_accel).to_upper();

	return atxt;
*/
}


Size2 PopupMenu::get_minimum_size() const {

	int vseparation = get_constant("vseparation");
	int hseparation = get_constant("hseparation");
	
	Size2 minsize = get_stylebox("panel")->get_minimum_size();
	Ref<Font> font = get_font("font");
	
	float max_w=0;
	int font_h = font->get_height();
	int check_w = get_icon("checked")->get_width();
	int accel_max_w=0;
	
	for (int i=0;i<items.size();i++) {
	
		Size2 size;
		if (!items[i].icon.is_null()) {
		
			Size2 icon_size = items[i].icon->get_size();
			size.height = MAX( icon_size.height, font_h );
			size.width+=icon_size.width;
			size.width+=hseparation;
		} else {
		
			size.height=font_h;
		}
		
		if (items[i].checkable) {
		
			size.width+=check_w+hseparation;
		}
		
		size.width+=font->get_string_size(items[i].text).width;
		if (i>0)
			size.height+=vseparation;

		if (items[i].accel) {

			int accel_w = hseparation*2;
			accel_w+=font->get_string_size(_get_accel_text(items[i].accel)).width;
			accel_max_w = MAX( accel_w, accel_max_w );
		}

		minsize.height+=size.height;
		max_w = MAX( max_w, size.width );
		
	}
	
	minsize.width+=max_w+accel_max_w;
	
	return minsize;
}

int PopupMenu::_get_mouse_over(const Point2& p_over) const {


	if (p_over.x<0 || p_over.x>=get_size().width)
		return -1;

	Ref<StyleBox> style = get_stylebox("panel");
	
	Point2 ofs=style->get_offset();
	

	if (ofs.y>p_over.y)
		return -1;
	
		
	Ref<Font> font = get_font("font");	
	int vseparation = get_constant("vseparation");
//	int hseparation = get_constant("hseparation");
	float font_h=font->get_height();

	
	for (int i=0;i<items.size();i++) {
	
		if (i>0)
			ofs.y+=vseparation;
		float h;
		
		if (!items[i].icon.is_null()) {
		
			Size2 icon_size = items[i].icon->get_size();
			h = MAX( icon_size.height, font_h );
		} else {
		
			h=font_h;
		}
		
		ofs.y+=h;
		if (p_over.y < ofs.y) {
			return i;
		}			
	}
	
	return -1;

}


void PopupMenu::_activate_submenu(int over) {

	Node* n = get_node(items[over].submenu);
	ERR_EXPLAIN("item subnode does not exist: "+items[over].submenu);
	ERR_FAIL_COND(!n);
	Popup *pm = n->cast_to<Popup>();
	ERR_EXPLAIN("item subnode is not a Popup: "+items[over].submenu);
	ERR_FAIL_COND(!pm);
	if (pm->is_visible())
		return; //already visible!


	Point2 p = get_global_pos();
	Rect2 pr(p,get_size());
	Ref<StyleBox> style = get_stylebox("panel");
	pm->set_pos(p+Point2(get_size().width,items[over]._ofs_cache-style->get_offset().y));
	pm->popup();

	PopupMenu *pum = pm->cast_to<PopupMenu>();
	if (pum) {

		pr.pos-=pum->get_global_pos();
		pum->clear_autohide_areas();
		pum->add_autohide_area(Rect2(pr.pos.x,pr.pos.y,pr.size.x,items[over]._ofs_cache));
		if (over<items.size()-1) {
			int from = items[over+1]._ofs_cache;
			pum->add_autohide_area(Rect2(pr.pos.x,pr.pos.y+from,pr.size.x,pr.size.y-from));
		}

	}

}

void PopupMenu::_submenu_timeout() {

	if (mouse_over==submenu_over) {
		_activate_submenu(mouse_over);
		submenu_over=-1;
	}
}


void PopupMenu::_input_event(const InputEvent &p_event) {

	switch( p_event.type) {

		case InputEvent::KEY: {


			if (!p_event.key.pressed)
				break;

			switch(p_event.key.scancode) {


				case KEY_DOWN: {


					for(int i=mouse_over+1;i<items.size();i++) {

						if (i<0 || i>=items.size())
							continue;

						if (!items[i].separator && !items[i].disabled) {


							mouse_over=i;
							update();
							break;
						}
					}
				} break;
				case KEY_UP: {


					for(int i=mouse_over-1;i>=0;i--) {

						if (i<0 || i>=items.size())
							continue;


						if (!items[i].separator && !items[i].disabled) {


						   mouse_over=i;
						   update();
						   break;
						}
					}
				} break;
				case KEY_RETURN:
				case KEY_ENTER: {

					if (mouse_over>=0 && mouse_over<items.size() && !items[mouse_over].separator) {


						activate_item(mouse_over);

					}
				} break;
			}





		} break;

		case InputEvent::MOUSE_BUTTON: {
	
			
			const InputEventMouseButton &b=p_event.mouse_button;
			if (b.pressed)
				break;			

			switch(b.button_index) {


				case BUTTON_WHEEL_DOWN: {

					if (get_global_pos().y + get_size().y > get_viewport_rect().size.y) {

						int vseparation = get_constant("vseparation");
						Ref<Font> font = get_font("font");

						Point2 pos = get_pos();
						int s = (vseparation+font->get_height())*3;
						pos.y-=s;
						set_pos(pos);

						//update hover
						InputEvent ie;
						ie.type=InputEvent::MOUSE_MOTION;
						ie.mouse_motion.x=b.x;
						ie.mouse_motion.y=b.y+s;
						_input_event(ie);
					}
				} break;
				case BUTTON_WHEEL_UP: {

					if (get_global_pos().y < 0) {

						int vseparation = get_constant("vseparation");
						Ref<Font> font = get_font("font");

						Point2 pos = get_pos();
						int s = (vseparation+font->get_height())*3;
						pos.y+=s;
						set_pos(pos);

						//update hover
						InputEvent ie;
						ie.type=InputEvent::MOUSE_MOTION;
						ie.mouse_motion.x=b.x;
						ie.mouse_motion.y=b.y-s;
						_input_event(ie);


					}
				} break;
				case BUTTON_LEFT: {

					int over=_get_mouse_over(Point2(b.x,b.y));

					if (invalidated_click) {
						invalidated_click=false;
						break;
					}
					if (over<0 || items[over].separator || items[over].disabled) {
						hide();
						break; //non-activable
					}

					if (items[over].submenu!="") {

						_activate_submenu(over);
						return;
					}
					activate_item(over);

				} break;
			}
			
			//update();
		} break;
		case InputEvent::MOUSE_MOTION: {
	

			if (invalidated_click) {
				moved+=Vector2(p_event.mouse_motion.relative_x,p_event.mouse_motion.relative_y);
				if (moved.length()>4)
					invalidated_click=false;

			}

			const InputEventMouseMotion &m=p_event.mouse_motion;
			for(List<Rect2>::Element *E=autohide_areas.front();E;E=E->next()) {

				if (!Rect2(Point2(),get_size()).has_point(Point2(m.x,m.y)) && E->get().has_point(Point2(m.x,m.y))) {
					call_deferred("hide");
					return;
				}
			}

			int over=_get_mouse_over(Point2(m.x,m.y));
			int id = (over<0 || items[over].separator || items[over].disabled)?-1:items[over].ID;

			if (id<0)
				break;

			if (items[over].submenu!="" && submenu_over!=over) {
				submenu_over=over;
				submenu_timer->start();
			}

			if (over!=mouse_over) {
				mouse_over=over;
				update();
			}
		} break;

	}
}


bool PopupMenu::has_point(const Point2& p_point) const {

	if (parent_rect.has_point(p_point))
		return true;
	for(const List<Rect2>::Element *E=autohide_areas.front();E;E=E->next()) {

		if (E->get().has_point(p_point))
			return true;
	}

	return Control::has_point(p_point);
}

void PopupMenu::_notification(int p_what) {

	switch(p_what) {
	

		case NOTIFICATION_DRAW: {
		
			RID ci = get_canvas_item();
			Size2 size=get_size();
		
			Ref<StyleBox> style = get_stylebox("panel");
			Ref<StyleBox> hover = get_stylebox("hover");
			Ref<Font> font = get_font("font");
			Ref<Texture> check = get_icon("checked");
			Ref<Texture> uncheck = get_icon("unchecked");
			Ref<Texture> submenu= get_icon("submenu");
			Ref<StyleBox> separator = get_stylebox("separator");
			
			style->draw( ci, Rect2( Point2(), get_size() ) );
			Point2 ofs=style->get_offset();
			int vseparation = get_constant("vseparation");
			int hseparation = get_constant("hseparation");
			Color font_color = get_color("font_color");
			Color font_color_disabled = get_color("font_color_disabled");
			Color font_color_accel = get_color("font_color_accel");
			Color font_color_hover = get_color("font_color_hover");
			float font_h=font->get_height();
			
			for (int i=0;i<items.size();i++) {
			
				if (i>0)
					ofs.y+=vseparation;
				Point2 item_ofs=ofs;
				float h;
				Size2 icon_size;
								
				if (!items[i].icon.is_null()) {
				
					icon_size = items[i].icon->get_size();
					h = MAX( icon_size.height, font_h );
				} else {
				
					h=font_h;
				}
					
				if (i==mouse_over) {
							
					hover->draw(ci,	Rect2( ofs+Point2(-hseparation,-vseparation), Size2( get_size().width - style->get_minimum_size().width + hseparation*2, h+vseparation*2 ) ));
				}
				
				if (items[i].separator) {
				
					int sep_h=separator->get_center_size().height+separator->get_minimum_size().height;
					separator->draw(ci,	Rect2( ofs+Point2(0,Math::floor((h-sep_h)/2.0)), Size2( get_size().width - style->get_minimum_size().width , sep_h ) ));
					
				}
				
				if (items[i].checkable) {

					if (items[i].checked)
						check->draw(ci, item_ofs+Point2(0,Math::floor((h-check->get_height())/2.0)));
					else
						uncheck->draw(ci, item_ofs+Point2(0,Math::floor((h-check->get_height())/2.0)));

					item_ofs.x+=check->get_width()+hseparation;
				}

				if (!items[i].icon.is_null()) {
					items[i].icon->draw( ci, item_ofs+Point2(0,Math::floor((h-icon_size.height)/2.0)));
					item_ofs.x+=items[i].icon->get_width();
					item_ofs.x+=hseparation;
				}

				if (items[i].submenu!="") {
					submenu->draw( ci, Point2(size.width - style->get_margin(MARGIN_RIGHT) - submenu->get_width(),item_ofs.y+Math::floor(h-submenu->get_height())/2));
				}

				item_ofs.y+=font->get_ascent();
				if (!items[i].separator)
					font->draw(ci,item_ofs+Point2(0,Math::floor((h-font_h)/2.0)),items[i].text,items[i].disabled?font_color_disabled:(i==mouse_over?font_color_hover:font_color));


				if (items[i].accel) {
					//accelerator
					String text = _get_accel_text(items[i].accel);
					item_ofs.x=size.width-style->get_margin(MARGIN_RIGHT)-font->get_string_size(text).width;
					font->draw(ci,item_ofs+Point2(0,Math::floor((h-font_h)/2.0)),text,i==mouse_over?font_color_hover:font_color_accel);

				}

				items[i]._ofs_cache=ofs.y;

				ofs.y+=h;
				
			}
			
		} break;
		case NOTIFICATION_MOUSE_ENTER: {

			grab_focus();
		} break;
		case NOTIFICATION_MOUSE_EXIT: {
		
			if (mouse_over>=0) {
				mouse_over=-1;
				update();
			}
		} break;
	}
}


void PopupMenu::add_icon_item(const Ref<Texture>& p_icon,const String& p_label,int p_ID,uint32_t p_accel) {

	Item item;
	item.icon=p_icon;
	item.text=p_label;
	item.accel=p_accel;
	item.ID=(p_ID<0)?idcount++:p_ID;
	items.push_back(item);
	update();
}
void PopupMenu::add_item(const String& p_label,int p_ID,uint32_t p_accel) {

	Item item;
	item.text=XL_MESSAGE(p_label);
	item.accel=p_accel;
	item.ID=(p_ID<0)?idcount++:p_ID;
	items.push_back(item);
	update();
}

void PopupMenu::add_submenu_item(const String& p_label, const String& p_submenu,int p_ID){

	Item item;
	item.text=XL_MESSAGE(p_label);
	item.ID=(p_ID<0)?idcount++:p_ID;
	item.submenu=p_submenu;
	items.push_back(item);
	update();
}

void PopupMenu::add_icon_check_item(const Ref<Texture>& p_icon,const String& p_label,int p_ID,uint32_t p_accel) {

	Item item;
	item.icon=p_icon;
	item.text=XL_MESSAGE(p_label);
	item.accel=p_accel;
	item.ID=(p_ID<0)?idcount++:p_ID;
	item.checkable=true;
	items.push_back(item);
	update();
}
void PopupMenu::add_check_item(const String& p_label,int p_ID,uint32_t p_accel) {

	Item item;
	item.text=XL_MESSAGE(p_label);
	item.accel=p_accel;
	item.ID=(p_ID<0)?idcount++:p_ID;
	item.checkable=true;
	items.push_back(item);
	update();
}

void PopupMenu::set_item_text(int p_idx,const String& p_text) {

	ERR_FAIL_INDEX(p_idx,items.size());
	items[p_idx].text=XL_MESSAGE(p_text);
	
	update();

}
void PopupMenu::set_item_icon(int p_idx,const Ref<Texture>& p_icon) {

	ERR_FAIL_INDEX(p_idx,items.size());
	items[p_idx].icon=p_icon;

	update();

}
void PopupMenu::set_item_checked(int p_idx,bool p_checked) {

	ERR_FAIL_INDEX(p_idx,items.size());

	items[p_idx].checked=p_checked;

	update();
}
void PopupMenu::set_item_ID(int p_idx,int p_ID) {

	ERR_FAIL_INDEX(p_idx,items.size());
	items[p_idx].ID=p_ID;	
	
	update();
}

void PopupMenu::set_item_accelerator(int p_idx,uint32_t p_accel) {

	ERR_FAIL_INDEX(p_idx,items.size());
	items[p_idx].accel=p_accel;

	update();

}


void PopupMenu::set_item_metadata(int p_idx,const Variant& p_meta) {

	ERR_FAIL_INDEX(p_idx,items.size());
	items[p_idx].metadata=p_meta;
	update();
}

void PopupMenu::set_item_disabled(int p_idx,bool p_disabled) {

	ERR_FAIL_INDEX(p_idx,items.size());
	items[p_idx].disabled=p_disabled;
	update();
}

void PopupMenu::set_item_submenu(int p_idx, const String& p_submenu) {

	ERR_FAIL_INDEX(p_idx,items.size());
	items[p_idx].submenu=p_submenu;
	update();
}

String PopupMenu::get_item_text(int p_idx) const {

	ERR_FAIL_INDEX_V(p_idx,items.size(),"");
	return items[p_idx].text;

}
Ref<Texture> PopupMenu::get_item_icon(int p_idx) const {

	ERR_FAIL_INDEX_V(p_idx,items.size(),Ref<Texture>());
	return items[p_idx].icon;
}


uint32_t PopupMenu::get_item_accelerator(int p_idx) const {

	ERR_FAIL_INDEX_V(p_idx,items.size(),0);
	return items[p_idx].accel;

}

Variant PopupMenu::get_item_metadata(int p_idx) const {

	ERR_FAIL_INDEX_V(p_idx,items.size(),Variant());
	return items[p_idx].metadata;

}

bool PopupMenu::is_item_disabled(int p_idx) const {

	ERR_FAIL_INDEX_V(p_idx,items.size(),false);
	return items[p_idx].disabled;
}

bool PopupMenu::is_item_checked(int p_idx) const {

	ERR_FAIL_INDEX_V(p_idx,items.size(),false);
	return items[p_idx].checked;
}

int PopupMenu::get_item_ID(int p_idx) const {

	ERR_FAIL_INDEX_V(p_idx,items.size(),0);
	return items[p_idx].ID;
}

int PopupMenu::get_item_index(int p_ID) const {

	for(int i=0;i<items.size();i++) {

		if (items[i].ID==p_ID)
			return i;
	}

	return -1;
}

String PopupMenu::get_item_submenu(int p_idx) const {

	ERR_FAIL_INDEX_V(p_idx,items.size(),"");
	return items[p_idx].submenu;
}

String PopupMenu::get_item_tooltip(int p_idx) const {

	ERR_FAIL_INDEX_V(p_idx,items.size(),"");
	return items[p_idx].tooltip;
}

void PopupMenu::set_item_as_separator(int p_idx, bool p_separator) {

	ERR_FAIL_INDEX(p_idx,items.size());
	items[p_idx].separator=p_separator;
	update();

}

bool PopupMenu::is_item_separator(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx,items.size(),false);
	return items[p_idx].separator;
}


void PopupMenu::set_item_as_checkable(int p_idx, bool p_checkable) {

	ERR_FAIL_INDEX(p_idx,items.size());
	items[p_idx].checkable=p_checkable;
	update();

}

void PopupMenu::set_item_tooltip(int p_idx,const String& p_tooltip) {

	ERR_FAIL_INDEX(p_idx,items.size());
	items[p_idx].tooltip=p_tooltip;
	update();
}

bool PopupMenu::is_item_checkable(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx,items.size(),false);
	return items[p_idx].checkable;
}

int PopupMenu::get_item_count() const {

	return items.size();
}

int PopupMenu::find_item_by_accelerator(uint32_t p_accel) const {

	int il=items.size();
	for(int i=0;i<il;i++) {

		if (items[i].accel==p_accel)
			return i;
	}
	return -1;
}

void PopupMenu::activate_item(int p_item) {

	ERR_FAIL_INDEX(p_item,items.size());
	ERR_FAIL_COND(items[p_item].separator);
	emit_signal("item_pressed",items[p_item].ID);

	//hide all parent PopupMenue's
	Node *next = get_parent();
	PopupMenu *pop = next->cast_to<PopupMenu>();
	while (pop) {
		pop->hide();
		next = next->get_parent();
		pop = next->cast_to<PopupMenu>();
	}
	hide();

}

void PopupMenu::remove_item(int p_idx) {

	items.remove(p_idx);
	update();
}

void PopupMenu::add_separator() {

	Item sep;
	sep.separator=true;
	sep.ID=-1;
	items.push_back(sep);
	update();
}
	
void PopupMenu::clear()  {

	items.clear();
	update();
	idcount=0;

}

Array PopupMenu::_get_items() const {

	Array items;
	for(int i=0;i<get_item_count();i++) {

		items.push_back(get_item_text(i));
		items.push_back(get_item_icon(i));
		items.push_back(is_item_checkable(i));
		items.push_back(is_item_checked(i));
		items.push_back(is_item_disabled(i));

		items.push_back(get_item_ID(i));
		items.push_back(get_item_accelerator(i));
		items.push_back(get_item_metadata(i));
		items.push_back(get_item_submenu(i));
		items.push_back(is_item_separator(i));
	}

	return items;

}
void PopupMenu::_set_items(const Array& p_items){

	ERR_FAIL_COND(p_items.size() % 10);
	clear();

	for(int i=0;i<p_items.size();i+=10) {

		String text=p_items[i+0];
		Ref<Texture> icon=p_items[i+1];
		bool checkable=p_items[i+2];
		bool checked=p_items[i+3];
		bool disabled=p_items[i+4];

		int id=p_items[i+5];
		int accel=p_items[i+6];
		Variant meta=p_items[i+7];
		String subm=p_items[i+8];
		bool sep=p_items[i+9];

		int idx=get_item_count();
		add_item(text,id);
		set_item_icon(idx,icon);
		set_item_as_checkable(idx,checkable);
		set_item_checked(idx,checked);
		set_item_disabled(idx,disabled);
		set_item_ID(idx,id);
		set_item_metadata(idx,meta);
		set_item_as_separator(idx,sep);
		set_item_accelerator(idx,accel);
		set_item_submenu(idx,subm);
	}


}


String PopupMenu::get_tooltip(const Point2& p_pos) const {


	int over=_get_mouse_over(p_pos);
	if (over<0 || over>=items.size())
		return "";
	return items[over].tooltip;
}


void PopupMenu::set_parent_rect(const Rect2& p_rect) {

	parent_rect=p_rect;
}

void PopupMenu::get_translatable_strings(List<String> *p_strings) const {

	for(int i=0;i<items.size();i++) {

		if (items[i].text!="")
			p_strings->push_back(items[i].text);
	}
}

void PopupMenu::add_autohide_area(const Rect2& p_area) {

	autohide_areas.push_back(p_area);
}

void PopupMenu::clear_autohide_areas(){

	autohide_areas.clear();
}

void PopupMenu::_bind_methods() {

	ObjectTypeDB::bind_method(_MD("_input_event"),&PopupMenu::_input_event);	
	ObjectTypeDB::bind_method(_MD("add_icon_item","texture","label","id","accel"),&PopupMenu::add_icon_item,DEFVAL(-1),DEFVAL(0));
	ObjectTypeDB::bind_method(_MD("add_item","label","id","accel"),&PopupMenu::add_item,DEFVAL(-1),DEFVAL(0));
	ObjectTypeDB::bind_method(_MD("add_icon_check_item","texture","label","id","accel"),&PopupMenu::add_icon_check_item,DEFVAL(-1),DEFVAL(0));
	ObjectTypeDB::bind_method(_MD("add_check_item","label","id","accel"),&PopupMenu::add_check_item,DEFVAL(-1),DEFVAL(0));
	ObjectTypeDB::bind_method(_MD("add_submenu_item","label","submenu","id"),&PopupMenu::add_submenu_item,DEFVAL(-1));
	ObjectTypeDB::bind_method(_MD("set_item_text","idx","text"),&PopupMenu::set_item_text);
	ObjectTypeDB::bind_method(_MD("set_item_icon","idx","icon"),&PopupMenu::set_item_icon);
	ObjectTypeDB::bind_method(_MD("set_item_accelerator","idx","accel"),&PopupMenu::set_item_accelerator);
	ObjectTypeDB::bind_method(_MD("set_item_metadata","idx","metadata"),&PopupMenu::set_item_metadata);
	ObjectTypeDB::bind_method(_MD("set_item_checked","idx"),&PopupMenu::set_item_checked);
	ObjectTypeDB::bind_method(_MD("set_item_disabled","idx","disabled"),&PopupMenu::set_item_disabled);
	ObjectTypeDB::bind_method(_MD("set_item_submenu","idx","submenu"),&PopupMenu::set_item_submenu);
	ObjectTypeDB::bind_method(_MD("set_item_as_separator","idx","enable"),&PopupMenu::set_item_as_separator);
	ObjectTypeDB::bind_method(_MD("set_item_as_checkable","idx","enable"),&PopupMenu::set_item_as_checkable);
	ObjectTypeDB::bind_method(_MD("set_item_ID","idx","id"),&PopupMenu::set_item_ID);
	ObjectTypeDB::bind_method(_MD("get_item_text","idx"),&PopupMenu::get_item_text);
	ObjectTypeDB::bind_method(_MD("get_item_icon","idx"),&PopupMenu::get_item_icon);
	ObjectTypeDB::bind_method(_MD("get_item_metadata","idx"),&PopupMenu::get_item_metadata);
	ObjectTypeDB::bind_method(_MD("get_item_accelerator","idx"),&PopupMenu::get_item_accelerator);
	ObjectTypeDB::bind_method(_MD("get_item_submenu","idx"),&PopupMenu::get_item_submenu);
	ObjectTypeDB::bind_method(_MD("is_item_separator","idx"),&PopupMenu::is_item_separator);
	ObjectTypeDB::bind_method(_MD("is_item_checkable","idx"),&PopupMenu::is_item_checkable);
	ObjectTypeDB::bind_method(_MD("is_item_checked","idx"),&PopupMenu::is_item_checked);
	ObjectTypeDB::bind_method(_MD("is_item_disabled","idx"),&PopupMenu::is_item_disabled);
	ObjectTypeDB::bind_method(_MD("get_item_ID","idx"),&PopupMenu::get_item_ID);
	ObjectTypeDB::bind_method(_MD("get_item_index","id"),&PopupMenu::get_item_index);
	ObjectTypeDB::bind_method(_MD("get_item_count"),&PopupMenu::get_item_count);
	ObjectTypeDB::bind_method(_MD("add_separator"),&PopupMenu::add_separator);	
	ObjectTypeDB::bind_method(_MD("remove_item","idx"),&PopupMenu::remove_item);
	ObjectTypeDB::bind_method(_MD("clear"),&PopupMenu::clear);

	ObjectTypeDB::bind_method(_MD("_set_items"),&PopupMenu::_set_items);
	ObjectTypeDB::bind_method(_MD("_get_items"),&PopupMenu::_get_items);

	ObjectTypeDB::bind_method(_MD("_submenu_timeout"),&PopupMenu::_submenu_timeout);

	ADD_PROPERTY( PropertyInfo(Variant::ARRAY,"items",PROPERTY_HINT_NONE,"",PROPERTY_USAGE_NOEDITOR), _SCS("_set_items"),_SCS("_get_items") );

	ADD_SIGNAL( MethodInfo("item_pressed", PropertyInfo( Variant::INT,"ID") ) );

}
	

void PopupMenu::set_invalidate_click_until_motion() {
	moved=Vector2();
	invalidated_click=true;
}

PopupMenu::PopupMenu() {

	idcount=0;
	mouse_over=-1;
	
	set_focus_mode(FOCUS_ALL);
	set_as_toplevel(true);

	submenu_timer = memnew( Timer );
	submenu_timer->set_wait_time(0.3);
	submenu_timer->set_one_shot(true);
	submenu_timer->connect("timeout",this,"_submenu_timeout");
	add_child(submenu_timer);
}


PopupMenu::~PopupMenu() {
}


