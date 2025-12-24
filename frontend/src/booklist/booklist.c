#include <gtk/gtk.h>
#include "booklist.h"
#include <cairo.h>
#include <cairo-pdf.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h> // FIX: Thêm thư viện này để sửa lỗi send/recv
#include "../global/global.h"
#include "../component/component.h"
#include "../list/list.h" 
#include "../homepage/homepage.h"
#include "../my_account/my_account.h"

// --- KHAI BÁO PROTOTYPE NGOÀI ---
extern void show_notification(GtkWidget *widget, gpointer data);

// ============================================================
// PHẦN 1: LOGIC CHUYỂN TRANG AN TOÀN (SAFE SWITCH)
// ============================================================

// 1. Về trang chủ
static gboolean safe_switch_to_home(gpointer data) {
    (void)data;
    GtkWidget *home_window = create_homepage_window();
    set_content(home_window);
    return FALSE;
}

void on_nav_home_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    g_idle_add(safe_switch_to_home, NULL);
}

// 2. Sang My Account
static gboolean safe_switch_to_account(gpointer data) {
    (void)data;
    GtkWidget *account_window = create_my_account_window();
    set_content(account_window);
    return FALSE;
}

void on_nav_account_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    g_idle_add(safe_switch_to_account, NULL);
}

// ============================================================
// PHẦN 2: CSS STYLE (ĐÃ FIX LỖI KHUNG VIỀN)
// ============================================================
void apply_booklist_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        // 1. Nền Gradient chung
        "window { "
        "   background-image: linear-gradient(135deg, #2E3192, #1BFFFF); "
        "   font-family: 'Segoe UI', sans-serif; "
        "}\n"
        
        // 2. HEADER BUTTONS
        "#booklist-header-btn { "
        "   background-color: transparent; "
        "   background-image: none; "
        "   border: none; "
        "   box-shadow: none; "
        "   color: rgba(255,255,255,0.8); "
        "   font-weight: 900; font-size: 16px; "
        "   padding: 12px 25px; margin: 0 10px; "
        "   border-bottom: 3px solid transparent; "
        "   border-radius: 0; "
        "}\n"

        "#booklist-header-btn:hover { "
        "   color: #FFD700; "
        "   text-shadow: 0 0 10px rgba(255, 215, 0, 0.6); "
        "   background-color: transparent; " 
        "   box-shadow: none; "
        "}\n"

        // Trạng thái Checked/Active: Trong suốt, chỉ hiện gạch chân vàng
        "#booklist-header-btn:checked, #booklist-header-btn:active { "
        "   background-color: transparent; "
        "   background-image: none; "
        "   box-shadow: none; "
        "   border: none; "
        "   color: #FFD700; "
        "   border-bottom: 3px solid #FFD700; "
        "}\n"
        
        "#booklist-header-btn:focus { "
        "   outline: none; "
        "   box-shadow: none; "
        "}\n"

        // 3. BOOKED CARD
        ".booked-card { "
        "   background-color: #ffffff; "
        "   border-radius: 20px; "
        "   padding: 0; margin: 15px 40px; "
        "   box-shadow: 0 15px 35px rgba(0,0,0,0.2); "
        "}\n"
        ".booked-card:hover { box-shadow: 0 20px 45px rgba(0,0,0,0.3); }\n"

        // 4. TYPOGRAPHY
        ".airline-label { font-weight: 900; font-size: 18px; color: #223A60; }\n"
        ".time-huge { font-weight: 900; font-size: 30px; color: #2a0845; }\n"
        ".airport-code { "
        "   background-color: #f1f2f6; color: #666; font-weight: 700; font-size: 13px; "
        "   padding: 4px 8px; border-radius: 6px; "
        "}\n"
        ".price-booked { font-weight: 900; font-size: 24px; color: #27ae60; }\n"
        ".seat-info { color: #555; font-weight: 600; font-size: 14px; }\n"

        // 5. BUTTONS
        ".btn-print { "
        "   background-image: linear-gradient(to right, #3498db, #2980b9); "
        "   color: white; font-weight: bold; border-radius: 15px; border: none; "
        "}\n"
        ".btn-print:hover { box-shadow: 0 5px 15px rgba(52, 152, 219, 0.4); }\n"
        
        ".btn-cancel { "
        "   background-color: #ecf0f1; color: #7f8c8d; font-weight: bold; "
        "   border-radius: 15px; border: none; "
        "}\n"
        ".btn-cancel:hover { background-color: #bdc3c7; color: #c0392b; }\n"

        // 6. NO TICKET
        ".no-ticket { color: white; font-size: 24px; font-weight: 300; opacity: 0.8; }\n"
        
        , -1, NULL);
    
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), 
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    g_object_unref(provider);
}

// ============================================================
// PHẦN 3: LOGIC IN VÉ (PDF)
// ============================================================
static void draw_ticket_for_pdf(cairo_t *cr, double width, double height, const Ticket *ticket) {
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_set_line_width(cr, 2.0);
    cairo_rectangle(cr, 20, 20, width - 40, height - 40);
    cairo_stroke(cr);

    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 24);
    cairo_move_to(cr, 50, 60);
    cairo_show_text(cr, "FLIGHT TICKET");

    cairo_set_font_size(cr, 18);
    cairo_move_to(cr, 50, 100);
    cairo_show_text(cr, ticket->airplane_name);

    char buffer[512]; // FIX: Tăng size buffer
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

// ============================================================
// PHẦN 4: LOGIC HỦY VÉ
// ============================================================
typedef struct {
    int booking_id;
} CancelParams;

static gboolean execute_cancel_on_main_thread(gpointer user_data) {
    CancelParams *params = (CancelParams *)user_data;
    
    snprintf(buffer, sizeof(buffer), "DELETE BOOKING: %d", params->booking_id);
    send(sock, buffer, strlen(buffer) + 1, 0);
    recv(sock, buffer, sizeof(buffer), 0);
    
    // Refresh danh sách sau khi xóa
    if (get_list_tickets_ordered() != -1) {
         GtkWidget *new_window = create_booklist_window();
         set_content(new_window);
    }
    
    g_free(params);
    return FALSE;
}

void on_cancel_clicked(GtkWidget *widget, gpointer data) {
    Ticket *ticket = (Ticket *)data;
    
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

// ============================================================
// PHẦN 5: GIAO DIỆN (HEADER, CARD, LIST)
// ============================================================

GtkWidget* create_booklist_header(GtkWidget *main_box) {
    (void)main_box;

    // 1. Tạo Container
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(header, 900, 60);
    gtk_widget_set_margin_top(header, 30);
    gtk_widget_set_halign(header, GTK_ALIGN_CENTER);

    // 2. Menu Box
    GtkWidget *menu_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(menu_box), TRUE);
    gtk_box_pack_start(GTK_BOX(header), menu_box, TRUE, TRUE, 0);

    // 3. Buttons (Tạo thủ công để kiểm soát ID và Style)
    // Nút HOME
    GtkWidget *btn_home = gtk_button_new_with_label("HOME");
    gtk_widget_set_name(btn_home, "booklist-header-btn");
    g_signal_connect(btn_home, "clicked", G_CALLBACK(on_nav_home_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(menu_box), btn_home, TRUE, TRUE, 10);

    // Nút MY BOOKINGS (Toggle Button)
    GtkWidget *btn_booking = gtk_toggle_button_new_with_label("MY BOOKINGS");
    gtk_widget_set_name(btn_booking, "booklist-header-btn");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btn_booking), TRUE);
    gtk_box_pack_start(GTK_BOX(menu_box), btn_booking, TRUE, TRUE, 10);

    // Nút MY ACCOUNT
    GtkWidget *btn_account = gtk_button_new_with_label("MY ACCOUNT");
    gtk_widget_set_name(btn_account, "booklist-header-btn");
    g_signal_connect(btn_account, "clicked", G_CALLBACK(on_nav_account_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(menu_box), btn_account, TRUE, TRUE, 10);

    // Nút NOTIFICATIONS
    GtkWidget *btn_noti = gtk_button_new_with_label("NOTIFICATIONS");
    gtk_widget_set_name(btn_noti, "booklist-header-btn");
    g_signal_connect(btn_noti, "clicked", G_CALLBACK(show_notification), NULL);
    gtk_box_pack_start(GTK_BOX(menu_box), btn_noti, TRUE, TRUE, 10);

    return header;
}

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

    // Row Top
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

    // Row Time
    GtkWidget *row_time = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_top(row_time, 15);

    char time_only[10], date_val[20];
    split_date_time(ticket->departure_time, date_val, time_only);
    
    GtkWidget *col_dep = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *lbl_dep = gtk_label_new(time_only);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_dep), "time-huge");
    GtkWidget *lbl_dep_code = gtk_label_new(extract_middle_string(ticket->departure_airport));
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_dep_code), "airport-code");
    gtk_box_pack_start(GTK_BOX(col_dep), lbl_dep, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(col_dep), lbl_dep_code, FALSE, FALSE, 0);

    GtkWidget *col_mid = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(col_mid, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(col_mid, 150, -1);
    GtkWidget *lbl_date = gtk_label_new(date_val);
    GtkWidget *visual = gtk_label_new("---- ✈ ----");
    gtk_widget_override_color(visual, GTK_STATE_FLAG_NORMAL, &grey);
    gtk_box_pack_start(GTK_BOX(col_mid), lbl_date, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(col_mid), visual, FALSE, FALSE, 0);

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
    
    // Seats
    char seat_buff[512]; // FIX: Tăng size buffer
    snprintf(seat_buff, sizeof(seat_buff), "Seats: %s", ticket->list_ticket);
    GtkWidget *lbl_seats = gtk_label_new(seat_buff);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_seats), "seat-info");
    gtk_widget_set_halign(lbl_seats, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(left_box), lbl_seats, FALSE, FALSE, 5);

    // RIGHT: Price & Buttons
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