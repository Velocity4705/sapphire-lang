#include "gui.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <cmath>

// ===== Phase 1: Text Rendering =====

struct GuiFont {
    std::string name;
    int size;
    std::string style; // normal, bold, italic, bold-italic
};

struct GuiFontManager {
    std::vector<GuiFont*> fonts;
};

struct GuiTextRenderer {
    GuiFont* font = nullptr;
    float r=1,g=1,b=1,a=1;
    std::string align = "left";
    bool antialias = true;
    std::vector<std::string> lines;
    std::string last_render;
};

struct GuiTextMetrics {
    int width, height, ascent, descent, line_height;
};

void* gui_font_manager_create() { return new GuiFontManager(); }
void  gui_font_manager_destroy(void* fm) {
    auto* m = (GuiFontManager*)fm;
    for (auto* f : m->fonts) delete f;
    delete m;
}
void* gui_font_load(void* fm, const char* name, int size, const char* style) {
    auto* m = (GuiFontManager*)fm;
    auto* f = new GuiFont{name, size, style};
    m->fonts.push_back(f);
    return f;
}
int   gui_font_manager_count(void* fm) { return (int)((GuiFontManager*)fm)->fonts.size(); }
const char* gui_font_name(void* font) { return ((GuiFont*)font)->name.c_str(); }
int   gui_font_size(void* font) { return ((GuiFont*)font)->size; }
const char* gui_font_style(void* font) { return ((GuiFont*)font)->style.c_str(); }

void* gui_text_renderer_create() { return new GuiTextRenderer(); }
void  gui_text_renderer_destroy(void* tr) { delete (GuiTextRenderer*)tr; }
void  gui_text_renderer_set_font(void* tr, void* font) { ((GuiTextRenderer*)tr)->font = (GuiFont*)font; }
void  gui_text_renderer_set_color(void* tr, float r, float g, float b, float a) {
    auto* t = (GuiTextRenderer*)tr; t->r=r; t->g=g; t->b=b; t->a=a;
}
void  gui_text_renderer_set_align(void* tr, const char* align) { ((GuiTextRenderer*)tr)->align = align; }
void  gui_text_renderer_set_antialias(void* tr, int enabled) { ((GuiTextRenderer*)tr)->antialias = enabled != 0; }
const char* gui_text_renderer_render(void* tr, const char* text) {
    auto* t = (GuiTextRenderer*)tr;
    t->lines.clear();
    std::string s(text);
    std::istringstream ss(s);
    std::string line;
    while (std::getline(ss, line)) t->lines.push_back(line);
    if (t->lines.empty()) t->lines.push_back(s);
    std::string font_info = t->font ? t->font->name + ":" + std::to_string(t->font->size) : "default:12";
    t->last_render = "[rendered:" + font_info + " align=" + t->align + " lines=" + std::to_string(t->lines.size()) + "]";
    return t->last_render.c_str();
}
int   gui_text_renderer_line_count(void* tr) { return (int)((GuiTextRenderer*)tr)->lines.size(); }
const char* gui_text_renderer_line_at(void* tr, int idx) {
    auto* t = (GuiTextRenderer*)tr;
    if (idx < 0 || idx >= (int)t->lines.size()) return "";
    return t->lines[idx].c_str();
}

void* gui_text_metrics_measure(void* font, const char* text) {
    auto* f = (GuiFont*)font;
    int sz = f ? f->size : 12;
    int len = (int)std::string(text).size();
    auto* m = new GuiTextMetrics();
    m->width = len * (int)(sz * 0.6f);
    m->height = sz + 4;
    m->ascent = (int)(sz * 0.8f);
    m->descent = (int)(sz * 0.2f);
    m->line_height = sz + 6;
    return m;
}
void  gui_text_metrics_destroy(void* tm) { delete (GuiTextMetrics*)tm; }
int   gui_text_metrics_width(void* tm) { return ((GuiTextMetrics*)tm)->width; }
int   gui_text_metrics_height(void* tm) { return ((GuiTextMetrics*)tm)->height; }
int   gui_text_metrics_ascent(void* tm) { return ((GuiTextMetrics*)tm)->ascent; }
int   gui_text_metrics_descent(void* tm) { return ((GuiTextMetrics*)tm)->descent; }
int   gui_text_metrics_line_height(void* tm) { return ((GuiTextMetrics*)tm)->line_height; }

// ===== Phase 2: Mouse Support =====

struct MouseEvent {
    std::string type;
    int x, y, button;
};

struct GuiMouseHandler {
    std::vector<MouseEvent> events;
    int cursor_x = 0, cursor_y = 0;
    bool buttons[5] = {};
};

void* gui_mouse_handler_create() { return new GuiMouseHandler(); }
void  gui_mouse_handler_destroy(void* mh) { delete (GuiMouseHandler*)mh; }
void  gui_mouse_handler_click(void* mh, int x, int y, int button) {
    auto* h = (GuiMouseHandler*)mh;
    h->events.push_back({"click", x, y, button});
    h->cursor_x = x; h->cursor_y = y;
}
void  gui_mouse_handler_move(void* mh, int x, int y) {
    auto* h = (GuiMouseHandler*)mh;
    h->events.push_back({"move", x, y, 0});
    h->cursor_x = x; h->cursor_y = y;
}
void  gui_mouse_handler_press(void* mh, int x, int y, int button) {
    auto* h = (GuiMouseHandler*)mh;
    h->events.push_back({"press", x, y, button});
    h->cursor_x = x; h->cursor_y = y;
    if (button >= 0 && button < 5) h->buttons[button] = true;
}
void  gui_mouse_handler_release(void* mh, int x, int y, int button) {
    auto* h = (GuiMouseHandler*)mh;
    h->events.push_back({"release", x, y, button});
    h->cursor_x = x; h->cursor_y = y;
    if (button >= 0 && button < 5) h->buttons[button] = false;
}
void  gui_mouse_handler_scroll(void* mh, int x, int y, int delta) {
    auto* h = (GuiMouseHandler*)mh;
    h->events.push_back({"scroll", x, y, delta});
}
int   gui_mouse_handler_event_count(void* mh) { return (int)((GuiMouseHandler*)mh)->events.size(); }
const char* gui_mouse_handler_event_type(void* mh, int idx) {
    auto* h = (GuiMouseHandler*)mh;
    static std::string s;
    if (idx < 0 || idx >= (int)h->events.size()) return "";
    s = h->events[idx].type; return s.c_str();
}
int   gui_mouse_handler_event_x(void* mh, int idx) {
    auto* h = (GuiMouseHandler*)mh;
    if (idx < 0 || idx >= (int)h->events.size()) return 0;
    return h->events[idx].x;
}
int   gui_mouse_handler_event_y(void* mh, int idx) {
    auto* h = (GuiMouseHandler*)mh;
    if (idx < 0 || idx >= (int)h->events.size()) return 0;
    return h->events[idx].y;
}
int   gui_mouse_handler_event_button(void* mh, int idx) {
    auto* h = (GuiMouseHandler*)mh;
    if (idx < 0 || idx >= (int)h->events.size()) return 0;
    return h->events[idx].button;
}
int   gui_mouse_handler_cursor_x(void* mh) { return ((GuiMouseHandler*)mh)->cursor_x; }
int   gui_mouse_handler_cursor_y(void* mh) { return ((GuiMouseHandler*)mh)->cursor_y; }
int   gui_mouse_handler_is_button_down(void* mh, int button) {
    auto* h = (GuiMouseHandler*)mh;
    if (button < 0 || button >= 5) return 0;
    return h->buttons[button] ? 1 : 0;
}

struct GuiDragDrop {
    bool dragging = false;
    int start_x = 0, start_y = 0;
    std::string data, mime;
};

void* gui_drag_drop_create() { return new GuiDragDrop(); }
void  gui_drag_drop_destroy(void* dd) { delete (GuiDragDrop*)dd; }
void  gui_drag_drop_start(void* dd, int x, int y, const char* data, const char* mime) {
    auto* d = (GuiDragDrop*)dd;
    d->dragging = true; d->start_x = x; d->start_y = y;
    d->data = data; d->mime = mime;
}
void  gui_drag_drop_move(void* dd, int x, int y) { (void)dd; (void)x; (void)y; }
int   gui_drag_drop_drop(void* dd, int x, int y) {
    auto* d = (GuiDragDrop*)dd; (void)x; (void)y;
    d->dragging = false; return 1;
}
int   gui_drag_drop_is_dragging(void* dd) { return ((GuiDragDrop*)dd)->dragging ? 1 : 0; }
const char* gui_drag_drop_data(void* dd) { return ((GuiDragDrop*)dd)->data.c_str(); }
const char* gui_drag_drop_mime(void* dd) { return ((GuiDragDrop*)dd)->mime.c_str(); }
int   gui_drag_drop_start_x(void* dd) { return ((GuiDragDrop*)dd)->start_x; }
int   gui_drag_drop_start_y(void* dd) { return ((GuiDragDrop*)dd)->start_y; }

struct ContextMenuItem { std::string label, action; bool separator = false; };
struct GuiContextMenu {
    std::vector<ContextMenuItem> items;
    bool visible = false;
    int x = 0, y = 0;
};

void* gui_context_menu_create() { return new GuiContextMenu(); }
void  gui_context_menu_destroy(void* cm) { delete (GuiContextMenu*)cm; }
void  gui_context_menu_add_item(void* cm, const char* label, const char* action) {
    ((GuiContextMenu*)cm)->items.push_back({label, action, false});
}
void  gui_context_menu_add_separator(void* cm) {
    ((GuiContextMenu*)cm)->items.push_back({"---", "", true});
}
void  gui_context_menu_show(void* cm, int x, int y) {
    auto* m = (GuiContextMenu*)cm; m->visible = true; m->x = x; m->y = y;
}
void  gui_context_menu_hide(void* cm) { ((GuiContextMenu*)cm)->visible = false; }
int   gui_context_menu_item_count(void* cm) { return (int)((GuiContextMenu*)cm)->items.size(); }
const char* gui_context_menu_item_label(void* cm, int idx) {
    auto* m = (GuiContextMenu*)cm;
    static std::string s;
    if (idx < 0 || idx >= (int)m->items.size()) return "";
    s = m->items[idx].label; return s.c_str();
}
const char* gui_context_menu_item_action(void* cm, int idx) {
    auto* m = (GuiContextMenu*)cm;
    static std::string s;
    if (idx < 0 || idx >= (int)m->items.size()) return "";
    s = m->items[idx].action; return s.c_str();
}
int   gui_context_menu_is_visible(void* cm) { return ((GuiContextMenu*)cm)->visible ? 1 : 0; }
int   gui_context_menu_x(void* cm) { return ((GuiContextMenu*)cm)->x; }
int   gui_context_menu_y(void* cm) { return ((GuiContextMenu*)cm)->y; }

// ===== Phase 3: Advanced Widgets =====

struct GuiTextInput {
    std::string text, placeholder;
    int max_length = 0;
    bool password = false, multiline = false, focused = false;
    int cursor_pos = 0;
};

void* gui_text_input_create() { return new GuiTextInput(); }
void  gui_text_input_destroy(void* ti) { delete (GuiTextInput*)ti; }
void  gui_text_input_set_text(void* ti, const char* text) {
    auto* t = (GuiTextInput*)ti; t->text = text;
    t->cursor_pos = (int)t->text.size();
}
void  gui_text_input_set_placeholder(void* ti, const char* ph) { ((GuiTextInput*)ti)->placeholder = ph; }
void  gui_text_input_set_max_length(void* ti, int max) { ((GuiTextInput*)ti)->max_length = max; }
void  gui_text_input_set_password(void* ti, int enabled) { ((GuiTextInput*)ti)->password = enabled != 0; }
void  gui_text_input_set_multiline(void* ti, int enabled) { ((GuiTextInput*)ti)->multiline = enabled != 0; }
void  gui_text_input_insert(void* ti, const char* text) {
    auto* t = (GuiTextInput*)ti;
    t->text.insert(t->cursor_pos, text);
    t->cursor_pos += (int)std::string(text).size();
}
void  gui_text_input_clear(void* ti) { auto* t = (GuiTextInput*)ti; t->text = ""; t->cursor_pos = 0; }
const char* gui_text_input_text(void* ti) { return ((GuiTextInput*)ti)->text.c_str(); }
int   gui_text_input_length(void* ti) { return (int)((GuiTextInput*)ti)->text.size(); }
int   gui_text_input_cursor_pos(void* ti) { return ((GuiTextInput*)ti)->cursor_pos; }
int   gui_text_input_is_focused(void* ti) { return ((GuiTextInput*)ti)->focused ? 1 : 0; }
void  gui_text_input_focus(void* ti) { ((GuiTextInput*)ti)->focused = true; }
void  gui_text_input_blur(void* ti) { ((GuiTextInput*)ti)->focused = false; }

struct DropdownOption { std::string label, value; };
struct GuiDropdown {
    std::vector<DropdownOption> options;
    int selected = -1;
    std::string placeholder;
    bool open = false;
};

void* gui_dropdown_create() { return new GuiDropdown(); }
void  gui_dropdown_destroy(void* dd) { delete (GuiDropdown*)dd; }
void  gui_dropdown_add_option(void* dd, const char* label, const char* value) {
    ((GuiDropdown*)dd)->options.push_back({label, value});
}
void  gui_dropdown_set_selected(void* dd, int idx) { ((GuiDropdown*)dd)->selected = idx; }
void  gui_dropdown_set_placeholder(void* dd, const char* ph) { ((GuiDropdown*)dd)->placeholder = ph; }
void  gui_dropdown_open(void* dd) { ((GuiDropdown*)dd)->open = true; }
void  gui_dropdown_close(void* dd) { ((GuiDropdown*)dd)->open = false; }
int   gui_dropdown_option_count(void* dd) { return (int)((GuiDropdown*)dd)->options.size(); }
const char* gui_dropdown_option_label(void* dd, int idx) {
    auto* d = (GuiDropdown*)dd;
    static std::string s;
    if (idx < 0 || idx >= (int)d->options.size()) return "";
    s = d->options[idx].label; return s.c_str();
}
const char* gui_dropdown_option_value(void* dd, int idx) {
    auto* d = (GuiDropdown*)dd;
    static std::string s;
    if (idx < 0 || idx >= (int)d->options.size()) return "";
    s = d->options[idx].value; return s.c_str();
}
int   gui_dropdown_selected_index(void* dd) { return ((GuiDropdown*)dd)->selected; }
const char* gui_dropdown_selected_value(void* dd) {
    auto* d = (GuiDropdown*)dd;
    static std::string s;
    if (d->selected < 0 || d->selected >= (int)d->options.size()) return "";
    s = d->options[d->selected].value; return s.c_str();
}
int   gui_dropdown_is_open(void* dd) { return ((GuiDropdown*)dd)->open ? 1 : 0; }

struct TableCell { std::string value; };
struct TableColumn { std::string header; int width; };
struct GuiTable {
    std::vector<TableColumn> columns;
    std::vector<std::vector<TableCell>> rows;
    int selected_row = -1;
    std::string csv_cache;
};

void* gui_table_create() { return new GuiTable(); }
void  gui_table_destroy(void* tbl) { delete (GuiTable*)tbl; }
void  gui_table_add_column(void* tbl, const char* header, int width) {
    ((GuiTable*)tbl)->columns.push_back({header, width});
}
void  gui_table_add_row(void* tbl) {
    auto* t = (GuiTable*)tbl;
    t->rows.push_back(std::vector<TableCell>(t->columns.size()));
}
void  gui_table_set_cell(void* tbl, int row, int col, const char* value) {
    auto* t = (GuiTable*)tbl;
    if (row < 0 || row >= (int)t->rows.size()) return;
    if (col < 0 || col >= (int)t->columns.size()) return;
    t->rows[row][col].value = value;
}
const char* gui_table_get_cell(void* tbl, int row, int col) {
    auto* t = (GuiTable*)tbl;
    static std::string s;
    if (row < 0 || row >= (int)t->rows.size()) return "";
    if (col < 0 || col >= (int)t->columns.size()) return "";
    s = t->rows[row][col].value; return s.c_str();
}
int   gui_table_row_count(void* tbl) { return (int)((GuiTable*)tbl)->rows.size(); }
int   gui_table_col_count(void* tbl) { return (int)((GuiTable*)tbl)->columns.size(); }
const char* gui_table_column_header(void* tbl, int col) {
    auto* t = (GuiTable*)tbl;
    static std::string s;
    if (col < 0 || col >= (int)t->columns.size()) return "";
    s = t->columns[col].header; return s.c_str();
}
int   gui_table_column_width(void* tbl, int col) {
    auto* t = (GuiTable*)tbl;
    if (col < 0 || col >= (int)t->columns.size()) return 0;
    return t->columns[col].width;
}
void  gui_table_set_selected_row(void* tbl, int row) { ((GuiTable*)tbl)->selected_row = row; }
int   gui_table_selected_row(void* tbl) { return ((GuiTable*)tbl)->selected_row; }
void  gui_table_sort_by_column(void* tbl, int col, int ascending) {
    auto* t = (GuiTable*)tbl;
    if (col < 0 || col >= (int)t->columns.size()) return;
    std::sort(t->rows.begin(), t->rows.end(), [&](const auto& a, const auto& b) {
        std::string va = col < (int)a.size() ? a[col].value : "";
        std::string vb = col < (int)b.size() ? b[col].value : "";
        return ascending ? va < vb : va > vb;
    });
}
const char* gui_table_to_csv(void* tbl) {
    auto* t = (GuiTable*)tbl;
    std::ostringstream ss;
    for (int i = 0; i < (int)t->columns.size(); i++) {
        if (i) ss << ",";
        ss << t->columns[i].header;
    }
    ss << "\n";
    for (auto& row : t->rows) {
        for (int i = 0; i < (int)row.size(); i++) {
            if (i) ss << ",";
            ss << row[i].value;
        }
        ss << "\n";
    }
    t->csv_cache = ss.str();
    return t->csv_cache.c_str();
}

struct TreeNode { int id, parent_id; std::string label, data; bool expanded = false; };
struct GuiTreeView {
    std::vector<TreeNode> nodes;
    int selected = -1;
    int next_id = 0;
};

void* gui_tree_view_create() { return new GuiTreeView(); }
void  gui_tree_view_destroy(void* tv) { delete (GuiTreeView*)tv; }
int   gui_tree_view_add_node(void* tv, int parent_id, const char* label, const char* data) {
    auto* t = (GuiTreeView*)tv;
    int id = t->next_id++;
    t->nodes.push_back({id, parent_id, label, data, false});
    return id;
}
void  gui_tree_view_expand(void* tv, int node_id) {
    for (auto& n : ((GuiTreeView*)tv)->nodes) if (n.id == node_id) n.expanded = true;
}
void  gui_tree_view_collapse(void* tv, int node_id) {
    for (auto& n : ((GuiTreeView*)tv)->nodes) if (n.id == node_id) n.expanded = false;
}
void  gui_tree_view_select(void* tv, int node_id) { ((GuiTreeView*)tv)->selected = node_id; }
int   gui_tree_view_node_count(void* tv) { return (int)((GuiTreeView*)tv)->nodes.size(); }
const char* gui_tree_view_node_label(void* tv, int node_id) {
    static std::string s;
    for (auto& n : ((GuiTreeView*)tv)->nodes) if (n.id == node_id) { s = n.label; return s.c_str(); }
    return "";
}
const char* gui_tree_view_node_data(void* tv, int node_id) {
    static std::string s;
    for (auto& n : ((GuiTreeView*)tv)->nodes) if (n.id == node_id) { s = n.data; return s.c_str(); }
    return "";
}
int   gui_tree_view_node_parent(void* tv, int node_id) {
    for (auto& n : ((GuiTreeView*)tv)->nodes) if (n.id == node_id) return n.parent_id;
    return -1;
}
int   gui_tree_view_node_is_expanded(void* tv, int node_id) {
    for (auto& n : ((GuiTreeView*)tv)->nodes) if (n.id == node_id) return n.expanded ? 1 : 0;
    return 0;
}
int   gui_tree_view_selected_node(void* tv) { return ((GuiTreeView*)tv)->selected; }
int   gui_tree_view_child_count(void* tv, int node_id) {
    int count = 0;
    for (auto& n : ((GuiTreeView*)tv)->nodes) if (n.parent_id == node_id) count++;
    return count;
}

struct TabItem { std::string label, content_id; };
struct GuiTabs {
    std::vector<TabItem> tabs;
    int active = 0;
};

void* gui_tabs_create() { return new GuiTabs(); }
void  gui_tabs_destroy(void* tabs) { delete (GuiTabs*)tabs; }
void  gui_tabs_add_tab(void* tabs, const char* label, const char* content_id) {
    ((GuiTabs*)tabs)->tabs.push_back({label, content_id});
}
void  gui_tabs_set_active(void* tabs, int idx) { ((GuiTabs*)tabs)->active = idx; }
void  gui_tabs_remove_tab(void* tabs, int idx) {
    auto* t = (GuiTabs*)tabs;
    if (idx >= 0 && idx < (int)t->tabs.size()) t->tabs.erase(t->tabs.begin() + idx);
}
int   gui_tabs_count(void* tabs) { return (int)((GuiTabs*)tabs)->tabs.size(); }
const char* gui_tabs_label(void* tabs, int idx) {
    auto* t = (GuiTabs*)tabs;
    static std::string s;
    if (idx < 0 || idx >= (int)t->tabs.size()) return "";
    s = t->tabs[idx].label; return s.c_str();
}
const char* gui_tabs_content_id(void* tabs, int idx) {
    auto* t = (GuiTabs*)tabs;
    static std::string s;
    if (idx < 0 || idx >= (int)t->tabs.size()) return "";
    s = t->tabs[idx].content_id; return s.c_str();
}
int   gui_tabs_active_index(void* tabs) { return ((GuiTabs*)tabs)->active; }
const char* gui_tabs_active_label(void* tabs) {
    auto* t = (GuiTabs*)tabs;
    static std::string s;
    if (t->active < 0 || t->active >= (int)t->tabs.size()) return "";
    s = t->tabs[t->active].label; return s.c_str();
}

// ===== Phase 4: Layout Management =====

struct FlexItem { std::string id; int flex, min_size, max_size; };
struct GuiFlexLayout {
    std::string direction; // row, column
    std::vector<FlexItem> items;
    int gap = 0, padding = 0;
    bool wrap = false;
    std::string align = "start", justify = "start";
    std::string compute_cache;
};

void* gui_flex_layout_create(const char* direction) {
    auto* fl = new GuiFlexLayout(); fl->direction = direction; return fl;
}
void  gui_flex_layout_destroy(void* fl) { delete (GuiFlexLayout*)fl; }
void  gui_flex_layout_add_item(void* fl, const char* id, int flex, int min_size, int max_size) {
    ((GuiFlexLayout*)fl)->items.push_back({id, flex, min_size, max_size});
}
void  gui_flex_layout_set_gap(void* fl, int gap) { ((GuiFlexLayout*)fl)->gap = gap; }
void  gui_flex_layout_set_padding(void* fl, int padding) { ((GuiFlexLayout*)fl)->padding = padding; }
void  gui_flex_layout_set_wrap(void* fl, int wrap) { ((GuiFlexLayout*)fl)->wrap = wrap != 0; }
void  gui_flex_layout_set_align(void* fl, const char* align) { ((GuiFlexLayout*)fl)->align = align; }
void  gui_flex_layout_set_justify(void* fl, const char* justify) { ((GuiFlexLayout*)fl)->justify = justify; }
int   gui_flex_layout_item_count(void* fl) { return (int)((GuiFlexLayout*)fl)->items.size(); }
const char* gui_flex_layout_item_id(void* fl, int idx) {
    auto* f = (GuiFlexLayout*)fl;
    static std::string s;
    if (idx < 0 || idx >= (int)f->items.size()) return "";
    s = f->items[idx].id; return s.c_str();
}
int   gui_flex_layout_item_flex(void* fl, int idx) {
    auto* f = (GuiFlexLayout*)fl;
    if (idx < 0 || idx >= (int)f->items.size()) return 0;
    return f->items[idx].flex;
}
const char* gui_flex_layout_compute(void* fl, int total_width, int total_height) {
    auto* f = (GuiFlexLayout*)fl;
    bool is_row = f->direction == "row";
    int total = is_row ? total_width : total_height;
    int avail = total - 2 * f->padding - (int)f->items.size() > 1 ? f->gap * ((int)f->items.size() - 1) : 0;
    int total_flex = 0;
    for (auto& item : f->items) total_flex += item.flex > 0 ? item.flex : 1;
    std::ostringstream ss;
    ss << "{\"direction\":\"" << f->direction << "\",\"items\":[";
    int pos = f->padding;
    for (int i = 0; i < (int)f->items.size(); i++) {
        auto& item = f->items[i];
        int flex = item.flex > 0 ? item.flex : 1;
        int size = (avail * flex) / (total_flex > 0 ? total_flex : 1);
        if (item.min_size > 0 && size < item.min_size) size = item.min_size;
        if (item.max_size > 0 && size > item.max_size) size = item.max_size;
        if (i) ss << ",";
        ss << "{\"id\":\"" << item.id << "\",\"pos\":" << pos << ",\"size\":" << size << "}";
        pos += size + f->gap;
    }
    ss << "]}";
    f->compute_cache = ss.str();
    return f->compute_cache.c_str();
}
const char* gui_flex_layout_direction(void* fl) { return ((GuiFlexLayout*)fl)->direction.c_str(); }

struct GridItem { std::string id; int col, row, col_span, row_span; };
struct GuiGridLayout {
    int cols, rows;
    std::vector<GridItem> items;
    std::vector<int> col_widths, row_heights;
    int gap = 0;
    std::string compute_cache;
};

void* gui_grid_layout_create(int cols, int rows) {
    auto* gl = new GuiGridLayout();
    gl->cols = cols; gl->rows = rows;
    gl->col_widths.resize(cols, 0);
    gl->row_heights.resize(rows, 0);
    return gl;
}
void  gui_grid_layout_destroy(void* gl) { delete (GuiGridLayout*)gl; }
void  gui_grid_layout_place(void* gl, const char* id, int col, int row, int col_span, int row_span) {
    ((GuiGridLayout*)gl)->items.push_back({id, col, row, col_span, row_span});
}
void  gui_grid_layout_set_col_width(void* gl, int col, int width) {
    auto* g = (GuiGridLayout*)gl;
    if (col >= 0 && col < (int)g->col_widths.size()) g->col_widths[col] = width;
}
void  gui_grid_layout_set_row_height(void* gl, int row, int height) {
    auto* g = (GuiGridLayout*)gl;
    if (row >= 0 && row < (int)g->row_heights.size()) g->row_heights[row] = height;
}
void  gui_grid_layout_set_gap(void* gl, int gap) { ((GuiGridLayout*)gl)->gap = gap; }
int   gui_grid_layout_cols(void* gl) { return ((GuiGridLayout*)gl)->cols; }
int   gui_grid_layout_rows(void* gl) { return ((GuiGridLayout*)gl)->rows; }
int   gui_grid_layout_item_count(void* gl) { return (int)((GuiGridLayout*)gl)->items.size(); }
const char* gui_grid_layout_item_id(void* gl, int idx) {
    auto* g = (GuiGridLayout*)gl;
    static std::string s;
    if (idx < 0 || idx >= (int)g->items.size()) return "";
    s = g->items[idx].id; return s.c_str();
}
const char* gui_grid_layout_compute(void* gl, int total_width, int total_height) {
    auto* g = (GuiGridLayout*)gl;
    int col_w = g->cols > 0 ? (total_width - g->gap * (g->cols - 1)) / g->cols : 0;
    int row_h = g->rows > 0 ? (total_height - g->gap * (g->rows - 1)) / g->rows : 0;
    std::ostringstream ss;
    ss << "{\"cols\":" << g->cols << ",\"rows\":" << g->rows << ",\"items\":[";
    for (int i = 0; i < (int)g->items.size(); i++) {
        auto& item = g->items[i];
        int cw = g->col_widths[item.col] > 0 ? g->col_widths[item.col] : col_w;
        int rh = g->row_heights[item.row] > 0 ? g->row_heights[item.row] : row_h;
        int x = item.col * (col_w + g->gap);
        int y = item.row * (row_h + g->gap);
        int w = cw * item.col_span + g->gap * (item.col_span - 1);
        int h = rh * item.row_span + g->gap * (item.row_span - 1);
        if (i) ss << ",";
        ss << "{\"id\":\"" << item.id << "\",\"x\":" << x << ",\"y\":" << y
           << ",\"w\":" << w << ",\"h\":" << h << "}";
    }
    ss << "]}";
    g->compute_cache = ss.str();
    return g->compute_cache.c_str();
}

struct AnchorItem {
    std::string id;
    int x, y, width, height;
    std::map<std::string, int> anchors;       // edge -> offset
    std::map<std::string, std::string> relatives; // edge -> ref_id:edge
};
struct GuiAnchorLayout {
    std::vector<AnchorItem> items;
    std::string compute_cache;
};

void* gui_anchor_layout_create() { return new GuiAnchorLayout(); }
void  gui_anchor_layout_destroy(void* al) { delete (GuiAnchorLayout*)al; }
void  gui_anchor_layout_add_item(void* al, const char* id, int x, int y, int width, int height) {
    ((GuiAnchorLayout*)al)->items.push_back({id, x, y, width, height, {}, {}});
}
void  gui_anchor_layout_set_anchor(void* al, const char* id, const char* edge, int offset) {
    for (auto& item : ((GuiAnchorLayout*)al)->items)
        if (item.id == id) { item.anchors[edge] = offset; return; }
}
void  gui_anchor_layout_set_relative(void* al, const char* id, const char* ref_id, const char* edge) {
    for (auto& item : ((GuiAnchorLayout*)al)->items)
        if (item.id == id) { item.relatives[edge] = std::string(ref_id) + ":" + edge; return; }
}
int   gui_anchor_layout_item_count(void* al) { return (int)((GuiAnchorLayout*)al)->items.size(); }
const char* gui_anchor_layout_item_id(void* al, int idx) {
    auto* a = (GuiAnchorLayout*)al;
    static std::string s;
    if (idx < 0 || idx >= (int)a->items.size()) return "";
    s = a->items[idx].id; return s.c_str();
}
const char* gui_anchor_layout_compute(void* al, int total_width, int total_height) {
    auto* a = (GuiAnchorLayout*)al;
    std::ostringstream ss;
    ss << "{\"items\":[";
    for (int i = 0; i < (int)a->items.size(); i++) {
        auto& item = a->items[i];
        int x = item.x, y = item.y, w = item.width, h = item.height;
        // Apply anchors
        if (item.anchors.count("right")) w = total_width - item.anchors.at("right") - x;
        if (item.anchors.count("bottom")) h = total_height - item.anchors.at("bottom") - y;
        if (item.anchors.count("left")) x = item.anchors.at("left");
        if (item.anchors.count("top")) y = item.anchors.at("top");
        if (i) ss << ",";
        ss << "{\"id\":\"" << item.id << "\",\"x\":" << x << ",\"y\":" << y
           << ",\"w\":" << w << ",\"h\":" << h << "}";
    }
    ss << "]}";
    a->compute_cache = ss.str();
    return a->compute_cache.c_str();
}

// ===== Window System Implementation =====
// SDL2-backed with headless fallback (no display needed for tests)

#ifdef __has_include
#  if __has_include(<SDL2/SDL.h>)
#    include <SDL2/SDL.h>
#    include <SDL2/SDL_ttf.h>
#    define HAVE_SDL2 1
#  endif
#endif

#include <chrono>
#include <unordered_set>
#include <cmath>

static bool s_sdl_ok = false;
static bool s_headless = false;

int gui_init() {
#ifdef HAVE_SDL2
    // Try to init with video; fall back to headless if no display
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == 0) {
        TTF_Init();
        s_sdl_ok = true;
        s_headless = false;
        return 1;
    }
    // No display — headless mode
#endif
    s_headless = true;
    s_sdl_ok = false;
    return 1; // still "succeeds" — headless is valid
}

void gui_quit() {
#ifdef HAVE_SDL2
    if (s_sdl_ok) { TTF_Quit(); SDL_Quit(); s_sdl_ok = false; }
#endif
}

int gui_has_display() { return s_headless ? 0 : 1; }

// ---- Window struct ----
struct GuiWindow {
    std::string title;
    int width, height;
    bool open = true, fullscreen = false, resizable = true;
    // SDL handles (null in headless)
#ifdef HAVE_SDL2
    SDL_Window*   sdl_win  = nullptr;
    SDL_Renderer* sdl_ren  = nullptr;
    int draw_r=255, draw_g=255, draw_b=255, draw_a=255;
#endif
    // Input state
    std::unordered_set<int> keys_down, keys_pressed;
    int mouse_x=0, mouse_y=0;
    int mouse_px=0, mouse_py=0;  // previous frame position for delta
    int mouse_dx=0, mouse_dy=0;  // delta since last poll
    bool mouse_btn[4] = {};
    int scroll_y = 0;            // accumulated scroll this frame
    // Timing
    std::chrono::steady_clock::time_point last_frame;
    double delta = 0.016;
    // Headless draw log (for testing)
    std::vector<std::string> draw_log;
};

void* gui_window_create(const char* title, int width, int height) {
    auto* w = new GuiWindow();
    w->title = title; w->width = width; w->height = height;
    w->last_frame = std::chrono::steady_clock::now();
#ifdef HAVE_SDL2
    if (s_sdl_ok) {
        w->sdl_win = SDL_CreateWindow(title,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
        if (w->sdl_win)
            w->sdl_ren = SDL_CreateRenderer(w->sdl_win, -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    }
#endif
    return w;
}

void gui_window_destroy(void* win) {
    auto* w = (GuiWindow*)win;
#ifdef HAVE_SDL2
    if (w->sdl_ren) SDL_DestroyRenderer(w->sdl_ren);
    if (w->sdl_win) SDL_DestroyWindow(w->sdl_win);
#endif
    delete w;
}

void gui_window_show(void* win) {
#ifdef HAVE_SDL2
    auto* w = (GuiWindow*)win;
    if (w->sdl_win) SDL_ShowWindow(w->sdl_win);
#endif
}
void gui_window_hide(void* win) {
#ifdef HAVE_SDL2
    auto* w = (GuiWindow*)win;
    if (w->sdl_win) SDL_HideWindow(w->sdl_win);
#endif
}
void gui_window_set_title(void* win, const char* title) {
    auto* w = (GuiWindow*)win; w->title = title;
#ifdef HAVE_SDL2
    if (w->sdl_win) SDL_SetWindowTitle(w->sdl_win, title);
#endif
}
const char* gui_window_title(void* win) { return ((GuiWindow*)win)->title.c_str(); }
int gui_window_width(void* win)  { return ((GuiWindow*)win)->width; }
int gui_window_height(void* win) { return ((GuiWindow*)win)->height; }
int gui_window_is_open(void* win) { return ((GuiWindow*)win)->open ? 1 : 0; }
void gui_window_close(void* win) { ((GuiWindow*)win)->open = false; }
void gui_window_resize(void* win, int w, int h) {
    auto* win_ = (GuiWindow*)win; win_->width = w; win_->height = h;
#ifdef HAVE_SDL2
    if (win_->sdl_win) SDL_SetWindowSize(win_->sdl_win, w, h);
#endif
}
void gui_window_set_resizable(void* win, int enabled) {
    ((GuiWindow*)win)->resizable = enabled != 0;
#ifdef HAVE_SDL2
    auto* w = (GuiWindow*)win;
    if (w->sdl_win) SDL_SetWindowResizable(w->sdl_win, enabled ? SDL_TRUE : SDL_FALSE);
#endif
}
void gui_window_set_fullscreen(void* win, int enabled) {
    auto* w = (GuiWindow*)win; w->fullscreen = enabled != 0;
#ifdef HAVE_SDL2
    if (w->sdl_win) SDL_SetWindowFullscreen(w->sdl_win, enabled ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
#endif
}
int gui_window_is_fullscreen(void* win) { return ((GuiWindow*)win)->fullscreen ? 1 : 0; }

int gui_window_poll(void* win) {
    auto* w = (GuiWindow*)win;
    w->keys_pressed.clear();
    w->scroll_y = 0;
    w->mouse_dx = 0; w->mouse_dy = 0;
    if (!w->open) return 0;
#ifdef HAVE_SDL2
    if (s_sdl_ok) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { w->open = false; return 0; }
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) { w->open = false; return 0; }
            if (e.type == SDL_KEYDOWN && !e.key.repeat) {
                w->keys_down.insert(e.key.keysym.scancode);
                w->keys_pressed.insert(e.key.keysym.scancode);
            }
            if (e.type == SDL_KEYUP) w->keys_down.erase(e.key.keysym.scancode);
            if (e.type == SDL_MOUSEMOTION) {
                w->mouse_dx += e.motion.xrel;
                w->mouse_dy += e.motion.yrel;
                w->mouse_x = e.motion.x;
                w->mouse_y = e.motion.y;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button <= 3) w->mouse_btn[e.button.button] = true;
            if (e.type == SDL_MOUSEBUTTONUP   && e.button.button <= 3) w->mouse_btn[e.button.button] = false;
            if (e.type == SDL_MOUSEWHEEL) w->scroll_y += e.wheel.y;
        }
        return w->open ? 1 : 0;
    }
#endif
    return w->open ? 1 : 0;
}

int gui_key_down(void* win, int sc) { return ((GuiWindow*)win)->keys_down.count(sc) ? 1 : 0; }
int gui_key_pressed(void* win, int sc) { return ((GuiWindow*)win)->keys_pressed.count(sc) ? 1 : 0; }

int gui_key(const char* name) {
#ifdef HAVE_SDL2
    SDL_Scancode sc = SDL_GetScancodeFromName(name);
    if (sc != SDL_SCANCODE_UNKNOWN) return (int)sc;
    // aliases
    std::string n(name);
    if (n=="up")     return SDL_SCANCODE_UP;
    if (n=="down")   return SDL_SCANCODE_DOWN;
    if (n=="left")   return SDL_SCANCODE_LEFT;
    if (n=="right")  return SDL_SCANCODE_RIGHT;
    if (n=="space")  return SDL_SCANCODE_SPACE;
    if (n=="escape") return SDL_SCANCODE_ESCAPE;
    if (n=="enter")  return SDL_SCANCODE_RETURN;
    if (n=="w")      return SDL_SCANCODE_W;
    if (n=="a")      return SDL_SCANCODE_A;
    if (n=="s")      return SDL_SCANCODE_S;
    if (n=="d")      return SDL_SCANCODE_D;
#endif
    return 0;
}

int gui_mouse_x(void* win) { return ((GuiWindow*)win)->mouse_x; }
int gui_mouse_y(void* win) { return ((GuiWindow*)win)->mouse_y; }
int gui_mouse_button(void* win, int btn) {
    auto* w = (GuiWindow*)win;
    if (btn < 1 || btn > 3) return 0;
    return w->mouse_btn[btn] ? 1 : 0;
}

int gui_scroll_y(void* win) { return ((GuiWindow*)win)->scroll_y; }
int gui_mouse_dx(void* win) { return ((GuiWindow*)win)->mouse_dx; }
int gui_mouse_dy(void* win) { return ((GuiWindow*)win)->mouse_dy; }

// ---- Drawing ----
void gui_clear(void* win, int r, int g, int b) {
    auto* w = (GuiWindow*)win;
    w->draw_log.push_back("clear(" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + ")");
#ifdef HAVE_SDL2
    if (w->sdl_ren) { SDL_SetRenderDrawColor(w->sdl_ren,r,g,b,255); SDL_RenderClear(w->sdl_ren); }
#endif
}

void gui_present(void* win) {
    auto* w = (GuiWindow*)win;
    auto now = std::chrono::steady_clock::now();
    w->delta = std::chrono::duration<double>(now - w->last_frame).count();
    w->last_frame = now;
    w->draw_log.push_back("present");
#ifdef HAVE_SDL2
    if (w->sdl_ren) SDL_RenderPresent(w->sdl_ren);
#endif
}

void gui_set_color(void* win, int r, int g, int b, int a) {
    auto* w = (GuiWindow*)win;
#ifdef HAVE_SDL2
    w->draw_r=r; w->draw_g=g; w->draw_b=b; w->draw_a=a;
    if (w->sdl_ren) SDL_SetRenderDrawColor(w->sdl_ren,r,g,b,a);
#else
    (void)w; (void)r; (void)g; (void)b; (void)a;
#endif
}

void gui_draw_point(void* win, int x, int y) {
    auto* w = (GuiWindow*)win;
    w->draw_log.push_back("point(" + std::to_string(x) + "," + std::to_string(y) + ")");
#ifdef HAVE_SDL2
    if (w->sdl_ren) SDL_RenderDrawPoint(w->sdl_ren, x, y);
#endif
}

void gui_draw_line(void* win, int x1, int y1, int x2, int y2) {
    auto* w = (GuiWindow*)win;
    w->draw_log.push_back("line(" + std::to_string(x1) + "," + std::to_string(y1) + "," + std::to_string(x2) + "," + std::to_string(y2) + ")");
#ifdef HAVE_SDL2
    if (w->sdl_ren) SDL_RenderDrawLine(w->sdl_ren, x1, y1, x2, y2);
#endif
}

void gui_draw_rect(void* win, int x, int y, int w, int h) {
    auto* win_ = (GuiWindow*)win;
    win_->draw_log.push_back("rect(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(w) + "," + std::to_string(h) + ")");
#ifdef HAVE_SDL2
    if (win_->sdl_ren) { SDL_Rect r={x,y,w,h}; SDL_RenderDrawRect(win_->sdl_ren, &r); }
#endif
}

void gui_fill_rect(void* win, int x, int y, int w, int h) {
    auto* win_ = (GuiWindow*)win;
    win_->draw_log.push_back("fill_rect(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(w) + "," + std::to_string(h) + ")");
#ifdef HAVE_SDL2
    if (win_->sdl_ren) { SDL_Rect r={x,y,w,h}; SDL_RenderFillRect(win_->sdl_ren, &r); }
#endif
}

void gui_draw_circle(void* win, int cx, int cy, int radius) {
    auto* w = (GuiWindow*)win;
    w->draw_log.push_back("circle(" + std::to_string(cx) + "," + std::to_string(cy) + "," + std::to_string(radius) + ")");
#ifdef HAVE_SDL2
    if (w->sdl_ren) {
        for (int deg = 0; deg < 360; deg++) {
            double a1 = deg * M_PI / 180.0, a2 = (deg+1) * M_PI / 180.0;
            SDL_RenderDrawLine(w->sdl_ren,
                cx + (int)(radius * cos(a1)), cy + (int)(radius * sin(a1)),
                cx + (int)(radius * cos(a2)), cy + (int)(radius * sin(a2)));
        }
    }
#endif
}

void gui_fill_circle(void* win, int cx, int cy, int radius) {
    auto* w = (GuiWindow*)win;
    w->draw_log.push_back("fill_circle(" + std::to_string(cx) + "," + std::to_string(cy) + "," + std::to_string(radius) + ")");
#ifdef HAVE_SDL2
    if (w->sdl_ren) {
        for (int dy = -radius; dy <= radius; dy++) {
            int dx = (int)sqrt((double)(radius*radius - dy*dy));
            SDL_RenderDrawLine(w->sdl_ren, cx-dx, cy+dy, cx+dx, cy+dy);
        }
    }
#endif
}

void gui_draw_triangle(void* win, int x1, int y1, int x2, int y2, int x3, int y3) {
    gui_draw_line(win, x1, y1, x2, y2);
    gui_draw_line(win, x2, y2, x3, y3);
    gui_draw_line(win, x3, y3, x1, y1);
}

void gui_fill_triangle(void* win, int x1, int y1, int x2, int y2, int x3, int y3) {
    auto* w = (GuiWindow*)win;
    w->draw_log.push_back("fill_triangle");
#ifdef HAVE_SDL2
    if (w->sdl_ren) {
        // Simple scanline fill
        int minY = std::min({y1,y2,y3}), maxY = std::max({y1,y2,y3});
        for (int y = minY; y <= maxY; y++) {
            std::vector<int> xs;
            auto edge = [&](int ax,int ay,int bx,int by) {
                if ((ay<=y&&by>y)||(by<=y&&ay>y)) {
                    xs.push_back(ax + (y-ay)*(bx-ax)/(by-ay));
                }
            };
            edge(x1,y1,x2,y2); edge(x2,y2,x3,y3); edge(x3,y3,x1,y1);
            if (xs.size() >= 2) {
                std::sort(xs.begin(), xs.end());
                SDL_RenderDrawLine(w->sdl_ren, xs[0], y, xs.back(), y);
            }
        }
    }
#endif
}

void gui_draw_text(void* win, const char* text, int x, int y, int size, int r, int g, int b) {
    auto* w = (GuiWindow*)win;
    w->draw_log.push_back(std::string("text(\"") + text + "\"," + std::to_string(x) + "," + std::to_string(y) + ")");
#ifdef HAVE_SDL2
    if (!w->sdl_ren) return;
    static const char* font_paths[] = {
        "/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:\\Windows\\Fonts\\arial.ttf",
        nullptr
    };
    TTF_Font* font = nullptr;
    for (int i = 0; font_paths[i]; i++) { font = TTF_OpenFont(font_paths[i], size); if (font) break; }
    if (!font) return;
    SDL_Color col = {(Uint8)r,(Uint8)g,(Uint8)b,255};
    SDL_Surface* surf = TTF_RenderText_Blended(font, text, col);
    if (surf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(w->sdl_ren, surf);
        if (tex) { SDL_Rect dst={x,y,surf->w,surf->h}; SDL_RenderCopy(w->sdl_ren,tex,nullptr,&dst); SDL_DestroyTexture(tex); }
        SDL_FreeSurface(surf);
    }
    TTF_CloseFont(font);
#endif
}

int gui_text_width(void* win, const char* text, int size) {
    (void)win;
    return (int)(strlen(text) * size * 0.6);
}
int gui_text_height(void* win, int size) { (void)win; return size + 4; }

// ---- Image ----
struct GuiImage {
    std::string path;
    int width=0, height=0;
#ifdef HAVE_SDL2
    SDL_Texture* tex = nullptr;
#endif
};

void* gui_image_load(void* win, const char* path) {
    auto* img = new GuiImage(); img->path = path;
#ifdef HAVE_SDL2
    auto* w = (GuiWindow*)win;
    if (w->sdl_ren) {
        SDL_Surface* surf = SDL_LoadBMP(path);
        if (surf) {
            img->width = surf->w; img->height = surf->h;
            img->tex = SDL_CreateTextureFromSurface(w->sdl_ren, surf);
            SDL_FreeSurface(surf);
        }
    }
#else
    (void)win;
#endif
    return img;
}
void gui_image_destroy(void* img) {
#ifdef HAVE_SDL2
    auto* i = (GuiImage*)img;
    if (i->tex) SDL_DestroyTexture(i->tex);
#endif
    delete (GuiImage*)img;
}
void gui_image_draw(void* win, void* img, int x, int y) {
    auto* i = (GuiImage*)img;
    gui_image_draw_scaled(win, img, x, y, i->width, i->height);
}
void gui_image_draw_scaled(void* win, void* img, int x, int y, int w, int h) {
    auto* win_ = (GuiWindow*)win;
    win_->draw_log.push_back("image(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(w) + "," + std::to_string(h) + ")");
#ifdef HAVE_SDL2
    auto* i = (GuiImage*)img;
    if (win_->sdl_ren && i->tex) { SDL_Rect dst={x,y,w,h}; SDL_RenderCopy(win_->sdl_ren,i->tex,nullptr,&dst); }
#endif
}
int gui_image_width(void* img)  { return ((GuiImage*)img)->width; }
int gui_image_height(void* img) { return ((GuiImage*)img)->height; }

// ---- Timing ----
void gui_delay(int ms) {
#ifdef HAVE_SDL2
    SDL_Delay(ms);
#else
    // headless: no-op
    (void)ms;
#endif
}
int gui_ticks() {
#ifdef HAVE_SDL2
    return (int)SDL_GetTicks();
#else
    return 0;
#endif
}
double gui_delta_time(void* win) { return ((GuiWindow*)win)->delta; }
