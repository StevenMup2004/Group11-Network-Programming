// #ifndef COMPONENT_H
// #define COMPONENT_H
// #include <gtk/gtk.h>
// #include <pango/pango.h>
// #include <pango/pangocairo.h>

// GtkWidget* create_header(GtkWidget **buttons, GtkWidget *parent_container);
// static void on_button_toggled(GtkToggleButton *button, gpointer user_data);
// void show_notification(GtkWidget *widget, gpointer data);
// #endif 

#ifndef COMPONENT_H
#define COMPONENT_H

#include <gtk/gtk.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

// Hàm tạo thanh Header (được dùng ở nhiều màn hình khác nhau)
GtkWidget* create_header(GtkWidget **buttons, GtkWidget *parent_container);

// Hàm hiển thị thông báo (Popup)
void show_notification(GtkWidget *widget, gpointer data);

#endif