// #include <gtk/gtk.h>
// #include "homepage.h"
// #include <pango/pango.h>
// #include <pango/pangocairo.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <string.h>
// #include "../component/component.h"
// #include "../global/global.h"
// #include "../server_com/server_com.h"
// #include  "../list/list.h"

// GtkWidget *homepage_window ;
// GtkWidget *input_from, *input_to, *input_departure, *input_return, *combo_box, *input_traveller;
// int bytes_received; 
// void on_list_link_click(GtkWidget *widget, gpointer data) {
//     send(sock, "GET FLIGHTS", strlen("GET FLIGHTS"), 0);
//     recv(sock, &flight_count, sizeof(flight_count), 0);
//     printf("Number of flights: %d\n", flight_count);
//         int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
//         if (bytes_received <= 0) {
//             printf("Failed to receive flight data.\n");
//         }

//         buffer[bytes_received] = '\0';
//         printf("Received flight: %s\n", buffer);

//     parse_flight_data(buffer,flights);
//     for (int i = 0; i < flight_count; i++){
//         printf("ID of flight: %s\n", flights[i].flight_id);
//     }
//     const char *traveller = gtk_entry_get_text(GTK_ENTRY(input_traveller));
//     const char *selected_class = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));
//     const char *date_text;
//     GList *children = gtk_container_get_children(GTK_CONTAINER(input_departure));
//     GtkWidget *entry = GTK_WIDGET(g_list_nth_data(children, 0)); 
//     const char *from = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input_from));
//     const char *to = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input_to));
//     if (GTK_IS_ENTRY(entry)) {
//         date_text = gtk_entry_get_text(GTK_ENTRY(entry));
//     } else {
//         g_print("No entry found in input_departure\n");
//     }

//     g_list_free(children);

//     // GList *children1 = gtk_container_get_children(GTK_CONTAINER(input_return));
//     // GtkWidget *entry1 = GTK_WIDGET(g_list_nth_data(children1, 0)); 

//     // if (GTK_IS_ENTRY(entry1)) {
//     //     date_return_text = gtk_entry_get_text(GTK_ENTRY(entry1));
//     // } else {
//     //     g_print("No entry found in input_return\n");
//     // }

//     // g_list_free(children1);

//     g_print("%s %s %s %s %s %s\n", from, to, traveller, selected_class, date_text);
//     strcpy(date_tem_flight, date_text);
//     strcpy(airport_from, from);
//     strcpy(airport_to, to);
//     strcpy(class, selected_class);
//     number_seat_order = atoi(traveller);
     
//     if (strlen(from) == 0 || strlen(to) == 0 || strlen(selected_class) == 0 || strlen(date_text) == 0 || strlen(traveller) == 0){
//         gtk_label_set_text(GTK_LABEL(label_status), "All fields are required!");
//         return;
//     }
//     if (!is_number(traveller)){
//         gtk_label_set_text(GTK_LABEL(label_status), "Number people must be a number!");
//         return;
//     }
//     if (strcmp(from, to) == 0){
//         gtk_label_set_text(GTK_LABEL(label_status), "Departure airport and arrive airport must be different!");
//         return;
//     }
    
//     filter_flights(flights, flight_count, tem_flights, &tem_flight_count, from, to, date_text, selected_class, atoi(traveller));
//     for (int i = 0; i < tem_flight_count; i++){
//         printf("ID of flight: %s\n", tem_flights[i].flight_id);
//     }
//     if (tem_flight_count == 0) {
//         g_print("No flight\n");
//     }
//     GtkWidget *list_window = create_list_window();
//     if (!list_window) {
//         g_warning("Failed to create list window!");
//         return;
//     }
//     set_content(list_window);
// }
// static void on_window_realize(GtkWidget *widget, gpointer user_data) {
//     GtkWidget *calendar = GTK_WIDGET(user_data);
//     gtk_widget_hide(calendar);  
// }

// static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
//     // GdkPixbuf *pixbuf;
//     // GdkPixbuf *scaled_pixbuf;

//     // pixbuf = gdk_pixbuf_new_from_file("../assets/images/homepage.png", NULL);
//     // if (!pixbuf) {
//     //     g_print("Error loading image!\n");
//     //     return FALSE;
//     // }

//     // gint width = gtk_widget_get_allocated_width(widget);
//     // gint height = gtk_widget_get_allocated_height(widget);
//     // scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);

//     // gdk_cairo_set_source_pixbuf(cr, scaled_pixbuf, 0, 0);
//     // cairo_paint(cr);

//     // g_object_unref(pixbuf);
//     // g_object_unref(scaled_pixbuf);

//     return FALSE;
// }

// static void on_window_destroy(GtkWidget *widget, gpointer data) {
//     gtk_main_quit();
// }


// static void on_calendar_day_selected(GtkCalendar *calendar, gpointer user_data) {
//     gchar date_str[11];  
//     guint year, month, day;
//     GtkWidget *input_box = GTK_WIDGET(user_data);  

    
//     gtk_calendar_get_date(calendar, &year, &month, &day);
    
    
//     g_snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d", year, month + 1, day);
    
    
//     gtk_entry_set_text(GTK_ENTRY(input_box), date_str);
    
    
//     gtk_widget_hide(GTK_WIDGET(calendar));
// }

// static gboolean on_input_box_focus_in(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
//     GtkWidget *calendar = GTK_WIDGET(user_data);

    
//     gtk_widget_show(calendar);
//     return FALSE;
// }
// static gboolean on_input_class_focus_in(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
//     GtkWidget *combo_box = GTK_WIDGET(user_data);

    
//     gtk_widget_show(combo_box);
//     return FALSE;
// }


// static void on_class_selected(GtkComboBox *combo_box, gpointer user_data) {
//     const gchar *selected_class = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));
//     GtkWidget *input_class = GTK_WIDGET(user_data);

    
//     gtk_entry_set_text(GTK_ENTRY(input_class), selected_class);

    
//     gtk_widget_hide(GTK_WIDGET(combo_box));
// }
// static void on_window_realize1(GtkWidget *widget, gpointer user_data) {
//     GtkWidget *combo_box = GTK_WIDGET(user_data);
//     gtk_widget_hide(combo_box);  
// }

// GtkWidget* create_input_box(const gchar *placeholder) {
//     GtkWidget *input_box = gtk_entry_new();  
//     gtk_widget_set_size_request(input_box, 210, 40);  
//     gtk_entry_set_placeholder_text(GTK_ENTRY(input_box), placeholder);

    
//     GtkCssProvider *input_provider = gtk_css_provider_new();
//     gtk_css_provider_load_from_data(input_provider,
//         "entry {"
//         "  font-family: 'DM Sans', sans-serif;"
//         "  font-size: 16px;"
//         "  font-weight: 500;"
//         "  letter-spacing: 0.06em;"
//         "  text-align: center;"
//         "  border-radius: 6px;"
//         "  padding: 12px 20px;"
//         "  background-color: white;"
//         "  border: 1px solid #ccc;"
//         "  color: #283841;"
//         "}",
//         -1, NULL);

//     gtk_style_context_add_provider(gtk_widget_get_style_context(input_box), GTK_STYLE_PROVIDER(input_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

//     return input_box;
// }


// GtkWidget* create_input_box_with_date_picker(const gchar *placeholder) {
//     GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);  
//     GtkWidget *input_box = create_input_box(placeholder);  
//     GtkWidget *calendar = gtk_calendar_new();  

    
//     gtk_widget_set_size_request(calendar, 210, 90);  

    
//     gtk_box_pack_start(GTK_BOX(box), input_box, FALSE, FALSE, 0);
//     gtk_box_pack_start(GTK_BOX(box), calendar, FALSE, FALSE, 0);

    
//     g_signal_connect(input_box, "focus-in-event", G_CALLBACK(on_input_box_focus_in), calendar);

    
//     g_signal_connect(calendar, "day-selected", G_CALLBACK(on_calendar_day_selected), input_box);
    
//     g_signal_connect(gtk_widget_get_toplevel(calendar), "realize", G_CALLBACK(on_window_realize), calendar);
//     return box;
// }



// GtkWidget* create_label(const gchar *text) {
//     GtkWidget *label = gtk_label_new(text);
    
//     GtkCssProvider *label_provider = gtk_css_provider_new();
//     gtk_css_provider_load_from_data(label_provider,
//         "label {"
//         "  font-family: 'DM Sans', sans-serif;"
//         "  font-size: 16px;"
//         "  font-weight: 700;"
//         "  letter-spacing: 0.06em;"
//         "  text-align: center;"
//         "  color: #283841;"
//         "  height: 21px;"
//         "  line-height: 20.83px;"
//         "}",
//         -1, NULL);

//     gtk_style_context_add_provider(gtk_widget_get_style_context(label), GTK_STYLE_PROVIDER(label_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
//     return label;
// }

// GtkWidget* create_selection_box() {
//     GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
//     gtk_widget_set_margin_top(box, 350);   
//     gtk_widget_set_margin_start(box, 30);  
//     gtk_widget_set_margin_end(box, 30);    
//     GtkCssProvider *box_provider = gtk_css_provider_new();
    
//     for (int i = 0; i < 5; i++) {
//         GtkWidget *section_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
//         gtk_widget_set_size_request(section_box, -1, 40); 

//         if (i == 0) {
//             GtkWidget *label_from = create_label("From");

//             input_from = gtk_combo_box_text_new(); 
//             gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(input_from), "Please choose an airport");
//             for (int i = 0; i < airport_count; i++) {
//                 gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(input_from), airports[i]);   
//             }
//             gtk_combo_box_set_active(GTK_COMBO_BOX(input_from), 0); 

//             // Tạo và áp dụng CSS cho input_from
//             GtkCssProvider *input_from_provider = gtk_css_provider_new();
//             gtk_css_provider_load_from_data(input_from_provider,
//                 "combobox {"
//                 "  font-family: 'DM Sans', sans-serif;"
//                 "  font-size: 16px;"
//                 "  font-weight: 500;"
//                 "  letter-spacing: 0.06em;"
//                 "  text-align: center;"
//                 "  border-radius: 6px;"
//                 "  padding: 12px 20px;"
//                 "  background-color: white;"
//                 "  border: 1px solid #ccc;"
//                 "  color: #283841;"
//                 "}",          
//                 -1, NULL);
//             gtk_style_context_add_provider(gtk_widget_get_style_context(input_from), GTK_STYLE_PROVIDER(input_from_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

//             gtk_box_pack_start(GTK_BOX(section_box), label_from, FALSE, FALSE, 0);
//             gtk_box_pack_start(GTK_BOX(section_box), input_from, FALSE, FALSE, 0);

//             GtkWidget *label_to = create_label("To");
//             input_to = gtk_combo_box_text_new(); 
//             gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(input_to), "Please choose an airport");
//             for (int i = 0; i < airport_count; i++) {
//                 gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(input_to), airports[i]);   
//             }
//             gtk_combo_box_set_active(GTK_COMBO_BOX(input_to), 0); 

//             // Tạo và áp dụng CSS cho input_to
//             GtkCssProvider *input_to_provider = gtk_css_provider_new();
//             gtk_css_provider_load_from_data(input_to_provider,
//                 "combobox {"
//                 "  font-family: 'DM Sans', sans-serif;"
//                 "  font-size: 16px;"
//                 "  font-weight: 500;"
//                 "  letter-spacing: 0.06em;"
//                 "  text-align: center;"
//                 "  border-radius: 6px;"
//                 "  padding: 12px 20px;"
//                 "  background-color: white;"
//                 "  border: 1px solid #ccc;"
//                 "  color: #283841;"
//                 "}",          
//                 -1, NULL);
//             gtk_style_context_add_provider(gtk_widget_get_style_context(input_to), GTK_STYLE_PROVIDER(input_to_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

//             gtk_box_pack_start(GTK_BOX(section_box), label_to, FALSE, FALSE, 0);
//             gtk_box_pack_start(GTK_BOX(section_box), input_to, FALSE, FALSE, 0);
//         }        
//         else if (i == 1) {
//             GtkWidget *label_departure = create_label("Departure");
//             input_departure = create_input_box_with_date_picker("Choose Departure Date");

//             // GtkWidget *label_return = create_label("Return");
//             // input_return = create_input_box_with_date_picker("+ Add Return Date (Optional)");

            
//             gtk_box_pack_start(GTK_BOX(section_box), label_departure, FALSE, FALSE, 0);
//             gtk_box_pack_start(GTK_BOX(section_box), input_departure, FALSE, FALSE, 0);
//             // gtk_box_pack_start(GTK_BOX(section_box), label_return, FALSE, FALSE, 0);
//             // gtk_box_pack_start(GTK_BOX(section_box), input_return, FALSE, FALSE, 0);
//         }
//         else if (i == 2) {
//             GtkWidget *label_traveller = create_label("Number people");
//             input_traveller = create_input_box("Choose number people");

//             gtk_box_pack_start(GTK_BOX(section_box), label_traveller, FALSE, FALSE, 0);
//             gtk_box_pack_start(GTK_BOX(section_box), input_traveller, FALSE, FALSE, 0);
//         }
//         else if (i == 3) {
//             GtkWidget *label_class = create_label("Class");

            
//             combo_box = gtk_combo_box_text_new();
            
            
//             gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Choose Class");
            
            
//             gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Economy");
//             gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Business");
//             gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "First Class");

            
//             gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 0);  

            
//             GtkCssProvider *combo_provider = gtk_css_provider_new();
//             gtk_css_provider_load_from_data(combo_provider,
//                 "combobox {"
//                 "  font-family: 'DM Sans', sans-serif;"
//                 "  font-size: 16px;"
//                 "  font-weight: 500;"
//                 "  letter-spacing: 0.06em;"
//                 "  text-align: center;"
//                 "  border-radius: 6px;"
//                 "  padding: 12px 20px;"
//                 "  background-color: white;"
//                 "  border: 1px solid #ccc;"
//                 "  color: #283841;"
//                 "}",          
//                 -1, NULL);

//             gtk_style_context_add_provider(gtk_widget_get_style_context(combo_box), GTK_STYLE_PROVIDER(combo_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

            
//             gtk_box_pack_start(GTK_BOX(section_box), label_class, FALSE, FALSE, 0);
//             gtk_box_pack_start(GTK_BOX(section_box), combo_box, FALSE, FALSE, 0);
//         }
//         else if (i == 4) {
        
//             GtkWidget *search_button = gtk_button_new_with_label("Search Flight");
//             gtk_widget_set_name(search_button, "search_button");
//         g_signal_connect(search_button, "clicked", G_CALLBACK(on_list_link_click),homepage_window);
//             gtk_widget_set_size_request(search_button, 100, 48);

            
//             GtkCssProvider *search_provider = gtk_css_provider_new();
//             gtk_css_provider_load_from_data(search_provider,
//                 "button {"
//                 "  font-family: 'Poppins', sans-serif;"
//                 "  font-size: 16px;"
//                 "  font-weight: 600;"
//                 "  letter-spacing: 0.06em;"
//                 "  color: white;"
//                 "  background: #223A60;"
//                 "  border-radius: 8px;"
//                 "  text-align: center;"
//                 "  padding: 16px;"
//                 "}",
//                 -1, NULL);

//             gtk_style_context_add_provider(gtk_widget_get_style_context(search_button), GTK_STYLE_PROVIDER(search_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
//             gtk_widget_set_margin_top(search_button, 35);   
//             gtk_box_pack_start(GTK_BOX(section_box), search_button, FALSE, FALSE, 0);

//             label_status = gtk_label_new("");
//             gtk_box_pack_start(GTK_BOX(section_box), label_status, FALSE, FALSE, 0);
//             GtkCssProvider *css_provider_label = gtk_css_provider_new();
//             gtk_css_provider_load_from_data(css_provider_label,
//                 "label {"
//                 "   color: red;"  // Đặt màu chữ thành đỏ
//                 "}",
//                 -1, NULL);

//             // Áp dụng CSS cho label
//             gtk_style_context_add_provider(gtk_widget_get_style_context(label_status),
//                 GTK_STYLE_PROVIDER(css_provider_label), GTK_STYLE_PROVIDER_PRIORITY_USER);

//         }

        
//         gtk_box_pack_start(GTK_BOX(box), section_box, TRUE, TRUE, 0);
//     }

//     return box;
// }


// GtkWidget* create_homepage_window() {
//     GtkWidget *main_box, *header, *overlay, *darea, *selection_box;

//     // Tạo một hộp chính chứa nội dung của trang homepage
//     main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
//     gtk_widget_set_halign(main_box, GTK_ALIGN_FILL);
//     gtk_widget_set_valign(main_box, GTK_ALIGN_FILL);

//     // Tạo phần header
//     GtkWidget *buttons[4];
//     header = create_header(buttons, main_box);
//     gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 0);

//     // Tạo overlay
//     overlay = gtk_overlay_new();
//     gtk_box_pack_start(GTK_BOX(main_box), overlay, TRUE, TRUE, 0);
//     gtk_widget_set_halign(overlay, GTK_ALIGN_FILL);
//     gtk_widget_set_valign(overlay, GTK_ALIGN_FILL);

//     // Vùng vẽ
//     darea = gtk_drawing_area_new();
//     g_signal_connect(darea, "draw", G_CALLBACK(on_draw_event), NULL);
//     gtk_overlay_add_overlay(GTK_OVERLAY(overlay), darea);
//     gtk_widget_set_halign(darea, GTK_ALIGN_FILL);
//     gtk_widget_set_valign(darea, GTK_ALIGN_FILL);

//     // Tạo hộp lựa chọn
//     selection_box = create_selection_box();
//     gtk_overlay_add_overlay(GTK_OVERLAY(overlay), selection_box);
//     gtk_widget_set_halign(selection_box, GTK_ALIGN_FILL);
//     gtk_widget_set_valign(selection_box, GTK_ALIGN_FILL);

//     return main_box;
// }
/* homepage.c  - updated UI (keep all logic) */



#include <gtk/gtk.h>
#include "homepage.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h> 
#include "../component/component.h"
#include "../global/global.h"
#include "../server_com/server_com.h"
#include "../list/list.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

#define PLANE_IMAGE_PATH "../assets/images/plane.png"

GtkWidget *homepage_window;
GtkWidget *input_from, *input_to, *input_departure, *input_return, *combo_box, *input_traveller;
int bytes_received; 

// --- Forward declarations ---
static void on_window_realize(GtkWidget *widget, gpointer user_data);
static gboolean on_input_box_focus_in(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static void on_calendar_day_selected(GtkCalendar *calendar, gpointer user_data);
static gboolean on_draw_background(GtkWidget *widget, cairo_t *cr, gpointer user_data);

// ============================================================
// PHẦN 1: LOGIC (GIỮ NGUYÊN 100%)
// ============================================================

void on_list_link_click(GtkWidget *widget, gpointer data) {
    send(sock, "GET FLIGHTS", strlen("GET FLIGHTS"), 0);
    recv(sock, &flight_count, sizeof(flight_count), 0);
    int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) buffer[bytes_received] = '\0';

    parse_flight_data(buffer, flights);
    
    const char *traveller = gtk_entry_get_text(GTK_ENTRY(input_traveller));
    const char *selected_class = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));
    const char *date_text;
    
    GList *children = gtk_container_get_children(GTK_CONTAINER(input_departure));
    if (children) {
        GtkWidget *entry = GTK_WIDGET(g_list_nth_data(children, 0)); 
        if (GTK_IS_ENTRY(entry)) {
            date_text = gtk_entry_get_text(GTK_ENTRY(entry));
        } else {
            date_text = "";
        }
        g_list_free(children);
    } else {
        date_text = "";
    }

    const char *from = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input_from));
    const char *to = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input_to));

    if (from == NULL || to == NULL || selected_class == NULL || strlen(date_text) == 0 || strlen(traveller) == 0){
        gtk_label_set_text(GTK_LABEL(label_status), "All fields are required!");
        return;
    }

    strncpy(date_tem_flight, date_text, sizeof(date_tem_flight)-1);
    date_tem_flight[sizeof(date_tem_flight)-1] = '\0';
    strncpy(airport_from, from, sizeof(airport_from)-1);
    airport_from[sizeof(airport_from)-1] = '\0';
    strncpy(airport_to, to, sizeof(airport_to)-1);
    airport_to[sizeof(airport_to)-1] = '\0';
    strncpy(class, selected_class, sizeof(class)-1);
    class[sizeof(class)-1] = '\0';
    number_seat_order = atoi(traveller);
     
    if (!is_number(traveller)){
        gtk_label_set_text(GTK_LABEL(label_status), "Number people must be a number!");
        return;
    }
    if (strcmp(from, to) == 0){
        gtk_label_set_text(GTK_LABEL(label_status), "Departure airport and arrive airport must be different!");
        return;
    }
    
    filter_flights(flights, flight_count, tem_flights, &tem_flight_count, from, to, date_text, selected_class, atoi(traveller));
    
    GtkWidget *list_window = create_list_window();
    set_content(list_window);
}

// ============================================================
// PHẦN 2: UI HELPERS & DRAWING
// ============================================================

static void on_window_realize(GtkWidget *widget, gpointer user_data) {
    GtkWidget *calendar = GTK_WIDGET(user_data);
    gtk_widget_hide(calendar);  
}

static void on_calendar_day_selected(GtkCalendar *calendar, gpointer user_data) {
    gchar date_str[11];  
    guint year, month, day;
    GtkWidget *input_box = GTK_WIDGET(user_data);  
    gtk_calendar_get_date(calendar, &year, &month, &day);
    g_snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d", year, month + 1, day);
    gtk_entry_set_text(GTK_ENTRY(input_box), date_str);
    gtk_widget_hide(GTK_WIDGET(calendar));
}

static gboolean on_input_box_focus_in(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    GtkWidget *calendar = GTK_WIDGET(user_data);
    gtk_widget_show(calendar);
    return FALSE;
}

// --- VẼ BACKGROUND (CAIRO) ---
static gboolean on_draw_background(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    gint width = gtk_widget_get_allocated_width(widget);
    gint height = gtk_widget_get_allocated_height(widget);

    // 1. Vẽ nền Gradient Tím
    cairo_pattern_t *pat_left = cairo_pattern_create_linear(0.0, 0.0, width, height);
    cairo_pattern_add_color_stop_rgb(pat_left, 0.0, 0.45, 0.45, 0.95);  // Tím
    cairo_pattern_add_color_stop_rgb(pat_left, 1.0, 0.40, 0.35, 0.90);  
    cairo_set_source(cr, pat_left);
    cairo_rectangle(cr, 0, 0, width, height); 
    cairo_fill(cr);
    cairo_pattern_destroy(pat_left);

    // 2. Vẽ phần cong màu trắng (Dịch sang phải như cũ)
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // Trắng tinh khôi
    
    double start_x = width * 0.75; 
    cairo_move_to(cr, start_x, 0); 
    cairo_curve_to(cr, 
        width * 0.6, height * 0.25,   
        width * 0.6, height * 0.75,   
        width * 0.85, height);        
    cairo_line_to(cr, width, height); 
    cairo_line_to(cr, width, 0);      
    cairo_close_path(cr);
    cairo_fill(cr);

    // 3. Vẽ Máy Bay
    GError *error = NULL;
    GdkPixbuf *plane_pixbuf = gdk_pixbuf_new_from_file(PLANE_IMAGE_PATH, &error);
    if (plane_pixbuf) {
        int p_w = gdk_pixbuf_get_width(plane_pixbuf);
        int p_h = gdk_pixbuf_get_height(plane_pixbuf);
        double target_width = width * 0.5; 
        double scale = target_width / p_w;
        int dest_w = target_width;
        int dest_h = p_h * scale;
        GdkPixbuf *scaled = gdk_pixbuf_scale_simple(plane_pixbuf, dest_w, dest_h, GDK_INTERP_BILINEAR);
        double pos_x = (width - dest_w) / 2 + (width * 0.2); 
        double pos_y = (height - dest_h) / 2;
        gdk_cairo_set_source_pixbuf(cr, scaled, pos_x, pos_y);
        cairo_paint(cr);
        g_object_unref(scaled);
        g_object_unref(plane_pixbuf);
    } else {
        if (error) g_error_free(error);
    }
    return FALSE;
}

// --- CSS CUSTOM ---
void apply_custom_css(GtkWidget *widget) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        // Ô nhập liệu
        "entry, combobox button, combobox { " 
        "   background-color: #ffffff; " 
        "   color: #333333; "
        "   border-radius: 10px; "
        "   border: 1px solid #e0e0e0; "
        "   padding: 10px 15px; " 
        "   min-height: 50px; "   
        "   font-size: 15px; "
        "   font-family: 'Segoe UI', sans-serif; "
        "   box-shadow: 0 4px 6px rgba(0,0,0,0.05); "
        "}\n"
        "entry:focus, combobox button:checked { "
        "   border-color: #b3b3ff; "
        "   box-shadow: 0 0 0 3px rgba(255,255,255,0.3); " 
        "}\n"
        
        // Nút BOOK NOW
        "#btn-book { "
        "   background-image: linear-gradient(to right, #FFC371, #FF5F6D); " 
        "   color: white; font-weight: 800; font-size: 16px; "
        "   border-radius: 30px; "
        "   border: none; "
        "   padding: 12px 30px; " 
        "   box-shadow: 0 5px 15px rgba(255, 95, 109, 0.4); "
        "}\n"
        "#btn-book:hover { transform: scale(1.05); box-shadow: 0 8px 20px rgba(255, 95, 109, 0.5); }\n"
        
        // Label trắng
        ".lbl-white { color: rgba(255,255,255,0.95); font-weight: 600; font-size: 13px; margin-bottom: 6px; }\n"
        
        // Tiêu đề lớn: Font đẹp hơn
        ".lbl-title { "
        "   color: white; "
        "   font-weight: 900; "
        "   font-size: 40px; " /* To hơn */
        "   font-family: 'Montserrat', 'Verdana', sans-serif; " /* Font sang hơn */
        "   margin-bottom: 35px; "
        "   line-height: 1.2; "
        "   letter-spacing: 1px; "
        "   text-shadow: 0 2px 4px rgba(0,0,0,0.1); "
        "}\n"
        
        // --- 4 NÚT HEADER ---
        "#header-btn { "
        "   background: transparent; border: none; box-shadow: none; "
        "   color: #ffffff; "       /* MÀU TRẮNG TINH (như yêu cầu) */
        "   font-weight: 800; "     /* Chữ đậm */
        "   font-size: 16px; "      /* Chữ to lên */
        "   font-family: 'Segoe UI', sans-serif; "
        "   padding: 12px 25px; "   /* Nút to hơn */
        "   margin: 0 15px; "       /* Khoảng cách giữa các nút rộng ra */
        "   text-transform: uppercase; " /* Viết hoa toàn bộ */
        "}\n"
        "#header-btn:hover { "
        "   color: #FFC371; "       /* Hover màu Cam */
        "   text-shadow: 0 0 5px rgba(255, 195, 113, 0.5); " /* Glow nhẹ */
        "}\n"
        "#header-btn:checked { "
        "   color: #FFC371; "
        "   border-bottom: 3px solid #FFC371; " /* Gạch chân màu cam */
        "   border-radius: 0; "
        "}\n"
        , -1, NULL);
    
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

GtkWidget* create_styled_input(const gchar *placeholder) {
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
    apply_custom_css(entry);
    return entry;
}

GtkWidget* create_styled_combo() {
    GtkWidget *combo = gtk_combo_box_text_new();
    apply_custom_css(combo);
    return combo;
}

// --- FORM BÊN TRÁI ---
GtkWidget* create_left_form() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20); 
    gtk_widget_set_margin_start(vbox, 80); 
    gtk_widget_set_margin_top(vbox, 80);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER); 
    gtk_widget_set_halign(vbox, GTK_ALIGN_START);
    gtk_widget_set_size_request(vbox, 500, -1); 

    // Title
    GtkWidget *lbl_title = gtk_label_new("WHERE WOULD YOU\nLIKE TO GO ?");
    gtk_widget_set_halign(lbl_title, GTK_ALIGN_START);
    gtk_label_set_justify(GTK_LABEL(lbl_title), GTK_JUSTIFY_LEFT);
    GtkStyleContext *context = gtk_widget_get_style_context(lbl_title);
    gtk_style_context_add_class(context, "lbl-title");
    apply_custom_css(lbl_title);
    gtk_box_pack_start(GTK_BOX(vbox), lbl_title, FALSE, FALSE, 0);

    // From
    GtkWidget *box_from = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *lbl_from = gtk_label_new("From");
    gtk_widget_set_halign(lbl_from, GTK_ALIGN_START);
    apply_custom_css(lbl_from); gtk_style_context_add_class(gtk_widget_get_style_context(lbl_from), "lbl-white");
    
    input_from = create_styled_combo();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(input_from), "Select Origin");
    for(int i=0; i<airport_count; i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(input_from), airports[i]);
    gtk_combo_box_set_active(GTK_COMBO_BOX(input_from), 0);
    gtk_widget_set_hexpand(input_from, TRUE); 

    gtk_box_pack_start(GTK_BOX(box_from), lbl_from, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_from), input_from, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), box_from, FALSE, FALSE, 0);

    // To
    GtkWidget *box_to = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *lbl_to = gtk_label_new("To");
    gtk_widget_set_halign(lbl_to, GTK_ALIGN_START);
    apply_custom_css(lbl_to); gtk_style_context_add_class(gtk_widget_get_style_context(lbl_to), "lbl-white");

    input_to = create_styled_combo();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(input_to), "Select Destination");
    for(int i=0; i<airport_count; i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(input_to), airports[i]);
    gtk_combo_box_set_active(GTK_COMBO_BOX(input_to), 0);
    gtk_widget_set_hexpand(input_to, TRUE);

    gtk_box_pack_start(GTK_BOX(box_to), lbl_to, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_to), input_to, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), box_to, FALSE, FALSE, 0);

    // Row: Date | Passenger | Class
    GtkWidget *hbox_row1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);

    // Date
    GtkWidget *box_date = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *lbl_date = gtk_label_new("Date");
    gtk_widget_set_halign(lbl_date, GTK_ALIGN_START);
    apply_custom_css(lbl_date); gtk_style_context_add_class(gtk_widget_get_style_context(lbl_date), "lbl-white");

    GtkWidget *date_cont = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *entry_date = create_styled_input("YYYY-MM-DD");
    gtk_widget_set_hexpand(entry_date, TRUE); 
    GtkWidget *calendar = gtk_calendar_new();
    gtk_widget_set_no_show_all(calendar, TRUE);
    gtk_box_pack_start(GTK_BOX(date_cont), entry_date, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(date_cont), calendar, FALSE, FALSE, 0);
    g_signal_connect(entry_date, "focus-in-event", G_CALLBACK(on_input_box_focus_in), calendar);
    g_signal_connect(calendar, "day-selected", G_CALLBACK(on_calendar_day_selected), entry_date);
    g_signal_connect(gtk_widget_get_toplevel(calendar), "realize", G_CALLBACK(on_window_realize), calendar);
    input_departure = date_cont;

    gtk_box_pack_start(GTK_BOX(box_date), lbl_date, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_date), date_cont, FALSE, FALSE, 0);

    // Passengers
    GtkWidget *box_pass = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *lbl_pass = gtk_label_new("Passenger");
    gtk_widget_set_halign(lbl_pass, GTK_ALIGN_START);
    apply_custom_css(lbl_pass); gtk_style_context_add_class(gtk_widget_get_style_context(lbl_pass), "lbl-white");
    
    input_traveller = create_styled_input("1");
    gtk_widget_set_size_request(input_traveller, 80, -1); 
    
    gtk_box_pack_start(GTK_BOX(box_pass), lbl_pass, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_pass), input_traveller, FALSE, FALSE, 0);

    // Class
    GtkWidget *box_class = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *lbl_class = gtk_label_new("Class");
    gtk_widget_set_halign(lbl_class, GTK_ALIGN_START);
    apply_custom_css(lbl_class); gtk_style_context_add_class(gtk_widget_get_style_context(lbl_class), "lbl-white");

    combo_box = create_styled_combo();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Economy");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Business");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "First Class");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 0);
    gtk_widget_set_hexpand(combo_box, TRUE); 

    gtk_box_pack_start(GTK_BOX(box_class), lbl_class, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_class), combo_box, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(hbox_row1), box_date, TRUE, TRUE, 0); 
    gtk_box_pack_start(GTK_BOX(hbox_row1), box_pass, FALSE, FALSE, 0); 
    gtk_box_pack_start(GTK_BOX(hbox_row1), box_class, TRUE, TRUE, 0); 

    gtk_box_pack_start(GTK_BOX(vbox), hbox_row1, FALSE, FALSE, 10);

    // Button BOOK NOW
    GtkWidget *btn_book = gtk_button_new_with_label("BOOK NOW");
    gtk_widget_set_name(btn_book, "btn-book");
    gtk_widget_set_size_request(btn_book, 180, 55); 
    gtk_widget_set_halign(btn_book, GTK_ALIGN_START);
    gtk_widget_set_margin_top(btn_book, 20);
    apply_custom_css(btn_book);
    g_signal_connect(btn_book, "clicked", G_CALLBACK(on_list_link_click), homepage_window);

    gtk_box_pack_start(GTK_BOX(vbox), btn_book, FALSE, FALSE, 10);

    // Error Label
    label_status = gtk_label_new("");
    GtkCssProvider *err_css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(err_css, "label { color: #ffe066; font-size: 13px; }", -1, NULL); 
    gtk_style_context_add_provider(gtk_widget_get_style_context(label_status), GTK_STYLE_PROVIDER(err_css), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_box_pack_start(GTK_BOX(vbox), label_status, FALSE, FALSE, 0);

    return vbox;
}

// --- HEADER CŨ (Căn giữa, chữ to, đổi tên, màu trắng) ---
GtkWidget* create_old_style_header(GtkWidget *main_box) {
    // Sử dụng hàm create_header cũ
    GtkWidget *buttons[5]; 
    GtkWidget *header_hbox = create_header(buttons, main_box);
    
    // Xóa Logo cũ
    GList *children = gtk_container_get_children(GTK_CONTAINER(header_hbox));
    if (children) {
        GtkWidget *logo_box = GTK_WIDGET(children->data); 
        if (GTK_IS_WIDGET(logo_box)) {
            gtk_widget_destroy(logo_box); 
        }
        g_list_free(children);
    }

    // Căn lề Header vào CHÍNH GIỮA
    gtk_widget_set_margin_top(header_hbox, 30);
    gtk_widget_set_halign(header_hbox, GTK_ALIGN_CENTER); 
    
    // Tăng kích thước HBox
    gtk_widget_set_size_request(header_hbox, 900, 70);

    // Lấy container chứa nút và style lại, đổi tên
    GList *current_children = gtk_container_get_children(GTK_CONTAINER(header_hbox));
    if (current_children) {
        GtkWidget *menu_box = GTK_WIDGET(current_children->data); // MenuBox
        if (GTK_IS_CONTAINER(menu_box)) {
            gtk_widget_set_halign(menu_box, GTK_ALIGN_CENTER);
            
            GList *btn_list = gtk_container_get_children(GTK_CONTAINER(menu_box));
            int i = 0;
            for (GList *l = btn_list; l != NULL; l = l->next) {
                GtkWidget *btn = GTK_WIDGET(l->data);
                if (GTK_IS_BUTTON(btn)) {
                    gtk_widget_set_name(btn, "header-btn"); 
                    apply_custom_css(btn);
                    
                    // Đổi tên nút sang tên mới (Viết hoa)
                    if (i == 0) gtk_button_set_label(GTK_BUTTON(btn), "HOME");
                    else if (i == 1) gtk_button_set_label(GTK_BUTTON(btn), "MY BOOKINGS");
                    else if (i == 2) gtk_button_set_label(GTK_BUTTON(btn), "MY ACCOUNT");
                    else if (i == 3) gtk_button_set_label(GTK_BUTTON(btn), "HELP CENTER");
                    
                    i++;
                }
            }
            g_list_free(btn_list);
        }
        g_list_free(current_children);
    }

    return header_hbox;
}


GtkWidget* create_homepage_window() {
    GtkWidget *main_overlay = gtk_overlay_new();
    
    GtkWidget *drawing_area = gtk_drawing_area_new();
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw_background), NULL);
    gtk_overlay_add_overlay(GTK_OVERLAY(main_overlay), drawing_area);

    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    GtkWidget *header = create_old_style_header(content_box);
    gtk_box_pack_start(GTK_BOX(content_box), header, FALSE, FALSE, 0);

    GtkWidget *form = create_left_form();
    gtk_box_pack_start(GTK_BOX(content_box), form, TRUE, TRUE, 0); 

    gtk_overlay_add_overlay(GTK_OVERLAY(main_overlay), content_box);
    homepage_window = main_overlay; 

    return main_overlay;
}