#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ===== Phase 1: Text Rendering =====
void* gui_font_manager_create(void);
void  gui_font_manager_destroy(void* fm);
void* gui_font_load(void* fm, const char* name, int size, const char* style);
int   gui_font_manager_count(void* fm);
const char* gui_font_name(void* font);
int   gui_font_size(void* font);
const char* gui_font_style(void* font);

void* gui_text_renderer_create(void);
void  gui_text_renderer_destroy(void* tr);
void  gui_text_renderer_set_font(void* tr, void* font);
void  gui_text_renderer_set_color(void* tr, float r, float g, float b, float a);
void  gui_text_renderer_set_align(void* tr, const char* align);
void  gui_text_renderer_set_antialias(void* tr, int enabled);
const char* gui_text_renderer_render(void* tr, const char* text);
int   gui_text_renderer_line_count(void* tr);
const char* gui_text_renderer_line_at(void* tr, int idx);

void* gui_text_metrics_measure(void* font, const char* text);
void  gui_text_metrics_destroy(void* tm);
int   gui_text_metrics_width(void* tm);
int   gui_text_metrics_height(void* tm);
int   gui_text_metrics_ascent(void* tm);
int   gui_text_metrics_descent(void* tm);
int   gui_text_metrics_line_height(void* tm);

// ===== Phase 2: Mouse Support =====
void* gui_mouse_handler_create(void);
void  gui_mouse_handler_destroy(void* mh);
void  gui_mouse_handler_click(void* mh, int x, int y, int button);
void  gui_mouse_handler_move(void* mh, int x, int y);
void  gui_mouse_handler_press(void* mh, int x, int y, int button);
void  gui_mouse_handler_release(void* mh, int x, int y, int button);
void  gui_mouse_handler_scroll(void* mh, int x, int y, int delta);
int   gui_mouse_handler_event_count(void* mh);
const char* gui_mouse_handler_event_type(void* mh, int idx);
int   gui_mouse_handler_event_x(void* mh, int idx);
int   gui_mouse_handler_event_y(void* mh, int idx);
int   gui_mouse_handler_event_button(void* mh, int idx);
int   gui_mouse_handler_cursor_x(void* mh);
int   gui_mouse_handler_cursor_y(void* mh);
int   gui_mouse_handler_is_button_down(void* mh, int button);

void* gui_drag_drop_create(void);
void  gui_drag_drop_destroy(void* dd);
void  gui_drag_drop_start(void* dd, int x, int y, const char* data, const char* mime);
void  gui_drag_drop_move(void* dd, int x, int y);
int   gui_drag_drop_drop(void* dd, int x, int y);
int   gui_drag_drop_is_dragging(void* dd);
const char* gui_drag_drop_data(void* dd);
const char* gui_drag_drop_mime(void* dd);
int   gui_drag_drop_start_x(void* dd);
int   gui_drag_drop_start_y(void* dd);

void* gui_context_menu_create(void);
void  gui_context_menu_destroy(void* cm);
void  gui_context_menu_add_item(void* cm, const char* label, const char* action);
void  gui_context_menu_add_separator(void* cm);
void  gui_context_menu_show(void* cm, int x, int y);
void  gui_context_menu_hide(void* cm);
int   gui_context_menu_item_count(void* cm);
const char* gui_context_menu_item_label(void* cm, int idx);
const char* gui_context_menu_item_action(void* cm, int idx);
int   gui_context_menu_is_visible(void* cm);
int   gui_context_menu_x(void* cm);
int   gui_context_menu_y(void* cm);

// ===== Phase 3: Advanced Widgets =====
void* gui_text_input_create(void);
void  gui_text_input_destroy(void* ti);
void  gui_text_input_set_text(void* ti, const char* text);
void  gui_text_input_set_placeholder(void* ti, const char* ph);
void  gui_text_input_set_max_length(void* ti, int max);
void  gui_text_input_set_password(void* ti, int enabled);
void  gui_text_input_set_multiline(void* ti, int enabled);
void  gui_text_input_insert(void* ti, const char* text);
void  gui_text_input_clear(void* ti);
const char* gui_text_input_text(void* ti);
int   gui_text_input_length(void* ti);
int   gui_text_input_cursor_pos(void* ti);
int   gui_text_input_is_focused(void* ti);
void  gui_text_input_focus(void* ti);
void  gui_text_input_blur(void* ti);

void* gui_dropdown_create(void);
void  gui_dropdown_destroy(void* dd);
void  gui_dropdown_add_option(void* dd, const char* label, const char* value);
void  gui_dropdown_set_selected(void* dd, int idx);
void  gui_dropdown_set_placeholder(void* dd, const char* ph);
void  gui_dropdown_open(void* dd);
void  gui_dropdown_close(void* dd);
int   gui_dropdown_option_count(void* dd);
const char* gui_dropdown_option_label(void* dd, int idx);
const char* gui_dropdown_option_value(void* dd, int idx);
int   gui_dropdown_selected_index(void* dd);
const char* gui_dropdown_selected_value(void* dd);
int   gui_dropdown_is_open(void* dd);

void* gui_table_create(void);
void  gui_table_destroy(void* tbl);
void  gui_table_add_column(void* tbl, const char* header, int width);
void  gui_table_add_row(void* tbl);
void  gui_table_set_cell(void* tbl, int row, int col, const char* value);
const char* gui_table_get_cell(void* tbl, int row, int col);
int   gui_table_row_count(void* tbl);
int   gui_table_col_count(void* tbl);
const char* gui_table_column_header(void* tbl, int col);
int   gui_table_column_width(void* tbl, int col);
void  gui_table_set_selected_row(void* tbl, int row);
int   gui_table_selected_row(void* tbl);
void  gui_table_sort_by_column(void* tbl, int col, int ascending);
const char* gui_table_to_csv(void* tbl);

void* gui_tree_view_create(void);
void  gui_tree_view_destroy(void* tv);
int   gui_tree_view_add_node(void* tv, int parent_id, const char* label, const char* data);
void  gui_tree_view_expand(void* tv, int node_id);
void  gui_tree_view_collapse(void* tv, int node_id);
void  gui_tree_view_select(void* tv, int node_id);
int   gui_tree_view_node_count(void* tv);
const char* gui_tree_view_node_label(void* tv, int node_id);
const char* gui_tree_view_node_data(void* tv, int node_id);
int   gui_tree_view_node_parent(void* tv, int node_id);
int   gui_tree_view_node_is_expanded(void* tv, int node_id);
int   gui_tree_view_selected_node(void* tv);
int   gui_tree_view_child_count(void* tv, int node_id);

void* gui_tabs_create(void);
void  gui_tabs_destroy(void* tabs);
void  gui_tabs_add_tab(void* tabs, const char* label, const char* content_id);
void  gui_tabs_set_active(void* tabs, int idx);
void  gui_tabs_remove_tab(void* tabs, int idx);
int   gui_tabs_count(void* tabs);
const char* gui_tabs_label(void* tabs, int idx);
const char* gui_tabs_content_id(void* tabs, int idx);
int   gui_tabs_active_index(void* tabs);
const char* gui_tabs_active_label(void* tabs);

// ===== Phase 4: Layout Management =====
void* gui_flex_layout_create(const char* direction);
void  gui_flex_layout_destroy(void* fl);
void  gui_flex_layout_add_item(void* fl, const char* id, int flex, int min_size, int max_size);
void  gui_flex_layout_set_gap(void* fl, int gap);
void  gui_flex_layout_set_padding(void* fl, int padding);
void  gui_flex_layout_set_wrap(void* fl, int wrap);
void  gui_flex_layout_set_align(void* fl, const char* align);
void  gui_flex_layout_set_justify(void* fl, const char* justify);
int   gui_flex_layout_item_count(void* fl);
const char* gui_flex_layout_item_id(void* fl, int idx);
int   gui_flex_layout_item_flex(void* fl, int idx);
const char* gui_flex_layout_compute(void* fl, int total_width, int total_height);
const char* gui_flex_layout_direction(void* fl);

void* gui_grid_layout_create(int cols, int rows);
void  gui_grid_layout_destroy(void* gl);
void  gui_grid_layout_place(void* gl, const char* id, int col, int row, int col_span, int row_span);
void  gui_grid_layout_set_col_width(void* gl, int col, int width);
void  gui_grid_layout_set_row_height(void* gl, int row, int height);
void  gui_grid_layout_set_gap(void* gl, int gap);
int   gui_grid_layout_cols(void* gl);
int   gui_grid_layout_rows(void* gl);
int   gui_grid_layout_item_count(void* gl);
const char* gui_grid_layout_item_id(void* gl, int idx);
const char* gui_grid_layout_compute(void* gl, int total_width, int total_height);

void* gui_anchor_layout_create(void);
void  gui_anchor_layout_destroy(void* al);
void  gui_anchor_layout_add_item(void* al, const char* id, int x, int y, int width, int height);
void  gui_anchor_layout_set_anchor(void* al, const char* id, const char* edge, int offset);
void  gui_anchor_layout_set_relative(void* al, const char* id, const char* ref_id, const char* edge);
int   gui_anchor_layout_item_count(void* al);
const char* gui_anchor_layout_item_id(void* al, int idx);
const char* gui_anchor_layout_compute(void* al, int total_width, int total_height);

#ifdef __cplusplus
}
#endif

// ===== Window System (SDL2-backed, headless-safe) =====

// Lifecycle
int   gui_init(void);          // returns 1 on success, 0 on failure (or headless)
void  gui_quit(void);
int   gui_has_display(void);   // 1 if a real display is available

// Window
void* gui_window_create(const char* title, int width, int height);
void  gui_window_destroy(void* win);
void  gui_window_show(void* win);
void  gui_window_hide(void* win);
void  gui_window_set_title(void* win, const char* title);
const char* gui_window_title(void* win);
int   gui_window_width(void* win);
int   gui_window_height(void* win);
int   gui_window_is_open(void* win);
void  gui_window_close(void* win);
void  gui_window_resize(void* win, int w, int h);
void  gui_window_set_resizable(void* win, int enabled);
void  gui_window_set_fullscreen(void* win, int enabled);
int   gui_window_is_fullscreen(void* win);

// Event pump — returns 1 while window is open, 0 when closed/quit
int   gui_window_poll(void* win);

// Keyboard input (SDL scancode integers)
int   gui_key_down(void* win, int scancode);
int   gui_key_pressed(void* win, int scancode);  // true only on the frame it was pressed
// Named key helpers (return scancode int)
int   gui_key(const char* name);  // "up","down","left","right","space","escape","enter","a".."z","0".."9"

// Mouse input
int   gui_mouse_x(void* win);
int   gui_mouse_y(void* win);
int   gui_mouse_button(void* win, int btn);  // btn: 1=left,2=middle,3=right

// Rendering — 2D drawing API
void  gui_clear(void* win, int r, int g, int b);
void  gui_present(void* win);
void  gui_set_color(void* win, int r, int g, int b, int a);
void  gui_draw_point(void* win, int x, int y);
void  gui_draw_line(void* win, int x1, int y1, int x2, int y2);
void  gui_draw_rect(void* win, int x, int y, int w, int h);
void  gui_fill_rect(void* win, int x, int y, int w, int h);
void  gui_draw_circle(void* win, int cx, int cy, int radius);
void  gui_fill_circle(void* win, int cx, int cy, int radius);
void  gui_draw_triangle(void* win, int x1, int y1, int x2, int y2, int x3, int y3);
void  gui_fill_triangle(void* win, int x1, int y1, int x2, int y2, int x3, int y3);

// Text rendering
void  gui_draw_text(void* win, const char* text, int x, int y, int size, int r, int g, int b);
int   gui_text_width(void* win, const char* text, int size);
int   gui_text_height(void* win, int size);

// Image/surface
void* gui_image_load(void* win, const char* path);
void  gui_image_destroy(void* img);
void  gui_image_draw(void* win, void* img, int x, int y);
void  gui_image_draw_scaled(void* win, void* img, int x, int y, int w, int h);
int   gui_image_width(void* img);
int   gui_image_height(void* img);

// Timing
void  gui_delay(int ms);
int   gui_ticks(void);   // milliseconds since gui_init
double gui_delta_time(void* win);  // seconds since last gui_present
