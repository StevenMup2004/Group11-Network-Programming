// #ifndef HOMEPAGE_H
// #define HOMEPAGE_H

// #include <gtk/gtk.h>
// #include <pango/pango.h>
// #include <pango/pangocairo.h>

// static void on_window_realize(GtkWidget *widget, gpointer user_data);
// static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data);
// static void on_window_destroy(GtkWidget *widget, gpointer data);
// static void on_calendar_day_selected(GtkCalendar *calendar, gpointer user_data);
// static gboolean on_input_box_focus_in(GtkWidget *widget, GdkEvent *event, gpointer user_data);
// static gboolean on_input_class_focus_in(GtkWidget *widget, GdkEvent *event, gpointer user_data);
// static void on_class_selected(GtkComboBox *combo_box, gpointer user_data);
// GtkWidget* create_input_box(const gchar *placeholder);
// GtkWidget* create_input_box_with_date_picker(const gchar *placeholder);
// GtkWidget* create_label(const gchar *text);
// GtkWidget* create_selection_box();
// GtkWidget* create_homepage_window();
// #endif 

#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <gtk/gtk.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

// Hàm tạo cửa sổ chính của Homepage (được gọi từ client.c hoặc component.c)
GtkWidget* create_homepage_window();

// Hàm xử lý sự kiện click nút tìm kiếm (được dùng làm callback)
void on_list_link_click(GtkWidget *widget, gpointer data);

#endif