// #include "booklist.h"
// #include <cairo.h>
// #include <cairo-pdf.h>
// #include <gtk/gtk.h>
// #include <math.h>
// #include "../global/global.h"
// #include "../component/component.h"


// typedef struct {         
//     Ticket *tickets;
//     int *ticket_count;
//     int index;
// } ConfirmParams;

// char date[20];
// char time_flight[20];

// static void show_contact_dialog(GtkWidget *parent) {
//     // Tạo một dialog
//     GtkWidget *dialog = gtk_dialog_new_with_buttons(
//         "Contact Information", // Tiêu đề
//         GTK_WINDOW(gtk_widget_get_toplevel(parent)),
//         GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
//         "_Close", GTK_RESPONSE_CLOSE,
//         NULL);

//     // Tạo nội dung thông báo
//     const char *message = 
//         "For assistance, please contact our ticket support team:\n\n"
//         "📧 Email: TicketTrail@gmail.com\n"
//         "📞 Phone: +1-800-555-1234\n\n"
//         "We are available 24/7 to assist you with your queries. Thank you for choosing TicketTrail.";

//     GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
//     GtkWidget *label = gtk_label_new(message);

//     // Cài đặt font chữ và căn chỉnh
//     PangoAttrList *attr_list = pango_attr_list_new();
//     PangoAttribute *font_attr = pango_attr_size_new_absolute(18 * PANGO_SCALE); // Font size 18px
//     pango_attr_list_insert(attr_list, font_attr);
//     gtk_label_set_attributes(GTK_LABEL(label), attr_list);
//     pango_attr_list_unref(attr_list);

//     gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
//     gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);

//     // Thêm label vào dialog
//     gtk_box_pack_start(GTK_BOX(content_area), label, TRUE, TRUE, 10);

//     // Hiển thị các thành phần của dialog
//     gtk_widget_show_all(dialog);

//     g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
// }

// static gboolean execute_on_main_thread(gpointer user_data) {
//     on_cancel(NULL, user_data);
//     return FALSE; 
// }

// void on_cancel(GtkWidget *widget, gpointer user_data){
//     g_print("Cancel button clicked");
//     ConfirmParams *params = (ConfirmParams *)user_data;
//     g_print("Booking check: %d\n", list_tickets[params->index].booking_id);
//     int booking_id = 10;
//     snprintf(buffer, sizeof(buffer), "DELETE BOOKING: %d", booking_id);
//     send(sock, buffer, strlen(buffer) + 1, 0); 

//     memset(buffer, 0, sizeof(buffer));
//     recv(sock, buffer, sizeof(buffer), 0);
//     printf("Response from server: %s\n", buffer);
// }


// static void draw_ticket(cairo_t *cr, double ticket_x, double ticket_y, double ticket_width, double ticket_height, const Ticket *ticket) {
//     split_date_time(ticket->departure_time, date, time_flight);
//     cairo_set_source_rgb(cr, 0.92, 0.94, 0.94); 
//     cairo_new_path(cr);
//     cairo_arc(cr, ticket_x + 24, ticket_y + 24, 24, M_PI, 3 * M_PI / 2);
//     cairo_arc(cr, ticket_x + ticket_width - 24, ticket_y + 24, 24, 3 * M_PI / 2, 2 * M_PI);
//     cairo_arc(cr, ticket_x + ticket_width - 24, ticket_y + ticket_height - 24, 24, 0, M_PI / 2);
//     cairo_arc(cr, ticket_x + 24, ticket_y + ticket_height - 24, 24, M_PI / 2, M_PI);
//     cairo_close_path(cr);
//     cairo_fill(cr);

    
//     double ticket_left_margin = 50;
//     double flight_icon_width = 156;
//     double flight_icon_x = ticket_x + ticket_left_margin + 120; 
//     double flight_icon_y = ticket_y + 50;

//     double departure_city_x = ticket_x + ticket_left_margin;
//     double departure_city_y = flight_icon_y + 10;
//     double arrival_city_x = flight_icon_x + flight_icon_width + 50; 
//     double arrival_city_y = departure_city_y;

//     cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
//     cairo_set_font_size(cr, 20);

    
//     cairo_move_to(cr, departure_city_x, departure_city_y);
//     cairo_show_text(cr,extract_middle_string(ticket->departure_airport));

    
//     cairo_move_to(cr, arrival_city_x, arrival_city_y);
//     cairo_show_text(cr, extract_middle_string(ticket->arrival_airport));

    
//     double departure_airport_y = departure_city_y + 30;
//     double arrival_airport_y = arrival_city_y + 30;

//     cairo_set_source_rgb(cr, 0.4, 0.4, 0.4); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
//     cairo_set_font_size(cr, 18);

//     cairo_move_to(cr, departure_city_x, departure_airport_y);
//     cairo_show_text(cr, extract_middle_string(ticket->departure_airport));

//     cairo_move_to(cr, arrival_city_x, arrival_airport_y);
//     cairo_show_text(cr, extract_middle_string(ticket->arrival_airport));

    
//     GdkPixbuf *flight_icon = gdk_pixbuf_new_from_file("../assets/images/flight-icon.png", NULL);
//     if (flight_icon) {
//         GdkPixbuf *scaled_flight_icon = gdk_pixbuf_scale_simple(flight_icon, flight_icon_width, 36, GDK_INTERP_BILINEAR);
//         gdk_cairo_set_source_pixbuf(cr, scaled_flight_icon, flight_icon_x, flight_icon_y);
//         cairo_paint(cr);
//         g_object_unref(scaled_flight_icon);
//         g_object_unref(flight_icon);
//     }

    
//     double icon_size = 20;
//     double date_time_start_x = ticket_x + ticket_left_margin + 70;
//     double current_y = ticket_y + 130;

//     GdkPixbuf *calendar_icon = gdk_pixbuf_new_from_file("../assets/images/calendar.png", NULL);
//     if (calendar_icon) {
//         GdkPixbuf *scaled_calendar_icon = gdk_pixbuf_scale_simple(calendar_icon, icon_size, icon_size, GDK_INTERP_BILINEAR);
//         gdk_cairo_set_source_pixbuf(cr, scaled_calendar_icon, date_time_start_x, current_y);
//         cairo_paint(cr);
//         g_object_unref(scaled_calendar_icon);
//         g_object_unref(calendar_icon);
//     }

//     cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
//     cairo_set_font_size(cr, 18);
//     cairo_move_to(cr, date_time_start_x + 30, current_y + 15);
//     cairo_show_text(cr, date);

//     GdkPixbuf *clock_icon = gdk_pixbuf_new_from_file("../assets/images/Clock.png", NULL);
//     if (clock_icon) {
//         GdkPixbuf *scaled_clock_icon = gdk_pixbuf_scale_simple(clock_icon, icon_size, icon_size, GDK_INTERP_BILINEAR);
//         gdk_cairo_set_source_pixbuf(cr, scaled_clock_icon, date_time_start_x + 200, current_y);
//         cairo_paint(cr);
//         g_object_unref(scaled_clock_icon);
//         g_object_unref(clock_icon);
//     }

//     cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
//     cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
//     cairo_set_font_size(cr, 18);
//     cairo_move_to(cr, date_time_start_x + 230, current_y + 15);
//     cairo_show_text(cr, time_flight);

//     if (ticket->list_ticket[0] == '1'){
//         strcpy(class, "First class");
//     }
//     else if (ticket->list_ticket[0] == '2'){
//         strcpy(class, "Business");
//     }
//     else strcpy(class, "Economy");

//     const char *flight_labels[4] = {"Flight", "Gate", "Seat", "Class"};
//     const char *flight_info[4] = {ticket->flight_id, "22", ticket->list_ticket, class};

//     double base_x = ticket_x + 70;
//     current_y += 70;
//     for (int i = 0; i < 4; i++) {
//         cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
//         cairo_set_font_size(cr, 16);
//         cairo_move_to(cr, base_x + i * 150, current_y);
//         cairo_show_text(cr, flight_labels[i]);

//         cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
//         cairo_set_font_size(cr, 18);
//         cairo_move_to(cr, base_x + i * 150, current_y + 20);
//         cairo_show_text(cr, flight_info[i]);
//     }

    
//     cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); 
//     cairo_set_font_size(cr, 22);
//     cairo_move_to(cr, ticket_x + ticket_width - 350, ticket_y + 60);
//     cairo_show_text(cr, "Price:");

//     cairo_set_font_size(cr, 32);
//     cairo_move_to(cr, ticket_x + ticket_width - 280, ticket_y + 60);
//     cairo_show_text(cr, format_number_with_separator(ticket->total_price, ','));

    
//     double barcode_x = ticket_x + ticket_width - 350;
//     double barcode_y = ticket_y + 80;
//     double barcode_width = 320;
//     double barcode_height = 50;
//     int num_bars = 80;

//     srand(12345); 
//     for (int i = 0; i < num_bars; i++) {
//         double bar_x = barcode_x + i * (barcode_width / num_bars);
//         double bar_width = (rand() % 2 == 0) ? (barcode_width / num_bars) : (barcode_width / (num_bars * 1.2));
//         cairo_set_source_rgb(cr, 0, 0, 0); 
//         cairo_rectangle(cr, bar_x, barcode_y, bar_width, barcode_height);
//         cairo_fill(cr);
//     }
// }

// static void save_ticket_as_pdf(const char *filename, const Ticket *ticket) {
//     double ticket_width = 1000, ticket_height = 280;

//     cairo_surface_t *pdf_surface = cairo_pdf_surface_create(filename, ticket_width, ticket_height);
//     cairo_t *pdf_cr = cairo_create(pdf_surface);

//     draw_ticket(pdf_cr, 0, 0, ticket_width, ticket_height, ticket);

//     cairo_destroy(pdf_cr);
//     cairo_surface_destroy(pdf_surface);
// }

// static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
//     if (event->type == GDK_BUTTON_PRESS) {
//         Ticket *tickets = (Ticket *)user_data;
//         int ticket_counts = ticket_count;

//         gint screen_width = gtk_widget_get_allocated_width(widget);
//         double ticket_width = 1000, ticket_height = 280;

//         for (int i = 0; i < ticket_counts; ++i) {
//             double ticket_x = (screen_width - ticket_width) / 2;

            
//             double print_button_x = ticket_x + ticket_width - 350;
//             double print_button_y = 200 + i * (ticket_height + 50);

//             if (event->x >= print_button_x && event->x <= print_button_x + 320 &&
//                 event->y >= print_button_y && event->y <= print_button_y + 50) {
//                     GtkWidget *dialog = gtk_file_chooser_dialog_new(
//                     "Save Ticket",
//                     GTK_WINDOW(gtk_widget_get_toplevel(widget)),
//                     GTK_FILE_CHOOSER_ACTION_SAVE,
//                     "_Cancel", GTK_RESPONSE_CANCEL,
//                     "_Save", GTK_RESPONSE_ACCEPT,
//                     NULL);

//                 gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "Your_Flight_Ticket.pdf");

//                 if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
//                     char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
//                     save_ticket_as_pdf(filename, &tickets[i]);
//                     g_print("Ticket saved as: %s\n", filename);
//                     g_free(filename);
//                 }

//                 gtk_widget_destroy(dialog);
//                 return TRUE;
//             }
//             double cancel_button_x = ticket_x + ticket_width - 350;
//             double cancel_button_y = print_button_y + 70;

//             if (event->x >= cancel_button_x && event->x <= cancel_button_x + 150 &&
//                 event->y >= cancel_button_y && event->y <= cancel_button_y + 50) {
//                 ConfirmParams *params = g_new(ConfirmParams, 1); 
//                 params->tickets = tickets;
//                 params->ticket_count = &ticket_count;
//                 params->index = i;

//                 // Gọi trực tiếp hàm on_cancel
//                 g_print("Cancel button area clicked!\n");
//                 g_idle_add(execute_on_main_thread, params);

//                 return TRUE;
//             }

//              double change_button_x = ticket_x + ticket_width - 200;
//             double change_button_y = cancel_button_y;

//             if (event->x >= change_button_x && event->x <= change_button_x + 150 &&
//                 event->y >= change_button_y && event->y <= change_button_y + 50) {
//                 ConfirmParams *params = g_malloc(sizeof(ConfirmParams));
//                 params->tickets = tickets;
//                 params->ticket_count = &ticket_count;
//                 params->index = i;

//                 show_contact_dialog(widget);
//                 return TRUE;
//             }
//         }
//     }
//     return FALSE;
// }
// static gboolean on_booklist_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
//     gint screen_width = gtk_widget_get_allocated_width(widget);
//     gint screen_height = gtk_widget_get_allocated_height(widget);

    
//     // GdkPixbuf *bg_pixbuf = gdk_pixbuf_new_from_file("../assets/images/bg_login.png", NULL);
//     // if (bg_pixbuf) {
//     //     GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(bg_pixbuf, screen_width, screen_height, GDK_INTERP_BILINEAR);
//     //     gdk_cairo_set_source_pixbuf(cr, scaled_pixbuf, 0, 0);
//     //     cairo_paint(cr);
//     //     g_object_unref(scaled_pixbuf);
//     //     g_object_unref(bg_pixbuf);
//     // }

//     if (ticket_count == 0){
//         cairo_set_source_rgb(cr, 0, 0, 0);
//         cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
//         cairo_set_font_size(cr, 20);
//         cairo_move_to(cr, 50, 100);
//         cairo_show_text(cr, "You do not have any tickets");

//         return FALSE;
//     }

//     Ticket *tickets = (Ticket *)user_data; 
//     size_t ticket_counts = ticket_count;

//     double ticket_width = 1000;
//     double ticket_height = 280;
//     double ticket_x = (screen_width - ticket_width) / 2;
//     double vertical_spacing = 50; 

    
//     for (size_t i = 0; i < ticket_counts; ++i) {
//         double ticket_y = 50 + i * (ticket_height + vertical_spacing); 

        
//         draw_ticket(cr, ticket_x, ticket_y, ticket_width, ticket_height, &tickets[i]);

        
//         double button_width = 320;
//         double button_height = 50;
//         double button_gap = 20; 
//         double cancel_change_width = (button_width - button_gap) / 2;

//         double button_x = ticket_x + ticket_width - 350;

        
//         double print_button_y = ticket_y + 150; 
//         cairo_set_source_rgb(cr, 34.0 / 255.0, 58.0 / 255.0, 96.0 / 255.0); 
//         cairo_new_path(cr);
//         cairo_arc(cr, button_x + 8, print_button_y + 8, 8, M_PI, 3 * M_PI / 2);
//         cairo_arc(cr, button_x + button_width - 8, print_button_y + 8, 8, 3 * M_PI / 2, 2 * M_PI);
//         cairo_arc(cr, button_x + button_width - 8, print_button_y + button_height - 8, 8, 0, M_PI / 2);
//         cairo_arc(cr, button_x + 8, print_button_y + button_height - 8, 8, M_PI / 2, M_PI);
//         cairo_close_path(cr);
//         cairo_fill_preserve(cr);
//         cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
//         cairo_stroke(cr);

        
//         cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); 
//         cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
//         cairo_set_font_size(cr, 18);
//         cairo_text_extents_t print_text_extents;
//         cairo_text_extents(cr, "Print ticket out", &print_text_extents);
//         cairo_move_to(cr, button_x + (button_width - print_text_extents.width) / 2,
//                       print_button_y + (button_height + print_text_extents.height) / 2);
//         cairo_show_text(cr, "Print ticket out");

//         if (is_valid_date(date) == 0) {
            
//             double cancel_button_y = print_button_y + button_height + 20;
//             double cancel_button_x = button_x;

//             cairo_set_source_rgb(cr, 0.92, 0.94, 0.94); 
//             cairo_new_path(cr);
//             cairo_arc(cr, cancel_button_x + 8, cancel_button_y + 8, 8, M_PI, 3 * M_PI / 2);
//             cairo_arc(cr, cancel_button_x + cancel_change_width - 8, cancel_button_y + 8, 8, 3 * M_PI / 2, 2 * M_PI);
//             cairo_arc(cr, cancel_button_x + cancel_change_width - 8, cancel_button_y + button_height - 8, 8, 0, M_PI / 2);
//             cairo_arc(cr, cancel_button_x + 8, cancel_button_y + button_height - 8, 8, M_PI / 2, M_PI);
//             cairo_close_path(cr);
//             cairo_fill_preserve(cr);
//             cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
//             cairo_stroke(cr);

            
//             cairo_set_source_rgb(cr, 0.13, 0.23, 0.37);
//             cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
//             cairo_set_font_size(cr, 18);
//             cairo_text_extents_t cancel_text_extents;
//             cairo_text_extents(cr, "Cancel", &cancel_text_extents);
//             cairo_move_to(cr, cancel_button_x + (cancel_change_width - cancel_text_extents.width) / 2,
//                         cancel_button_y + (button_height + cancel_text_extents.height) / 2);
//             cairo_show_text(cr, "Cancel");

            
//             double change_button_x = cancel_button_x + cancel_change_width + button_gap;

//             cairo_set_source_rgb(cr, 0.92, 0.94, 0.94); 
//             cairo_new_path(cr);
//             cairo_arc(cr, change_button_x + 8, cancel_button_y + 8, 8, M_PI, 3 * M_PI / 2);
//             cairo_arc(cr, change_button_x + cancel_change_width - 8, cancel_button_y + 8, 8, 3 * M_PI / 2, 2 * M_PI);
//             cairo_arc(cr, change_button_x + cancel_change_width - 8, cancel_button_y + button_height - 8, 8, 0, M_PI / 2);
//             cairo_arc(cr, change_button_x + 8, cancel_button_y + button_height - 8, 8, M_PI / 2, M_PI);
//             cairo_close_path(cr);
//             cairo_fill_preserve(cr);
//             cairo_set_source_rgb(cr, 0.13, 0.23, 0.37); 
//             cairo_stroke(cr);

            
//             cairo_set_source_rgb(cr, 0.13, 0.23, 0.37);
//             cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
//             cairo_set_font_size(cr, 18);
//             cairo_text_extents_t change_text_extents;
//             cairo_text_extents(cr, "Change", &change_text_extents);
//             cairo_move_to(cr, change_button_x + (cancel_change_width - change_text_extents.width) / 2,
//                         cancel_button_y + (button_height + change_text_extents.height) / 2);
//             cairo_show_text(cr, "Change");
//         }
        
//     }

//     return FALSE;
// }

// GtkWidget *create_booklist_window() {
//     GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
//     gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

//     GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
//     gtk_container_add(GTK_CONTAINER(scrolled_window), main_box);

//     GtkWidget *buttons[4];
//     GtkWidget *header = create_header(buttons, scrolled_window);
//     gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 0);

//     GtkWidget *drawing_area = gtk_drawing_area_new();
//     gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
//     g_signal_connect(drawing_area, "draw", G_CALLBACK(on_booklist_draw), list_tickets);
//     g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_button_press), list_tickets);

//     gtk_widget_set_size_request(drawing_area, -1, 3 * (280 + 50 + 50));

//     gtk_box_pack_start(GTK_BOX(main_box), drawing_area, TRUE, TRUE, 0);

//     return scrolled_window;

// }

#include <gtk/gtk.h>
#include "booklist.h"
#include <cairo.h>
#include <cairo-pdf.h>
#include <math.h>
#include <string.h>
#include "../global/global.h"
#include "../component/component.h"
#include "../list/list.h" // Dùng chung logic header nếu cần
#include "../homepage/homepage.h"

// --- CSS STYLE (Đồng bộ với list.c) ---
void apply_booklist_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        // 1. Nền Gradient Tím Đậm
        "window { "
        "   background-image: linear-gradient(135deg, #2E3192, #1BFFFF); "
        "   font-family: 'Segoe UI', 'Montserrat', sans-serif; "
        "}\n"
        
        // 2. Header Buttons
        "#booklist-header-btn { "
        "   background: transparent; border: none; box-shadow: none; "
        "   color: rgba(255,255,255,0.8); font-weight: 900; font-size: 16px; "
        "   padding: 12px 25px; margin: 0 10px; "
        "   text-transform: uppercase; letter-spacing: 1px; "
        "   border-bottom: 3px solid transparent; "
        "}\n"
        "#booklist-header-btn:hover { color: #FFD700; text-shadow: 0 0 10px rgba(255, 215, 0, 0.6); }\n"
        "#booklist-header-btn:checked { color: #FFD700; border-bottom: 3px solid #FFD700; }\n"

        // 3. Booked Ticket Card
        ".booked-card { "
        "   background-color: #ffffff; "
        "   border-radius: 20px; "
        "   padding: 0; margin: 15px 40px; "
        "   box-shadow: 0 15px 35px rgba(0,0,0,0.2); "
        "}\n"
        ".booked-card:hover { transform: translateY(-3px); box-shadow: 0 20px 45px rgba(0,0,0,0.3); }\n"

        // 4. Typography
        ".airline-label { font-weight: 900; font-size: 18px; color: #223A60; }\n"
        ".time-huge { font-weight: 900; font-size: 30px; color: #2a0845; }\n"
        ".airport-code { "
        "   background-color: #f1f2f6; color: #666; font-weight: 700; font-size: 13px; "
        "   padding: 4px 8px; border-radius: 6px; "
        "}\n"
        ".price-booked { font-weight: 900; font-size: 24px; color: #27ae60; }\n" /* Màu xanh lá cho vé đã mua */
        ".seat-info { color: #555; font-weight: 600; font-size: 14px; }\n"

        // 5. Buttons (Print & Cancel)
        ".btn-print { "
        "   background-image: linear-gradient(to right, #3498db, #2980b9); "
        "   color: white; font-weight: bold; border-radius: 15px; border: none; "
        "}\n"
        ".btn-print:hover { transform: scale(1.05); box-shadow: 0 5px 15px rgba(52, 152, 219, 0.4); }\n"
        
        ".btn-cancel { "
        "   background-color: #ecf0f1; color: #7f8c8d; font-weight: bold; "
        "   border-radius: 15px; border: none; "
        "}\n"
        ".btn-cancel:hover { background-color: #bdc3c7; color: #c0392b; }\n"

        // 6. No Ticket Label
        ".no-ticket { color: white; font-size: 24px; font-weight: 300; opacity: 0.8; }\n"
        
        , -1, NULL);
    
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), 
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

// --- LOGIC PDF (Giữ lại logic vẽ PDF cũ nhưng tách biệt với UI) ---
static void draw_ticket_for_pdf(cairo_t *cr, double width, double height, const Ticket *ticket) {
    // Vẽ nền trắng
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);

    // Vẽ khung
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_set_line_width(cr, 2.0);
    cairo_rectangle(cr, 20, 20, width - 40, height - 40);
    cairo_stroke(cr);

    // Text info
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 24);
    cairo_move_to(cr, 50, 60);
    cairo_show_text(cr, "FLIGHT TICKET");

    cairo_set_font_size(cr, 18);
    cairo_move_to(cr, 50, 100);
    cairo_show_text(cr, ticket->airplane_name);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "From: %s", extract_middle_string(ticket->departure_airport));
    cairo_move_to(cr, 50, 140);
    cairo_show_text(cr, buffer);

    snprintf(buffer, sizeof(buffer), "To: %s", extract_middle_string(ticket->arrival_airport));
    cairo_move_to(cr, 50, 170);
    cairo_show_text(cr, buffer);

    snprintf(buffer, sizeof(buffer), "Date: %s", ticket->departure_time);
    cairo_move_to(cr, 50, 200);
    cairo_show_text(cr, buffer);

    snprintf(buffer, sizeof(buffer), "Price: %s VND", format_number_with_separator(ticket->total_price, ','));
    cairo_move_to(cr, 50, 230);
    cairo_show_text(cr, buffer);
    
    snprintf(buffer, sizeof(buffer), "Booking ID: %d", ticket->booking_id);
    cairo_move_to(cr, 400, 60);
    cairo_show_text(cr, buffer);
}

static void save_ticket_as_pdf(const char *filename, const Ticket *ticket) {
    cairo_surface_t *pdf_surface = cairo_pdf_surface_create(filename, 600, 300);
    cairo_t *pdf_cr = cairo_create(pdf_surface);
    draw_ticket_for_pdf(pdf_cr, 600, 300, ticket);
    cairo_destroy(pdf_cr);
    cairo_surface_destroy(pdf_surface);
}

// --- CALLBACKS ---
void on_print_clicked(GtkWidget *widget, gpointer data) {
    Ticket *ticket = (Ticket *)data;
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save Ticket",
                                                    GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Save", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "MyTicket.pdf");

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        save_ticket_as_pdf(filename, ticket);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

// Struct để truyền dữ liệu vào callback Cancel
typedef struct {
    int booking_id;
} CancelParams;

static gboolean execute_cancel_on_main_thread(gpointer user_data) {
    CancelParams *params = (CancelParams *)user_data;
    
    snprintf(buffer, sizeof(buffer), "DELETE BOOKING: %d", params->booking_id);
    send(sock, buffer, strlen(buffer) + 1, 0);
    recv(sock, buffer, sizeof(buffer), 0);
    
    // Refresh lại trang list vé sau khi xóa
    // Để đơn giản, ta gọi lại get_list_tickets_ordered và reload
    if (get_list_tickets_ordered() != -1) {
         GtkWidget *new_window = create_booklist_window();
         set_content(new_window);
    }
    
    g_free(params);
    return FALSE;
}

void on_cancel_clicked(GtkWidget *widget, gpointer data) {
    Ticket *ticket = (Ticket *)data;
    
    // Tạo dialog xác nhận
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                           GTK_DIALOG_MODAL,
                                           GTK_MESSAGE_QUESTION,
                                           GTK_BUTTONS_YES_NO,
                                           "Are you sure you want to cancel this booking?");
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
        CancelParams *params = g_new(CancelParams, 1);
        params->booking_id = ticket->booking_id;
        g_idle_add(execute_cancel_on_main_thread, params);
    }
    gtk_widget_destroy(dialog);
}

// --- UI COMPONENTS ---

// 1. Header: Tái sử dụng header 4 nút của trang List
GtkWidget* create_booklist_header(GtkWidget *main_box) {
    GtkWidget *buttons[5];
    GtkWidget *header = create_header(buttons, main_box);
    
    // Xóa logo cũ
    GList *children = gtk_container_get_children(GTK_CONTAINER(header));
    if (children) {
        gtk_widget_destroy(GTK_WIDGET(children->data));
        g_list_free(children);
    }

    gtk_widget_set_halign(header, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(header, 30);
    gtk_widget_set_size_request(header, 900, 60);

    // Đổi style nút
    GList *curr_children = gtk_container_get_children(GTK_CONTAINER(header));
    if (curr_children) {
        GtkWidget *menu_box = GTK_WIDGET(curr_children->data);
        if (GTK_IS_CONTAINER(menu_box)) {
            gtk_widget_set_halign(menu_box, GTK_ALIGN_CENTER);
            GList *btns = gtk_container_get_children(GTK_CONTAINER(menu_box));
            int i=0;
            for (GList *l = btns; l != NULL; l = l->next) {
                GtkWidget *btn = GTK_WIDGET(l->data);
                if (GTK_IS_BUTTON(btn)) {
                    gtk_widget_set_name(btn, "booklist-header-btn"); // CSS ID
                    if (i==0) gtk_button_set_label(GTK_BUTTON(btn), "HOME");
                    else if (i==1) {
                        gtk_button_set_label(GTK_BUTTON(btn), "MY BOOKINGS");
                        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btn), TRUE); // Active nút này
                    }
                    else if (i==2) gtk_button_set_label(GTK_BUTTON(btn), "MY ACCOUNT");
                    else if (i==3) gtk_button_set_label(GTK_BUTTON(btn), "HELP CENTER");
                    i++;
                }
            }
            g_list_free(btns);
        }
        g_list_free(curr_children);
    }
    return header;
}

// 2. Ticket Card: Phiên bản "Đã Đặt" (Booked)
GtkWidget* create_booked_card(Ticket *ticket) {
    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkStyleContext *ctx = gtk_widget_get_style_context(card);
    gtk_style_context_add_class(ctx, "booked-card");
    gtk_widget_set_size_request(card, 950, 140);

    // LEFT: Info
    GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(left_box, 30);
    gtk_widget_set_margin_top(left_box, 20);
    gtk_widget_set_margin_bottom(left_box, 20);

    // Logo + ID Vé
    GtkWidget *row_top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *icon = gtk_image_new_from_icon_name("airplane-mode-symbolic", GTK_ICON_SIZE_DND);
    GtkWidget *lbl_name = gtk_label_new(ticket->airplane_name);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_name), "airline-label");
    
    char id_buff[50];
    snprintf(id_buff, sizeof(id_buff), "Booking ID: #%d", ticket->booking_id);
    GtkWidget *lbl_id = gtk_label_new(id_buff);
    GdkRGBA grey; gdk_rgba_parse(&grey, "#7f8c8d");
    gtk_widget_override_color(lbl_id, GTK_STATE_FLAG_NORMAL, &grey);

    gtk_box_pack_start(GTK_BOX(row_top), icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row_top), lbl_name, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(row_top), lbl_id, FALSE, FALSE, 20);
    gtk_box_pack_start(GTK_BOX(left_box), row_top, FALSE, FALSE, 0);

    // Time Row
    GtkWidget *row_time = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_top(row_time, 15);

    char time_only[10], date_val[20];
    split_date_time(ticket->departure_time, date_val, time_only);
    
    // Dep
    GtkWidget *col_dep = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *lbl_dep = gtk_label_new(time_only);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_dep), "time-huge");
    GtkWidget *lbl_dep_code = gtk_label_new(extract_middle_string(ticket->departure_airport));
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_dep_code), "airport-code");
    gtk_box_pack_start(GTK_BOX(col_dep), lbl_dep, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(col_dep), lbl_dep_code, FALSE, FALSE, 0);

    // Arrow
    GtkWidget *col_mid = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(col_mid, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(col_mid, 150, -1);
    GtkWidget *lbl_date = gtk_label_new(date_val);
    GtkWidget *visual = gtk_label_new("---- ✈ ----");
    gtk_widget_override_color(visual, GTK_STATE_FLAG_NORMAL, &grey);
    gtk_box_pack_start(GTK_BOX(col_mid), lbl_date, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(col_mid), visual, FALSE, FALSE, 0);

    // Arr
    char *end_time_str = calculate_end_time(time_only, ticket->duration_minutes * 60);
    GtkWidget *col_arr = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *lbl_arr = gtk_label_new(end_time_str);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_arr), "time-huge");
    GtkWidget *lbl_arr_code = gtk_label_new(extract_middle_string(ticket->arrival_airport));
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_arr_code), "airport-code");
    gtk_box_pack_start(GTK_BOX(col_arr), lbl_arr, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(col_arr), lbl_arr_code, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(row_time), col_dep, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row_time), col_mid, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row_time), col_arr, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left_box), row_time, FALSE, FALSE, 0);
    
    // Seat Info
    char seat_buff[100];
    snprintf(seat_buff, sizeof(seat_buff), "Seats: %s", ticket->list_ticket);
    GtkWidget *lbl_seats = gtk_label_new(seat_buff);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_seats), "seat-info");
    gtk_widget_set_halign(lbl_seats, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(left_box), lbl_seats, FALSE, FALSE, 5);

    // RIGHT: Price & Action
    GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(right_box, 200, -1);
    gtk_widget_set_valign(right_box, GTK_ALIGN_CENTER);

    char price_buff[64];
    snprintf(price_buff, sizeof(price_buff), "%s VND", format_number_with_separator(ticket->total_price, ','));
    GtkWidget *lbl_price = gtk_label_new(price_buff);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_price), "price-booked");

    GtkWidget *btn_print = gtk_button_new_with_label("🖨 Print Ticket");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_print), "btn-print");
    gtk_widget_set_size_request(btn_print, 120, 35);
    g_signal_connect(btn_print, "clicked", G_CALLBACK(on_print_clicked), ticket);

    GtkWidget *btn_cancel = gtk_button_new_with_label("Cancel");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_cancel), "btn-cancel");
    gtk_widget_set_size_request(btn_cancel, 120, 30);
    g_signal_connect(btn_cancel, "clicked", G_CALLBACK(on_cancel_clicked), ticket);

    gtk_box_pack_start(GTK_BOX(right_box), lbl_price, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_box), btn_print, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_box), btn_cancel, FALSE, FALSE, 0);

    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_margin_top(separator, 15);
    gtk_widget_set_margin_bottom(separator, 15);

    gtk_box_pack_start(GTK_BOX(card), left_box, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(card), separator, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(card), right_box, FALSE, FALSE, 20);

    return card;
}

// --- LIST CONTAINER ---
GtkWidget* create_booked_list() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    
    if (ticket_count == 0) {
        GtkWidget *empty_lbl = gtk_label_new("You do not have any tickets.");
        GtkStyleContext *ctx = gtk_widget_get_style_context(empty_lbl);
        gtk_style_context_add_class(ctx, "no-ticket");
        gtk_box_pack_start(GTK_BOX(box), empty_lbl, TRUE, TRUE, 100);
        return box;
    }

    for (int i = 0; i < ticket_count; i++) {
        GtkWidget *card = create_booked_card(&list_tickets[i]);
        gtk_box_pack_start(GTK_BOX(box), card, FALSE, FALSE, 0);
    }
    return box;
}

GtkWidget* create_booklist_window() {
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(scrolled_window), main_box);

    apply_booklist_css();

    GtkWidget *header = create_booklist_header(main_box);
    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 0);

    GtkWidget *title = gtk_label_new("MY BOOKINGS");
    GdkRGBA white; gdk_rgba_parse(&white, "white");
    gtk_widget_override_color(title, GTK_STATE_FLAG_NORMAL, &white);
    PangoAttrList *attr = pango_attr_list_new();
    pango_attr_list_insert(attr, pango_attr_size_new_absolute(24 * PANGO_SCALE));
    pango_attr_list_insert(attr, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
    gtk_label_set_attributes(GTK_LABEL(title), attr);
    gtk_widget_set_margin_top(title, 20);
    gtk_widget_set_margin_bottom(title, 20);
    gtk_box_pack_start(GTK_BOX(main_box), title, FALSE, FALSE, 0);

    GtkWidget *list = create_booked_list();
    gtk_box_pack_start(GTK_BOX(main_box), list, TRUE, TRUE, 0);

    return scrolled_window;
}