/*************************************************************************/
/*  rich_text_label.h                                                    */
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
#ifndef RICH_TEXT_LABEL_H
#define RICH_TEXT_LABEL_H


#include "scene/gui/scroll_bar.h"

class RichTextLabel : public Control {

	OBJ_TYPE( RichTextLabel, Control );
public:

	enum Align {

		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT,
		ALIGN_FILL
	};

	enum ListType {

		LIST_NUMBERS,
		LIST_LETTERS,
		LIST_DOTS
	};

	enum ItemType {

		ITEM_MAIN,
		ITEM_TEXT,
		ITEM_IMAGE,
		ITEM_NEWLINE,
		ITEM_FONT,
		ITEM_COLOR,
		ITEM_UNDERLINE,
		ITEM_ALIGN,
		ITEM_INDENT,
		ITEM_LIST,
		ITEM_META
	};

protected:

	static void _bind_methods();
private:

	struct Item {

		int index;
		Item *parent;
		ItemType type;
		List<Item*> subitems;
		List<Item*>::Element *E;

		void _clear_children() { while (subitems.size()) { memdelete(subitems.front()->get()); subitems.pop_front(); } }

		Item() { parent=NULL; E=NULL; }
		virtual ~Item() {  _clear_children(); }
	};

	struct ItemMain : public Item {

		ItemMain() { type=ITEM_MAIN; }
	};

	struct ItemText : public Item {

		String text;
		ItemText() { type=ITEM_TEXT; }
	};

	struct ItemImage : public Item {

		Ref<Texture> image;
		ItemImage() { type=ITEM_IMAGE; }
	};

	struct ItemFont : public Item {

		Ref<Font> font;
		ItemFont() { type=ITEM_FONT; }
	};

	struct ItemColor : public Item {

		Color color;
		ItemColor() { type=ITEM_COLOR; }
	};

	struct ItemUnderline : public Item {

		ItemUnderline() { type=ITEM_UNDERLINE; }
	};

	struct ItemMeta : public Item {

		Variant meta;
		ItemMeta() { type=ITEM_META; }
	};

	struct ItemAlign : public Item {

		Align align;
		ItemAlign() { type=ITEM_ALIGN; }
	};

	struct ItemIndent : public Item {

		int level;
		ItemIndent() { type=ITEM_INDENT; }
	};

	struct ItemList : public Item {

		ListType list_type;
		ItemList() { type=ITEM_LIST; }
	};

	struct ItemNewline : public Item {

		int line;
		ItemNewline() { type=ITEM_NEWLINE; }
	};

	ItemMain *main;
	Item *current;

	VScrollBar *vscroll;

	bool scroll_visible;
	bool scroll_follow;
	bool scroll_following;
	bool scroll_active;
	int scroll_w;
	bool updating_scroll;
	int current_idx;

	struct Line {

		Item *from;
		Vector<int> offset_caches;
		Vector<int> height_caches;
		Vector<int> space_caches;
		int height_cache;
		int height_accum_cache;
		int char_count;

		Line() { from=NULL; char_count=0; }
	};




	Vector<Line> lines;
	int first_invalid_line;

	int tab_size;
	bool underline_meta;

	Align default_align;

	void _invalidate_current_line();
	void _validate_line_caches();

	void _add_item(Item *p_item, bool p_enter=false);




	struct ProcessState {

		int line_width;
	};

	enum ProcessMode {

		PROCESS_CACHE,
		PROCESS_DRAW,
		PROCESS_POINTER
	};

	struct Selection {

		Item *click;
		int click_char;

		Item *from;
		int from_char;
		Item *to;
		int to_char;

		bool active;
		bool enabled;
	};

	Selection selection;


	int visible_characters;


	void _process_line(int &y, int p_width, int p_line, ProcessMode p_mode,const Ref<Font> &p_base_font,const Color &p_base_color,const Point2i& p_click_pos=Point2i(),Item **r_click_item=NULL,int *r_click_char=NULL,bool *r_outside=NULL,int p_char_count=0);
	void _find_click(const Point2i& p_click,Item **r_click_item=NULL,int *r_click_char=NULL,bool *r_outside=NULL);


	Ref<Font> _find_font(Item *p_item);
	int _find_margin(Item *p_item,const Ref<Font>& p_base_font);
	Align _find_align(Item *p_item);
	Color _find_color(Item *p_item,const Color& p_default_color);
	bool _find_underline(Item *p_item);
	bool _find_meta(Item *p_item,Variant *r_meta);

	void _update_scroll();
	void _scroll_changed(double);

	void _input_event(InputEvent p_event);
	Item *_get_next_item(Item* p_item);

	bool use_bbcode;
	String bbcode;



protected:
	void _notification(int p_what);

public:

	void add_text(const String& p_text);
	void add_image(const Ref<Texture>& p_image);
	void add_newline();
	void push_font(const Ref<Font>& p_font);
	void push_color(const Color& p_color);
	void push_underline();
	void push_align(Align p_align);
	void push_indent(int p_level);
	void push_list(ListType p_list);
	void push_meta(const Variant& p_data);
	void pop();

	void clear();

	void set_offset(int p_pixel);

	void set_meta_underline(bool p_underline);
	bool is_meta_underlined() const;

	void set_scroll_active(bool p_active);
	bool is_scroll_active() const;

	void set_scroll_follow(bool p_follow);
	bool is_scroll_following() const;

	void set_tab_size(int p_spaces);
	int get_tab_size() const;



	bool search(const String& p_string,bool p_from_selection=false);

	void scroll_to_line(int p_line);
	int get_line_count() const;

	VScrollBar *get_v_scroll() { return vscroll; }

	virtual CursorShape get_cursor_shape(const Point2& p_pos) const;

	void set_selection_enabled(bool p_enabled);
	bool is_selection_enabled() const;
	void selection_copy();


	Error parse_bbcode(const String& p_bbcode);
	Error append_bbcode(const String& p_bbcode);

	void set_use_bbcode(bool p_enable);
	bool is_using_bbcode() const;

	void set_bbcode(const String& p_bbcode);
	String get_bbcode() const;

	void set_visible_characters(int p_visible);
	int get_visible_characters() const;
	int get_total_character_count() const;

	RichTextLabel();
	~RichTextLabel();
};

VARIANT_ENUM_CAST( RichTextLabel::Align );
VARIANT_ENUM_CAST( RichTextLabel::ListType );
VARIANT_ENUM_CAST( RichTextLabel::ItemType );

#endif // RICH_TEXT_LABEL_H
