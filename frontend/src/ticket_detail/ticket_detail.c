// #include <gtk/gtk.h>
// #include "ticket_detail.h"
// #include <pango/pango.h>
// #include <pango/pangocairo.h>
// #include <math.h>
// #include <string.h>
// #include <arpa/inet.h>
// #include "../global/global.h"
// #include "../list/list.h"
// #include "../book_seat/book_seat.h"
// gint screen_width;
// gint screen_height;
// cairo_text_extents_t cancel_extents;


// gboolean on_mouse_click_ticket_detail(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
//     double cancel_button_x = (screen_width - 936) / 2 + (936 - 344) / 2;
//     double cancel_button_y = (screen_height - 588) / 2 + 500;
//     double cancel_button_width = 156;
//     double cancel_button_height = 56;

//     double confirm_button_x = (screen_width - 936) / 2 + (936 - 344) / 2 + 180;
//     double confirm_button_y = (screen_height - 588) / 2 + 500;
//     double confirm_button_width = 156;
//     double confirm_button_height = 56;

//     if (event->x >= cancel_button_x &&
//         event->x <= cancel_button_x + cancel_button_width &&
//         event->y >= cancel_button_y &&
//         event->y <= cancel_button_y + cancel_button_height) {
//         g_print("Cancel button clicked in ticket detail!\n");
//         GtkWidget *list_window = create_list_window();
//         set_content(list_window);
//     }

//     if (event->x >= confirm_button_x &&
//         event->x <= confirm_button_x + confirm_button_width &&
//         event->y >= confirm_button_y &&
//         event->y <= confirm_button_y + confirm_button_height) {
//         g_print("Confirm button clicked!\n");
//         sprintf(buffer, "GET ORDERED SEATS %s", detail_flight.flight_id);
//         send(sock, buffer, strlen(buffer), 0);
//         int bytes_received = recv(sock, buffer, sizeof(buffer), 0);
//         if (bytes_received > 0) {
//             buffer[bytes_received] = '\0';
//             if (strcmp(buffer, "NOTHING") == 0){
//                 g_print("No ordered seats.\n");
//                 GtkWidget *book_seat_window = create_book_seat_window();
//                 set_content(book_seat_window);
//                 return TRUE;
//             }
//             sscanf(buffer, "SEAT COUNT: %d", &seat_count); 
//         }
//         g_print("Start\n");
//         seats_array = malloc(seat_count * sizeof(char *));
//         bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
//         if (bytes_received <= 0) {
//             perror("Error receiving seats data");
//             return TRUE;
//         }
//         buffer[bytes_received] = '\0'; 
//         printf("Received seats string: %s\n", buffer);

//         char *token = strtok(buffer, ",");
//         int i = 0;
//         while (token) {
//             seats_array[i] = strdup(token);
//             if (!seats_array[i]) {
//                 perror("Error duplicating seat string");
//                 break;
//             }
//             token = strtok(NULL, ",");
//             i++;
//         }

//         printf("Parsed seats:\n");
//         for (int j = 0; j < seat_count; j++) {
//             printf("Seat %d: %s\n", j + 1, seats_array[j]);
//         }
//         printf("Receive done\n");
//         GtkWidget *book_seat_window = create_book_seat_window();
//         set_content(book_seat_window);
//     }

//     return TRUE; 
// }

// static gboolean on_ticket_detail_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
//     // GdkPixbuf *bg_pixbuf;
//     // GdkPixbuf *scaled_pixbuf;
//     char date[20];
//     char time[20];
//     split_date_time(detail_flight.departure_time, date, time);

    
//     screen_width = gtk_widget_get_allocated_width(widget);
//     screen_height = gtk_widget_get_allocated_height(widget);

    
//     // bg_pixbuf = gdk_pixbuf_new_from_file("../assets/images/bg_login.png", NULL);
//     // if (!bg_pixbuf) {
//     //     g_print("Error loading image!\n");
//     //     return FALSE;
//     // }

//     // scaled_pixbuf = gdk_pixbuf_scale_simple(bg_pixbuf, screen_width, screen_height, GDK_INTERP_BILINEAR);
//     // gdk_cairo_set_source_pixbuf(cr, scaled_pixbuf, 0, 0);
//     // cairo_paint(cr);
//     // g_object_unref(scaled_pixbuf);
//     // g_object_unref(bg_pixbuf);

    
    
//     cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); 
//     gfloat radius = 24.0;

    
//     cairo_move_to(cr, (screen_width - 936) / 2 + radius, (screen_height - 588) / 2); 
//     cairo_arc(cr, (screen_width - 936) / 2 + 936 - radius, (screen_height - 588) / 2 + radius, radius, 3 * M_PI / 2, 2 * M_PI); 
//     cairo_arc(cr, (screen_width - 936) / 2 + 936 - radius, (screen_height - 588) / 2 + 588 - radius, radius, 0, M_PI / 2); 
//     cairo_arc(cr, (screen_width - 936) / 2 + radius, (screen_height - 588) / 2 + 588 - radius, radius, M_PI / 2, M_PI); 
//     cairo_arc(cr, (screen_width - 936) / 2 + radius, (screen_height - 588) / 2 + radius, radius, M_PI, 3 * M_PI / 2); 

//     cairo_close_path(cr); 

//     cairo_set_line_width(cr, 1.0);
//     cairo_fill_preserve(cr); 
//     cairo_set_source_rgb(cr, 0.9, 0.9, 0.9); 
//     cairo_stroke(cr); 



    
    
//     GdkPixbuf *airline_logo = gdk_pixbuf_new_from_file("../assets/images/airline.png", NULL);
//     if (airline_logo) {
//         GdkPixbuf *scaled_logo = gdk_pixbuf_scale_simple(airline_logo, 328, 80, GDK_INTERP_BILINEAR);
//         gdk_cairo_set_source_pixbuf(cr, scaled_logo, (screen_width - 936) / 2 + (936 - 328) / 2, (screen_height - 588) / 2 + 10);
//         cairo_paint(cr);
//         g_object_unref(scaled_logo);
//         g_object_unref(airline_logo);
//     }

    
//     cairo_set_source_rgb(cr, 0.91, 0.91, 0.91); 
//     cairo_set_line_width(cr, 1.0);
//     cairo_move_to(cr, (screen_width - 936) / 2 + (936 - 600) / 2, (screen_height - 588) / 2 + 90);
//     cairo_line_to(cr, (screen_width - 936) / 2 + (936 - 600) / 2 + 600, (screen_height - 588) / 2 + 90);
//     cairo_stroke(cr);

    
    
//     cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
//     cairo_set_font_size(cr, 24);
//     cairo_move_to(cr, (screen_width - 936) / 2 + 200, (screen_height - 588) / 2 + 160);
//     cairo_show_text(cr, extract_middle_string(detail_flight.departure_airport));

    
//     cairo_set_source_rgb(cr, 0.4, 0.4, 0.4); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
//     cairo_set_font_size(cr, 20);
//     cairo_move_to(cr, (screen_width - 936) / 2 + 200, (screen_height - 588) / 2 + 190);
//     cairo_show_text(cr, extract_middle_string(detail_flight.departure_airport));

    
//     cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
//     cairo_move_to(cr, (screen_width - 936) / 2 + 600, (screen_height - 588) / 2 + 160);
//     cairo_set_font_size(cr, 24);
//     cairo_show_text(cr, extract_middle_string(detail_flight.arrival_airport));

    
//     cairo_set_source_rgb(cr, 0.4, 0.4, 0.4); 
//     cairo_move_to(cr, (screen_width - 936) / 2 + 600, (screen_height - 588) / 2 + 190);
//     cairo_set_font_size(cr, 20);
//     cairo_show_text(cr, extract_middle_string(detail_flight.arrival_airport));

    
//     GdkPixbuf *flight_icon = gdk_pixbuf_new_from_file("../assets/images/flight-icon.png", NULL);
//     if (flight_icon) {
//         GdkPixbuf *scaled_flight_icon = gdk_pixbuf_scale_simple(flight_icon, 156, 36, GDK_INTERP_BILINEAR);
//         gdk_cairo_set_source_pixbuf(cr, scaled_flight_icon, (screen_width - 936) / 2 + (936 - 156) / 2, (screen_height - 588) / 2 + 160);
//         cairo_paint(cr);
//         g_object_unref(scaled_flight_icon);
//         g_object_unref(flight_icon);
//     }

    
//     cairo_set_source_rgb(cr, 0.91, 0.91, 0.91); 
//     cairo_move_to(cr, (screen_width - 936) / 2 + (936 - 600) / 2, (screen_height - 588) / 2 + 250);
//     cairo_line_to(cr, (screen_width - 936) / 2 + (936 - 600) / 2 + 600, (screen_height - 588) / 2 + 250);
//     cairo_stroke(cr);

    
    
//         cairo_set_source_rgb(cr, 0.4, 0.4, 0.4); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
//     cairo_set_font_size(cr, 20); 

    
//     cairo_text_extents_t date_extents, time_extents;
//     cairo_text_extents(cr, "Date", &date_extents);
//     cairo_text_extents(cr, "Time", &time_extents);

    
//     cairo_move_to(cr, (screen_width - 936) / 2 + 270 + (140 + 16) / 2 - date_extents.width / 2, 
//                 (screen_height - 588) / 2 + 300 - 10); 
//     cairo_show_text(cr, "Date");

    
//     cairo_move_to(cr, (screen_width - 936) / 2 + 16 + 140 + 270 + (140 + 16) / 2 - time_extents.width / 2,
//                 (screen_height - 588) / 2 + 300 - 10); 
//     cairo_show_text(cr, "Time");

    
//     double date_x = (screen_width - 936) / 2 + 320;
//     double time_x = date_x + 140 + 16;

    
    
//     cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); 
//     cairo_new_path(cr);

    
//     cairo_move_to(cr, date_x + 8, (screen_height - 588) / 2 + 300); 

    
//     cairo_line_to(cr, date_x + 140 - 8, (screen_height - 588) / 2 + 300); 
//     cairo_arc(cr, date_x + 140 - 8, (screen_height - 588) / 2 + 300 + 8, 8, 3 * M_PI / 2, 2 * M_PI); 

//     cairo_line_to(cr, date_x + 140, (screen_height - 588) / 2 + 300 + 46 - 8); 
//     cairo_arc(cr, date_x + 140 - 8, (screen_height - 588) / 2 + 300 + 46 - 8, 8, 0, M_PI / 2); 

//     cairo_line_to(cr, date_x + 8, (screen_height - 588) / 2 + 300 + 46); 
//     cairo_arc(cr, date_x + 8, (screen_height - 588) / 2 + 300 + 46 - 8, 8, M_PI / 2, M_PI); 

//     cairo_line_to(cr, date_x, (screen_height - 588) / 2 + 300 + 8); 
//     cairo_arc(cr, date_x + 8, (screen_height - 588) / 2 + 300 + 8, 8, M_PI, 3 * M_PI / 2); 

//     cairo_close_path(cr); 

//     cairo_fill_preserve(cr); 
//     cairo_set_source_rgb(cr, 0.91, 0.91, 0.91); 
//     cairo_stroke(cr); 

    
//     GdkPixbuf *calendar_icon = gdk_pixbuf_new_from_file("../assets/images/calendar.png", NULL);
//     if (calendar_icon) {
//         GdkPixbuf *scaled_calendar_icon = gdk_pixbuf_scale_simple(calendar_icon, 20, 20, GDK_INTERP_BILINEAR);
//         gdk_cairo_set_source_pixbuf(cr, scaled_calendar_icon, date_x + 8, (screen_height - 588) / 2 + 300 + (46 - 20) / 2);
//         cairo_paint(cr);
//         g_object_unref(scaled_calendar_icon);
//         g_object_unref(calendar_icon);
//     }

    
//     cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
//     cairo_set_font_size(cr, 18); 
    
//     cairo_text_extents_t date_text_extents;
//     cairo_text_extents(cr, date, &date_text_extents);

//     double date_text_y = (screen_height - 588) / 2 + 315 + (46 - date_text_extents.height) / 2;
    
//     cairo_move_to(cr, date_x + (160 - date_text_extents.width) / 2, date_text_y);
//     cairo_show_text(cr, date);
    
//     cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); 
//     cairo_new_path(cr);
    
//     cairo_move_to(cr, time_x + 8, (screen_height - 588) / 2 + 300); 

    
//     cairo_line_to(cr, time_x + 140 - 8, (screen_height - 588) / 2 + 300); 
//     cairo_arc(cr, time_x + 140 - 8, (screen_height - 588) / 2 + 300 + 8, 8, 3 * M_PI / 2, 2 * M_PI); 

//     cairo_line_to(cr, time_x + 140, (screen_height - 588) / 2 + 300 + 46 - 8); 
//     cairo_arc(cr, time_x + 140 - 8, (screen_height - 588) / 2 + 300 + 46 - 8, 8, 0, M_PI / 2); 

//     cairo_line_to(cr, time_x + 8, (screen_height - 588) / 2 + 300 + 46); 
//     cairo_arc(cr, time_x + 8, (screen_height - 588) / 2 + 300 + 46 - 8, 8, M_PI / 2, M_PI); 

//     cairo_line_to(cr, time_x, (screen_height - 588) / 2 + 300 + 8); 
//     cairo_arc(cr, time_x + 8, (screen_height - 588) / 2 + 300 + 8, 8, M_PI, 3 * M_PI / 2); 

//     cairo_close_path(cr); 

//     cairo_fill_preserve(cr); 
//     cairo_set_source_rgb(cr, 0.91, 0.91, 0.91); 
//     cairo_stroke(cr); 


    
//     GdkPixbuf *clock_icon = gdk_pixbuf_new_from_file("../assets/images/Clock.png", NULL);
//     if (clock_icon) {
//         GdkPixbuf *scaled_clock_icon = gdk_pixbuf_scale_simple(clock_icon, 20, 20, GDK_INTERP_BILINEAR);
//         gdk_cairo_set_source_pixbuf(cr, scaled_clock_icon, time_x + 8, (screen_height - 588) / 2 + 300 + (46 - 20) / 2);
//         cairo_paint(cr);
//         g_object_unref(scaled_clock_icon);
//         g_object_unref(clock_icon);
//     }
//     cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
//     cairo_set_font_size(cr, 18); 
//     cairo_text_extents_t time_text_extents;
//     cairo_text_extents(cr, time, &time_text_extents);  

    
//     double time_text_y = (screen_height - 588) / 2 + 315 + (46 - time_text_extents.height) / 2;  

    
//     cairo_move_to(cr, time_x + (150 - time_text_extents.width) / 2, time_text_y);

    
//     cairo_show_text(cr, time);

//     cairo_set_source_rgb(cr, 0.91, 0.91, 0.91); 
//     cairo_move_to(cr, (screen_width - 936) / 2 + (936 - 600) / 2, (screen_height - 588) / 2 + 380);
//     cairo_line_to(cr, (screen_width - 936) / 2 + (936 - 600) / 2 + 600, (screen_height - 588) / 2 + 380);
//     cairo_stroke(cr);

    
//     cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); 
//     cairo_set_font_size(cr, 22); 
//     cairo_text_extents_t price_text_extents;
//     cairo_text_extents(cr, "Price", &price_text_extents); 

    
//     double price_x = (screen_width - 936) / 2 + (160 - price_text_extents.width) / 2 + 300;
//     double price_y = (screen_height - 588) / 2 + 430;

    
//     cairo_move_to(cr, price_x, price_y);
//     cairo_show_text(cr, "Price");

    
//     cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); 
//     cairo_set_font_size(cr, 32); 
//     cairo_text_extents_t price_value_extents;
//     cairo_text_extents(cr, format_number_with_separator(detail_flight.price, ','), &price_value_extents); 

    
//     double price_value_x = price_x + price_text_extents.width + 10; 
//     double price_value_y = price_y; 

    
//     cairo_move_to(cr, price_value_x, price_value_y);
//     cairo_show_text(cr, format_number_with_separator(detail_flight.price, ','));


    
    
//     cairo_set_source_rgb(cr, 0.92, 0.94, 0.94); 
//     cairo_new_path(cr);
//     cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 8, (screen_height - 588) / 2 + 500 + 8, 8, M_PI, 3 * M_PI / 2); 
//     cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 156 - 8, (screen_height - 588) / 2 + 500 + 8, 8, 3 * M_PI / 2, 2 * M_PI); 
//     cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 156 - 8, (screen_height - 588) / 2 + 500 + 56 - 8, 8, 0, M_PI / 2); 
//     cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 8, (screen_height - 588) / 2 + 500 + 56 - 8, 8, M_PI / 2, M_PI); 
//     cairo_close_path(cr);
//     cairo_fill_preserve(cr); 
//     cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
//     cairo_set_line_width(cr, 1.0);
//     cairo_stroke(cr); 

    
//     cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL); 
//     cairo_set_font_size(cr, 18); 

    
//     cancel_extents;
//     cairo_text_extents(cr, "Cancel", &cancel_extents);
//     cairo_move_to(
//         cr,
//         (screen_width - 936) / 2 + (936 - 344) / 2 + 156 / 2 - cancel_extents.width / 2,
//         (screen_height - 588) / 2 + 500 + 56 / 2 + cancel_extents.height / 2
//     );
//     cairo_show_text(cr, "Cancel");
    
//     cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
//     cairo_new_path(cr);
//     cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 180 + 8, (screen_height - 588) / 2 + 500 + 8, 8, M_PI, 3 * M_PI / 2); 
//     cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 180 + 156 - 8, (screen_height - 588) / 2 + 500 + 8, 8, 3 * M_PI / 2, 2 * M_PI); 
//     cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 180 + 156 - 8, (screen_height - 588) / 2 + 500 + 56 - 8, 8, 0, M_PI / 2); 
//     cairo_arc(cr, (screen_width - 936) / 2 + (936 - 344) / 2 + 180 + 8, (screen_height - 588) / 2 + 500 + 56 - 8, 8, M_PI / 2, M_PI); 
//     cairo_close_path(cr);
//     cairo_fill_preserve(cr); 
//     cairo_stroke(cr); 

    
//     cairo_set_source_rgb(cr, 0.92, 0.94, 0.94); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL); 
//     cairo_set_font_size(cr, 18); 

    
//     cairo_text_extents_t confirm_extents;
//     cairo_text_extents(cr, "Confirm", &confirm_extents);
//     cairo_move_to(
//         cr,
//         (screen_width - 936) / 2 + (936 - 344) / 2 + 180 + 156 / 2 - confirm_extents.width / 2,
//         (screen_height - 588) / 2 + 500 + 56 / 2 + confirm_extents.height / 2
//     );
//     cairo_show_text(cr, "Confirm");

//         return FALSE;
//     }
// GtkWidget* create_ticket_detail_window() {
//     GtkWidget *drawing_area, *main_box;

    
//     main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    
//     drawing_area = gtk_drawing_area_new();
//     g_signal_connect(drawing_area, "draw", G_CALLBACK(on_ticket_detail_draw), NULL);

//     gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
//     g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_mouse_click_ticket_detail), NULL);
    
//     gtk_box_pack_start(GTK_BOX(main_box), drawing_area, TRUE, TRUE, 0);

//     return main_box;
// }

// #include <gtk/gtk.h>
// #include "ticket_detail.h"
// #include <pango/pango.h>
// #include <pango/pangocairo.h>
// #include <math.h>
// #include <string.h>
// #include <arpa/inet.h>
// #include "../global/global.h"
// #include "../list/list.h"
// #include "../book_seat/book_seat.h"

// gint screen_width;
// gint screen_height;

// // Kích thước cố định của Card (Giữ nguyên để không hỏng logic click)
// #define CARD_WIDTH 936
// #define CARD_HEIGHT 588

// // --- HÀM XỬ LÝ CLICK (LOGIC GIỮ NGUYÊN, CHỈ FIX LỖI) ---
// gboolean on_mouse_click_ticket_detail(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
//     (void)widget;
//     (void)user_data;

//     // Tính toán lại tọa độ dựa trên kích thước màn hình hiện tại
//     double start_x = (screen_width - CARD_WIDTH) / 2.0;
//     double start_y = (screen_height - CARD_HEIGHT) / 2.0;

//     double cancel_button_x = start_x + (CARD_WIDTH - 344) / 2.0;
//     double cancel_button_y = start_y + 500;
//     double cancel_button_width = 156;
//     double cancel_button_height = 56;

//     double confirm_button_x = start_x + (CARD_WIDTH - 344) / 2.0 + 180;
//     double confirm_button_y = start_y + 500;
//     double confirm_button_width = 156;
//     double confirm_button_height = 56;

//     // Xử lý nút Cancel
//     if (event->x >= cancel_button_x &&
//         event->x <= cancel_button_x + cancel_button_width &&
//         event->y >= cancel_button_y &&
//         event->y <= cancel_button_y + cancel_button_height) {
        
//         g_print("Cancel button clicked in ticket detail!\n");
//         GtkWidget *list_window = create_list_window();
//         set_content(list_window);
//         return TRUE;
//     }

//     // Xử lý nút Confirm
//     if (event->x >= confirm_button_x &&
//         event->x <= confirm_button_x + confirm_button_width &&
//         event->y >= confirm_button_y &&
//         event->y <= confirm_button_y + confirm_button_height) {
        
//         g_print("Confirm button clicked!\n");
        
//         // Dùng snprintf để tránh warning buffer overflow
//         snprintf(buffer, sizeof(buffer), "GET ORDERED SEATS %s", detail_flight.flight_id);
        
//         send(sock, buffer, strlen(buffer), 0);
//         int bytes_received = recv(sock, buffer, sizeof(buffer), 0);
        
//         if (bytes_received > 0) {
//             buffer[bytes_received] = '\0';
//             if (strcmp(buffer, "NOTHING") == 0){
//                 g_print("No ordered seats.\n");
//                 // Reset mảng ghế nếu không có ghế nào đặt trước
//                 seat_count = 0;
//                 if(seats_array) { free(seats_array); seats_array = NULL; }
                
//                 GtkWidget *book_seat_window = create_book_seat_window();
//                 set_content(book_seat_window);
//                 return TRUE;
//             }
//             sscanf(buffer, "SEAT COUNT: %d", &seat_count); 
//         }
        
//         g_print("Start receiving seats...\n");
        
//         // Cấp phát lại bộ nhớ cho mảng ghế
//         if (seats_array) free(seats_array);
//         seats_array = malloc(seat_count * sizeof(char *));
        
//         bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
//         if (bytes_received <= 0) {
//             perror("Error receiving seats data");
//             return TRUE;
//         }
//         buffer[bytes_received] = '\0'; 
//         printf("Received seats string: %s\n", buffer);

//         char *token = strtok(buffer, ",");
//         int i = 0;
//         while (token && i < seat_count) {
//             seats_array[i] = strdup(token);
//             if (!seats_array[i]) {
//                 perror("Error duplicating seat string");
//                 break;
//             }
//             token = strtok(NULL, ",");
//             i++;
//         }

//         printf("Parsed seats: %d\n", i);
//         GtkWidget *book_seat_window = create_book_seat_window();
//         set_content(book_seat_window);
//     }

//     return TRUE; 
// }

// // --- HÀM VẼ UI (ĐƯỢC LÀM ĐẸP LẠI) ---
// static gboolean on_ticket_detail_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
//     (void)user_data;
//     char date[20];
//     char time[20];
//     split_date_time(detail_flight.departure_time, date, time);

//     screen_width = gtk_widget_get_allocated_width(widget);
//     screen_height = gtk_widget_get_allocated_height(widget);

//     // 1. Vẽ lớp phủ mờ (Dim background) để làm nổi bật vé
//     // Vẽ nền Gradient nhẹ phía sau
//     cairo_pattern_t *pat = cairo_pattern_create_linear(0.0, 0.0, screen_width, screen_height);
//     cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.95, 0.96, 0.98);  
//     cairo_pattern_add_color_stop_rgb(pat, 1.0, 0.85, 0.87, 0.92);  
//     cairo_set_source(cr, pat);
//     cairo_paint(cr);
//     cairo_pattern_destroy(pat);

//     // 2. Tính toán vị trí Card (Căn giữa)
//     double x = (screen_width - CARD_WIDTH) / 2.0;
//     double y = (screen_height - CARD_HEIGHT) / 2.0;
//     double radius = 20.0;

//     // 3. Vẽ Bóng đổ (Shadow) cho Card
//     cairo_save(cr);
//     cairo_set_source_rgba(cr, 0, 0, 0, 0.15); // Màu đen mờ
//     // Vẽ hình chữ nhật bo tròn lệch xuống dưới một chút để làm bóng
//     cairo_new_path(cr);
//     cairo_arc(cr, x + radius + 5, y + radius + 10, radius, M_PI, 3 * M_PI / 2);
//     cairo_arc(cr, x + CARD_WIDTH - radius + 5, y + radius + 10, radius, 3 * M_PI / 2, 2 * M_PI);
//     cairo_arc(cr, x + CARD_WIDTH - radius + 5, y + CARD_HEIGHT - radius + 10, radius, 0, M_PI / 2);
//     cairo_arc(cr, x + radius + 5, y + CARD_HEIGHT - radius + 10, radius, M_PI / 2, M_PI);
//     cairo_close_path(cr);
//     cairo_fill(cr);
//     cairo_restore(cr);

//     // 4. Vẽ nền Card (Màu trắng)
//     cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
//     cairo_new_path(cr);
//     cairo_arc(cr, x + radius, y + radius, radius, M_PI, 3 * M_PI / 2);
//     cairo_arc(cr, x + CARD_WIDTH - radius, y + radius, radius, 3 * M_PI / 2, 2 * M_PI);
//     cairo_arc(cr, x + CARD_WIDTH - radius, y + CARD_HEIGHT - radius, radius, 0, M_PI / 2);
//     cairo_arc(cr, x + radius, y + CARD_HEIGHT - radius, radius, M_PI / 2, M_PI);
//     cairo_close_path(cr);
//     cairo_fill(cr);

//     // 5. Vẽ Header của Card (Logo Airline)
//     GdkPixbuf *airline_logo = gdk_pixbuf_new_from_file("../assets/images/airline.png", NULL);
//     if (airline_logo) {
//         // Vẽ logo nhỏ gọn hơn ở giữa phía trên
//         int logo_w = 200;
//         int logo_h = 50;
//         GdkPixbuf *scaled_logo = gdk_pixbuf_scale_simple(airline_logo, logo_w, logo_h, GDK_INTERP_BILINEAR);
//         gdk_cairo_set_source_pixbuf(cr, scaled_logo, x + (CARD_WIDTH - logo_w) / 2, y + 30);
//         cairo_paint(cr);
//         g_object_unref(scaled_logo);
//         g_object_unref(airline_logo);
//     }

//     // 6. Vẽ đường kẻ phân cách (Nét đứt)
//     cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
//     cairo_set_line_width(cr, 2.0);
//     double dash[] = {10.0};
//     cairo_set_dash(cr, dash, 1, 0);
    
//     cairo_move_to(cr, x + 40, y + 100);
//     cairo_line_to(cr, x + CARD_WIDTH - 40, y + 100);
//     cairo_stroke(cr);
    
//     cairo_set_dash(cr, NULL, 0, 0); // Reset dash

//     // 7. Vẽ thông tin chuyến bay (Điểm đi - Điểm đến)
//     cairo_select_font_face(cr, "Segoe UI", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    
//     // --- Điểm đi ---
//     cairo_set_source_rgb(cr, 0.2, 0.2, 0.2); // Màu đen
//     cairo_set_font_size(cr, 32);
//     cairo_move_to(cr, x + 100, y + 160);
//     cairo_show_text(cr, extract_middle_string(detail_flight.departure_airport)); // Mã sân bay to (SGN)

//     cairo_set_source_rgb(cr, 0.5, 0.5, 0.5); // Màu xám
//     cairo_set_font_size(cr, 16);
//     cairo_move_to(cr, x + 100, y + 190);
//     cairo_show_text(cr, "Departure City");

//     // --- Điểm đến ---
//     cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
//     cairo_set_font_size(cr, 32);
//     // Căn phải cho điểm đến
//     cairo_text_extents_t extents;
//     cairo_text_extents(cr, extract_middle_string(detail_flight.arrival_airport), &extents);
//     cairo_move_to(cr, x + CARD_WIDTH - 100 - extents.width, y + 160);
//     cairo_show_text(cr, extract_middle_string(detail_flight.arrival_airport)); // Mã sân bay to (HAN)

//     cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
//     cairo_set_font_size(cr, 16);
//     cairo_text_extents(cr, "Arrival City", &extents);
//     cairo_move_to(cr, x + CARD_WIDTH - 100 - extents.width, y + 190);
//     cairo_show_text(cr, "Arrival City");

//     // --- Icon máy bay ở giữa ---
//     GdkPixbuf *flight_icon = gdk_pixbuf_new_from_file("../assets/images/flight-icon.png", NULL);
//     if (flight_icon) {
//         GdkPixbuf *scaled_icon = gdk_pixbuf_scale_simple(flight_icon, 60, 60, GDK_INTERP_BILINEAR);
//         gdk_cairo_set_source_pixbuf(cr, scaled_icon, x + (CARD_WIDTH - 60) / 2, y + 130);
//         cairo_paint(cr);
//         g_object_unref(scaled_icon);
//         g_object_unref(flight_icon);
//     }

//     // 8. Vẽ thông tin chi tiết (Date, Time, Price)
//     double info_y = y + 250;
    
//     // Label Date
//     cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
//     cairo_set_font_size(cr, 14);
//     cairo_move_to(cr, x + 100, info_y);
//     cairo_show_text(cr, "DATE");
//     cairo_move_to(cr, x + 350, info_y);
//     cairo_show_text(cr, "TIME");
//     cairo_move_to(cr, x + 600, info_y);
//     cairo_show_text(cr, "FLIGHT NO");

//     // Value Date
//     cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
//     cairo_set_font_size(cr, 20);
//     cairo_move_to(cr, x + 100, info_y + 30);
//     cairo_show_text(cr, date);
    
//     cairo_move_to(cr, x + 350, info_y + 30);
//     cairo_show_text(cr, time);

//     cairo_move_to(cr, x + 600, info_y + 30);
//     cairo_show_text(cr, detail_flight.flight_id);

//     // --- Đường kẻ ngang thứ 2 ---
//     cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
//     cairo_set_line_width(cr, 1.0);
//     cairo_move_to(cr, x + 40, info_y + 60);
//     cairo_line_to(cr, x + CARD_WIDTH - 40, info_y + 60);
//     cairo_stroke(cr);

//     // 9. Hiển thị Giá tiền (To và Nổi bật)
//     double price_y = info_y + 110;
//     cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
//     cairo_set_font_size(cr, 18);
//     cairo_move_to(cr, x + 100, price_y);
//     cairo_show_text(cr, "TOTAL PRICE");

//     cairo_set_source_rgb(cr, 1.0, 0.4, 0.0); // Màu Cam đậm (#FF6600)
//     cairo_select_font_face(cr, "Segoe UI", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
//     cairo_set_font_size(cr, 36);
    
//     char price_str[64];
//     snprintf(price_str, sizeof(price_str), "%s VND", format_number_with_separator(detail_flight.price, ','));
//     cairo_move_to(cr, x + 100, price_y + 40);
//     cairo_show_text(cr, price_str);

//     // 10. Vẽ Nút Bấm (Cancel & Confirm)
//     // Nút Cancel (Màu xám nhạt)
//     double btn_cancel_x = x + (CARD_WIDTH - 344) / 2.0;
//     double btn_y = y + 500;
//     double btn_w = 156;
//     double btn_h = 56;
//     double btn_radius = 28; // Bo tròn hoàn toàn (Pill shape)

//     // Vẽ nền Cancel
//     cairo_set_source_rgb(cr, 0.9, 0.9, 0.9); // Xám nhạt
//     cairo_new_path(cr);
//     cairo_arc(cr, btn_cancel_x + btn_radius, btn_y + btn_radius, btn_radius, M_PI, 3 * M_PI / 2);
//     cairo_arc(cr, btn_cancel_x + btn_w - btn_radius, btn_y + btn_radius, btn_radius, 3 * M_PI / 2, 2 * M_PI);
//     cairo_arc(cr, btn_cancel_x + btn_w - btn_radius, btn_y + btn_h - btn_radius, btn_radius, 0, M_PI / 2);
//     cairo_arc(cr, btn_cancel_x + btn_radius, btn_y + btn_h - btn_radius, btn_radius, M_PI / 2, M_PI);
//     cairo_close_path(cr);
//     cairo_fill(cr);

//     // Chữ Cancel
//     cairo_set_source_rgb(cr, 0.4, 0.4, 0.4); // Chữ xám đậm
//     cairo_set_font_size(cr, 16);
//     cairo_text_extents_t ext;
//     cairo_text_extents(cr, "Cancel", &ext);
//     cairo_move_to(cr, btn_cancel_x + (btn_w - ext.width)/2, btn_y + (btn_h + ext.height)/2);
//     cairo_show_text(cr, "Cancel");

//     // Nút Confirm (Gradient Cam)
//     double btn_confirm_x = btn_cancel_x + 180;
    
//     // Vẽ nền Confirm (Gradient)
//     cairo_pattern_t *pat_btn = cairo_pattern_create_linear(btn_confirm_x, btn_y, btn_confirm_x + btn_w, btn_y);
//     cairo_pattern_add_color_stop_rgb(pat_btn, 0.0, 1.0, 0.73, 0.35); // #FFBA5A
//     cairo_pattern_add_color_stop_rgb(pat_btn, 1.0, 1.0, 0.37, 0.43); // #FF5F6D
//     cairo_set_source(cr, pat_btn);

//     cairo_new_path(cr);
//     cairo_arc(cr, btn_confirm_x + btn_radius, btn_y + btn_radius, btn_radius, M_PI, 3 * M_PI / 2);
//     cairo_arc(cr, btn_confirm_x + btn_w - btn_radius, btn_y + btn_radius, btn_radius, 3 * M_PI / 2, 2 * M_PI);
//     cairo_arc(cr, btn_confirm_x + btn_w - btn_radius, btn_y + btn_h - btn_radius, btn_radius, 0, M_PI / 2);
//     cairo_arc(cr, btn_confirm_x + btn_radius, btn_y + btn_h - btn_radius, btn_radius, M_PI / 2, M_PI);
//     cairo_close_path(cr);
//     cairo_fill(cr);
//     cairo_pattern_destroy(pat_btn);

//     // Chữ Confirm
//     cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // Chữ trắng
//     cairo_text_extents(cr, "Confirm", &ext);
//     cairo_move_to(cr, btn_confirm_x + (btn_w - ext.width)/2, btn_y + (btn_h + ext.height)/2);
//     cairo_show_text(cr, "Confirm");

//     return FALSE;
// }

// GtkWidget* create_ticket_detail_window() {
//     GtkWidget *drawing_area, *main_box;
//     main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
//     drawing_area = gtk_drawing_area_new();
//     g_signal_connect(drawing_area, "draw", G_CALLBACK(on_ticket_detail_draw), NULL);

//     gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
//     g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_mouse_click_ticket_detail), NULL);
    
//     gtk_box_pack_start(GTK_BOX(main_box), drawing_area, TRUE, TRUE, 0);
//     return main_box;
// }
#include <gtk/gtk.h>
#include "ticket_detail.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "../global/global.h"
#include "../list/list.h"
#include "../book_seat/book_seat.h"

/* ============================================
   CONSTANTS & GLOBALS
   ============================================ */
#define CARD_W 600       
#define CARD_H 750       
#define BTN_H 55        
#define THEME_BLUE_R 0.0
#define THEME_BLUE_G 0.48
#define THEME_BLUE_B 1.0

gint screen_width;
gint screen_height;
cairo_text_extents_t cancel_extents; 

/* ============================================
   HELPER FUNCTIONS (DRAWING)
   ============================================ */

// Helper: Vẽ hình chữ nhật bo góc
void draw_rounded_rect(cairo_t *cr, double x, double y, double w, double h, double r) {
    cairo_new_path(cr);
    cairo_arc(cr, x + w - r, y + r, r, -M_PI / 2, 0);
    cairo_arc(cr, x + w - r, y + h - r, r, 0, M_PI / 2);
    cairo_arc(cr, x + r, y + h - r, r, M_PI / 2, M_PI);
    cairo_arc(cr, x + r, y + r, r, M_PI, 3 * M_PI / 2);
    cairo_close_path(cr);
}

// Helper: Vẽ Barcode
void draw_barcode(cairo_t *cr, double x, double y, double w, double h) {
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    double current_x = x;
    srand(123); // Seed cố định
    while (current_x < x + w) {
        double line_w = (rand() % 3 + 1) * 2.0;
        if (current_x + line_w > x + w) break;
        cairo_rectangle(cr, current_x, y, line_w, h);
        cairo_fill(cr);
        current_x += line_w + (rand() % 3 + 1) * 2.0;
    }
}

// Helper: Lấy mã sân bay (3 ký tự đầu tiên)
void get_airport_code(const char* src, char* dest) {
    if (!src) { strcpy(dest, "---"); return; }
    int i = 0;
    while (src[i] != '\0' && src[i] != ' ' && i < 10) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

/* ============================================
   EVENT HANDLERS
   ============================================ */

gboolean on_mouse_click_ticket_detail(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    (void)widget; (void)user_data;
    
    double card_x = (screen_width - CARD_W) / 2.0;
    double card_y = (screen_height - CARD_H) / 2.0;

    double confirm_y = card_y + CARD_H - 40 - BTN_H;
    double confirm_x = card_x + 60;
    double confirm_w = CARD_W - 120;
    double cancel_y = confirm_y - 40;
    
    // 1. CONFIRM CLICK
    if (event->x >= confirm_x && event->x <= confirm_x + confirm_w &&
        event->y >= confirm_y && event->y <= confirm_y + BTN_H) {
        
        g_print("Confirm clicked!\n");
        snprintf(buffer, sizeof(buffer), "GET ORDERED SEATS %s", detail_flight.flight_id);
        send(sock, buffer, strlen(buffer), 0);
        
        int bytes_received = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            if (strcmp(buffer, "NOTHING") == 0){
                seat_count = 0;
                if(seats_array) { free(seats_array); seats_array = NULL; }
                
                GtkWidget *book_seat_window = create_book_seat_window();
                set_content(book_seat_window);
                return TRUE;
            }
            sscanf(buffer, "SEAT COUNT: %d", &seat_count);
        }
        
        // Nhận danh sách ghế
        if (seats_array) free(seats_array);
        seats_array = malloc(seat_count * sizeof(char *));
        
        bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            perror("Error receiving seats data");
            return TRUE;
        }
        buffer[bytes_received] = '\0';
        
        char *token = strtok(buffer, ",");
        int i = 0;
        while (token && i < seat_count) {
            seats_array[i] = strdup(token);
            token = strtok(NULL, ",");
            i++;
        }
        
        GtkWidget *book_seat_window = create_book_seat_window();
        set_content(book_seat_window);
        return TRUE;
    }

    // 2. CANCEL CLICK
    if (event->x >= card_x && event->x <= card_x + CARD_W &&
        event->y >= cancel_y && event->y <= cancel_y + 30) {
        GtkWidget *list_window = create_list_window();
        set_content(list_window);
        return TRUE;
    }

    return TRUE;
}

/* ============================================
   DRAWING FUNCTION (VERTICAL TICKET ON BLUE GRADIENT)
   ============================================ */

static gboolean on_ticket_detail_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    (void)user_data;
    screen_width = gtk_widget_get_allocated_width(widget);
    screen_height = gtk_widget_get_allocated_height(widget);
    
    char date_str[20], time_str[20];
    split_date_time(detail_flight.departure_time, date_str, time_str);

    // 1. VẼ NỀN GRADIENT TÍM/XANH (Thay vì màu xám cũ)
    cairo_pattern_t *pat = cairo_pattern_create_linear(0.0, 0.0, screen_width, screen_height);
    cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.18, 0.19, 0.57); // Tím đậm (#2E3192)
    cairo_pattern_add_color_stop_rgb(pat, 1.0, 0.11, 1.0, 1.0);   // Xanh Cyan (#1BFFFF)
    cairo_set_source(cr, pat);
    cairo_paint(cr);
    cairo_pattern_destroy(pat);

    // Center Card
    double cx = (screen_width - CARD_W) / 2.0;
    double cy = (screen_height - CARD_H) / 2.0;
    double r = 30.0;

    // 2. Bóng đổ (Shadow)
    cairo_save(cr);
    draw_rounded_rect(cr, cx + 10, cy + 10, CARD_W, CARD_H, r);
    cairo_set_source_rgba(cr, 0, 0, 0, 0.25); // Bóng đen mờ
    cairo_fill(cr);
    cairo_restore(cr);

    // 3. Thẻ Vé (Card Body - Màu trắng)
    cairo_save(cr);
    draw_rounded_rect(cr, cx, cy, CARD_W, CARD_H, r);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_fill(cr);
    cairo_restore(cr);

    // 4. HEADER (Mã Sân Bay To)
    double content_y = cy + 50;
    double margin_x = 50.0;

    char dep_code[20];
    char arr_code[20];
    get_airport_code(detail_flight.departure_airport, dep_code); 
    get_airport_code(detail_flight.arrival_airport, arr_code);   

    cairo_select_font_face(cr, "Segoe UI", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    
    // Trái (Dep Code)
    cairo_set_font_size(cr, 60); 
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.3); // Màu tối
    cairo_move_to(cr, cx + margin_x, content_y + 50);
    cairo_show_text(cr, dep_code);

    // Tên sân bay đi
    cairo_set_font_size(cr, 16);
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_move_to(cr, cx + margin_x, content_y + 80);
    cairo_show_text(cr, extract_middle_string(detail_flight.departure_airport));

    // Phải (Arr Code)
    cairo_text_extents_t extents;
    cairo_select_font_face(cr, "Segoe UI", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 60);
    cairo_text_extents(cr, arr_code, &extents);
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.3);
    cairo_move_to(cr, cx + CARD_W - margin_x - extents.width, content_y + 50);
    cairo_show_text(cr, arr_code);

    // Tên sân bay đến
    cairo_set_font_size(cr, 16);
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    char *arr_name = extract_middle_string(detail_flight.arrival_airport);
    cairo_text_extents(cr, arr_name, &extents);
    cairo_move_to(cr, cx + CARD_W - margin_x - extents.width, content_y + 80);
    cairo_show_text(cr, arr_name);

    // Giữa (Icon máy bay)
    double mid_y = content_y + 25;
    GdkPixbuf *icon = gdk_pixbuf_new_from_file("../assets/images/flight-icon.png", NULL);
    if(icon) {
        GdkPixbuf *s_icon = gdk_pixbuf_scale_simple(icon, 50, 50, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, s_icon, cx + CARD_W/2 - 25, mid_y - 25);
        cairo_paint(cr);
        g_object_unref(s_icon); g_object_unref(icon);
    }

    // 5. GRID INFO (Thông tin chi tiết)
    double grid_y = content_y + 150;
    double col1_x = cx + margin_x;
    double col2_x = cx + CARD_W / 2 + 40;
    double row_h = 80; 

    void draw_info_pair(cairo_t *c, const char* lbl, const char* val, double x, double y) {
        cairo_set_source_rgb(c, 0.6, 0.6, 0.6);
        cairo_select_font_face(c, "Segoe UI", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(c, 14);
        cairo_move_to(c, x, y);
        cairo_show_text(c, lbl);

        cairo_set_source_rgb(c, 0.15, 0.15, 0.15);
        cairo_select_font_face(c, "Segoe UI", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(c, 22);
        cairo_move_to(c, x, y + 30);
        cairo_show_text(c, val);
    }

    draw_info_pair(cr, "Passenger", "GUEST USER", col1_x, grid_y);
    draw_info_pair(cr, "Flight No", detail_flight.flight_id, col2_x, grid_y);

    draw_info_pair(cr, "Date", date_str, col1_x, grid_y + row_h);
    draw_info_pair(cr, "Time", time_str, col2_x, grid_y + row_h);

    char price_txt[50];
    snprintf(price_txt, 50, "%s VND", format_number_with_separator(detail_flight.price, ','));
    draw_info_pair(cr, "Class", "Economy", col1_x, grid_y + row_h * 2);
    draw_info_pair(cr, "Total Price", price_txt, col2_x, grid_y + row_h * 2);

    // 6. TEAR LINE (Đường xé vé)
    double tear_y = grid_y + row_h * 3 + 30;
    
    // Lỗ tròn (Phải tô màu Gradient trùng với nền để tạo cảm giác trong suốt)
    // Vì nền là gradient, ta vẽ lại gradient bên trong lỗ tròn (hoặc đơn giản là tô màu trung bình của gradient tại vị trí đó)
    // Ở đây để đơn giản, ta tô màu nền giả lập (hoặc clip nếu muốn xịn hơn).
    // Tô màu tím xanh đậm trung bình
    cairo_set_source_rgb(cr, 0.3, 0.6, 0.8); 
    cairo_arc(cr, cx, tear_y, 25, 0, 2 * M_PI); 
    cairo_fill(cr);
    cairo_arc(cr, cx + CARD_W, tear_y, 25, 0, 2 * M_PI); 
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
    cairo_set_line_width(cr, 2.0);
    const double dashes[] = {8.0};
    cairo_set_dash(cr, dashes, 1, 0);
    cairo_move_to(cr, cx + 30, tear_y);
    cairo_line_to(cr, cx + CARD_W - 30, tear_y);
    cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0);

    // 7. BARCODE
    double barcode_y = tear_y + 40;
    draw_barcode(cr, cx + 60, barcode_y, CARD_W - 120, 50);

    // 8. BUTTONS
    double btn_y = cy + CARD_H - 30 - BTN_H;
    double btn_x = cx + 60;
    double btn_w = CARD_W - 120;

    // Cancel Link
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_select_font_face(cr, "Segoe UI", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 16);
    cairo_text_extents(cr, "Cancel Selection", &extents);
    cairo_move_to(cr, cx + (CARD_W - extents.width)/2, btn_y - 20);
    cairo_show_text(cr, "Cancel Selection");

    // Confirm Button (Gradient Cam/Hồng)
    cairo_pattern_t *pat_btn = cairo_pattern_create_linear(btn_x, btn_y, btn_x + btn_w, btn_y);
    cairo_pattern_add_color_stop_rgb(pat_btn, 0.0, 1.0, 0.73, 0.35); // #FFBA5A
    cairo_pattern_add_color_stop_rgb(pat_btn, 1.0, 1.0, 0.37, 0.43); // #FF5F6D
    
    cairo_new_path(cr);
    draw_rounded_rect(cr, btn_x, btn_y, btn_w, BTN_H, BTN_H/2);
    cairo_set_source(cr, pat_btn);
    cairo_fill(cr);
    cairo_pattern_destroy(pat_btn);

    // Button Text
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_set_font_size(cr, 18);
    const char* btn_text = "CONFIRM BOOKING";
    cairo_text_extents(cr, btn_text, &extents);
    cairo_move_to(cr, btn_x + (btn_w - extents.width)/2, btn_y + (BTN_H + extents.height)/2 - 5);
    cairo_show_text(cr, btn_text);

    return FALSE;
}

/* ============================================
   MAIN WINDOW CREATION
   ============================================ */

GtkWidget* create_ticket_detail_window() {
    // Không cần load CSS ngoài vì đã vẽ bằng Cairo
    GtkWidget *drawing_area, *main_box;
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    drawing_area = gtk_drawing_area_new();
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_ticket_detail_draw), NULL);
    gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_mouse_click_ticket_detail), NULL);
    
    gtk_box_pack_start(GTK_BOX(main_box), drawing_area, TRUE, TRUE, 0);
    
    return main_box;
}