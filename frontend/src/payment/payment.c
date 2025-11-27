#include "../payment/payment.h"
#include "../server_com/server_com.h"
#include "../book_seat/book_seat.h"
#include <cairo.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "../global/global.h"
#include "../booklist/booklist.h"
 
// --- MÀU SẮC & CẤU HÌNH ---
#define BG_COLOR_R 0.96
#define BG_COLOR_G 0.97
#define BG_COLOR_B 0.99
 
#define BLUE_R 0.0
#define BLUE_G 0.48
#define BLUE_B 1.0 // #007AFF - Màu xanh chủ đạo
 
#define CARD_W 380 // Chiều rộng thẻ vé trái
#define CARD_H 640 // Chiều cao thẻ vé trái
#define SUMMARY_W 400 // Chiều rộng thẻ summary phải
 
const char *gate_code = "22";
int selected_voucher = -1;
double discounted_price = 0;
char error_message[256] = "";
time_t error_start_time = 0;
 
// Tọa độ nút bấm để bắt sự kiện
double btn_pay_x, btn_pay_y, btn_pay_w, btn_pay_h;
double btn_cancel_x, btn_cancel_y, btn_cancel_w, btn_cancel_h;
 
// --- HELPERS ---
 
// Vẽ hình chữ nhật bo góc
static void draw_rounded_rect(cairo_t *cr, double x, double y, double w, double h, double r) {
    cairo_new_path(cr);
    cairo_arc(cr, x + w - r, y + r, r, -M_PI / 2, 0);
    cairo_arc(cr, x + w - r, y + h - r, r, 0, M_PI / 2);
    cairo_arc(cr, x + r, y + h - r, r, M_PI / 2, M_PI);
    cairo_arc(cr, x + r, y + r, r, M_PI, 3 * M_PI / 2);
    cairo_close_path(cr);
}
 
// Vẽ bóng đổ
static void draw_shadow(cairo_t *cr, double x, double y, double w, double h, double r) {
    cairo_save(cr);
    draw_rounded_rect(cr, x + 3, y + 3, w, h, r);
    cairo_set_source_rgba(cr, 0, 0, 0, 0.1);
    cairo_fill(cr);
    cairo_restore(cr);
}
 
// Vẽ text nhanh
static void draw_text(cairo_t *cr, const char *text, const char *font, double size, double x, double y, double r, double g, double b, gboolean is_bold) {
    cairo_set_source_rgb(cr, r, g, b);
    cairo_select_font_face(cr, font, CAIRO_FONT_SLANT_NORMAL, is_bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, size);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, text);
}
 
// Lấy mã sân bay (3 ký tự đầu)
static void get_airport_code(const char* src, char* dest) {
    if (!src) { strcpy(dest, "---"); return; }
    int i = 0;
    while (src[i] != '\0' && src[i] != ' ' && i < 5) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}
 
// Vẽ Barcode
static void draw_barcode(cairo_t *cr, double x, double y, double w, double h) {
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    double current_x = x;
    srand(123);
    while (current_x < x + w) {
        double line_w = (rand() % 3 + 1) * 1.5;
        if (current_x + line_w > x + w) break;
        cairo_rectangle(cr, current_x, y, line_w, h);
        cairo_fill(cr);
        current_x += line_w + (rand() % 3 + 1) * 1.5;
    }
}
 
// --- LOGIC VẼ ---
 
static gboolean on_payment_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    gint w = gtk_widget_get_allocated_width(widget);
    gint h = gtk_widget_get_allocated_height(widget);
 
    char date_str[20], time_str[20];
    split_date_time(detail_flight.departure_time, date_str, time_str);
 
    // 1. Nền toàn màn hình
    cairo_set_source_rgb(cr, BG_COLOR_R, BG_COLOR_G, BG_COLOR_B);
    cairo_paint(cr);
 
    // Header Title
    draw_text(cr, "Confirm Payment", "Inter", 28, (w - 250)/2, 50, 0.1, 0.1, 0.2, TRUE);
 
    // Tính toán Layout 2 cột
    double total_w = CARD_W + 40 + SUMMARY_W; // Khoảng cách giữa 2 cột là 40
    double start_x = (w - total_w) / 2;
    double start_y = 80;
 
    // ============================================================
    // CỘT TRÁI: TICKET PREVIEW (Giống Ticket Detail)
    // ============================================================
    double ticket_x = start_x;
    double ticket_y = start_y;
 
    // Bóng đổ và nền vé
    draw_shadow(cr, ticket_x, ticket_y, CARD_W, CARD_H, 24);
    draw_rounded_rect(cr, ticket_x, ticket_y, CARD_W, CARD_H, 24);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_fill(cr);
 
    // HEADER VÉ (SGN - HAN)
    char dep_code[10], arr_code[10];
    get_airport_code(detail_flight.departure_airport, dep_code);
    get_airport_code(detail_flight.arrival_airport, arr_code);
 
    // Mã sân bay đi
    cairo_set_source_rgb(cr, BLUE_R, BLUE_G, BLUE_B);
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 40);
    cairo_move_to(cr, ticket_x + 30, ticket_y + 80);
    cairo_show_text(cr, dep_code);
 
    // Tên sân bay đi
    draw_text(cr, extract_middle_string(detail_flight.departure_airport), "Inter", 14, ticket_x + 30, ticket_y + 105, 0.5, 0.5, 0.5, TRUE);
 
    // Mã sân bay đến (Căn phải)
    cairo_text_extents_t ext;
    cairo_set_font_size(cr, 40); cairo_text_extents(cr, arr_code, &ext);
    cairo_set_source_rgb(cr, BLUE_R, BLUE_G, BLUE_B);
    cairo_move_to(cr, ticket_x + CARD_W - 30 - ext.width, ticket_y + 80);
    cairo_show_text(cr, arr_code);
 
    // Tên sân bay đến
    char *arr_name = extract_middle_string(detail_flight.arrival_airport);
    cairo_set_font_size(cr, 14); cairo_text_extents(cr, arr_name, &ext);
    draw_text(cr, arr_name, "Inter", 14, ticket_x + CARD_W - 30 - ext.width, ticket_y + 105, 0.5, 0.5, 0.5, TRUE);
 
    // Icon máy bay và đường kẻ giữa
    double mid_y = ticket_y + 65;
    cairo_set_source_rgb(cr, 0.8, 0.8, 0.8); cairo_set_line_width(cr, 2.0);
    const double dashes[] = {4.0}; cairo_set_dash(cr, dashes, 1, 0);
    cairo_move_to(cr, ticket_x + 130, mid_y);
    cairo_line_to(cr, ticket_x + CARD_W - 130, mid_y);
    cairo_stroke(cr); cairo_set_dash(cr, NULL, 0, 0);
 
    GdkPixbuf *icon = gdk_pixbuf_new_from_file("../assets/images/flight-icon.png", NULL);
    if(icon) {
        GdkPixbuf *s_icon = gdk_pixbuf_scale_simple(icon, 30, 30, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, s_icon, ticket_x + CARD_W/2 - 15, mid_y - 15);
        cairo_paint(cr);
        g_object_unref(s_icon); g_object_unref(icon);
    }
 
    // GRID THÔNG TIN
    double grid_y = ticket_y + 150;
    double col1 = ticket_x + 30;
    double col2 = ticket_x + CARD_W/2 + 20;
    double row_h = 60;
 
    // Row 1
    draw_text(cr, "Passenger", "Inter", 12, col1, grid_y, 0.6, 0.6, 0.6, FALSE);
    draw_text(cr, "JOHN DOE", "Inter", 16, col1, grid_y + 20, 0.1, 0.1, 0.1, TRUE);
 
    draw_text(cr, "Flight", "Inter", 12, col2, grid_y, 0.6, 0.6, 0.6, FALSE);
    draw_text(cr, detail_flight.flight_id, "Inter", 16, col2, grid_y + 20, 0.1, 0.1, 0.1, TRUE);
 
    // Row 2
    draw_text(cr, "Date", "Inter", 12, col1, grid_y + row_h, 0.6, 0.6, 0.6, FALSE);
    draw_text(cr, date_str, "Inter", 16, col1, grid_y + row_h + 20, 0.1, 0.1, 0.1, TRUE);
 
    draw_text(cr, "Time", "Inter", 12, col2, grid_y + row_h, 0.6, 0.6, 0.6, FALSE);
    draw_text(cr, time_str, "Inter", 16, col2, grid_y + row_h + 20, 0.1, 0.1, 0.1, TRUE);
 
    // Row 3
    draw_text(cr, "Class", "Inter", 12, col1, grid_y + row_h*2, 0.6, 0.6, 0.6, FALSE);
    draw_text(cr, "Economy", "Inter", 16, col1, grid_y + row_h*2 + 20, 0.1, 0.1, 0.1, TRUE);
 
    draw_text(cr, "Price", "Inter", 12, col2, grid_y + row_h*2, 0.6, 0.6, 0.6, FALSE);
    char price_str[32]; snprintf(price_str, 32, "%s VND", format_number_with_separator(price, ','));
    draw_text(cr, price_str, "Inter", 16, col2, grid_y + row_h*2 + 20, 0.1, 0.1, 0.1, TRUE);
 
    // TEAR LINE (Đường xé vé)
    double tear_y = grid_y + row_h*3 + 20;
    cairo_set_source_rgb(cr, BG_COLOR_R, BG_COLOR_G, BG_COLOR_B); // Màu trùng màu nền
    cairo_arc(cr, ticket_x, tear_y, 15, 0, 2*M_PI); cairo_fill(cr); // Khuyết trái
    cairo_arc(cr, ticket_x + CARD_W, tear_y, 15, 0, 2*M_PI); cairo_fill(cr); // Khuyết phải
 
    cairo_set_source_rgb(cr, 0.8, 0.8, 0.8); cairo_set_line_width(cr, 2.0); cairo_set_dash(cr, dashes, 1, 0);
    cairo_move_to(cr, ticket_x + 20, tear_y); cairo_line_to(cr, ticket_x + CARD_W - 20, tear_y);
    cairo_stroke(cr); cairo_set_dash(cr, NULL, 0, 0);
 
    // BARCODE
    draw_barcode(cr, ticket_x + 40, tear_y + 40, CARD_W - 80, 60);
 
    // Nút Cancel (Text) dưới vé
    btn_cancel_w = 150; btn_cancel_h = 30;
    btn_cancel_x = ticket_x + (CARD_W - btn_cancel_w)/2;
    btn_cancel_y = ticket_y + CARD_H - 40;
    draw_text(cr, "Cancel Selection", "Inter", 14, btn_cancel_x + 20, btn_cancel_y + 20, 0.5, 0.5, 0.5, TRUE);
 
 
    // ============================================================
    // CỘT PHẢI: ORDER SUMMARY (Giống ảnh image_02b1d4.png)
    // ============================================================
    double summary_x = ticket_x + CARD_W + 40;
    double summary_y = start_y + 80; // Thấp hơn vé một chút để căn giữa đẹp
    double summary_h = 350;
 
    // Nền thẻ summary
    draw_shadow(cr, summary_x, summary_y, SUMMARY_W, summary_h, 15);
    draw_rounded_rect(cr, summary_x, summary_y, SUMMARY_W, summary_h, 15);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); cairo_fill(cr);
 
    double pad = 30;
    double cur_ry = summary_y + 40;
 
    // Header
    draw_text(cr, "Order Summary", "Inter", 20, summary_x + pad, cur_ry, 0.1, 0.15, 0.3, TRUE); // Màu xanh đen đậm
 
    // Thông tin chuyến bay
    cur_ry += 40;
    draw_text(cr, detail_flight.flight_id, "Inter", 16, summary_x + pad, cur_ry, 0.1, 0.15, 0.3, TRUE);
   
    char route_txt[100];
    snprintf(route_txt, 100, "%s -> %s", extract_middle_string(detail_flight.departure_airport), extract_middle_string(detail_flight.arrival_airport));
    cairo_set_font_size(cr, 14); cairo_text_extents(cr, route_txt, &ext);
    draw_text(cr, route_txt, "Inter", 14, summary_x + SUMMARY_W - pad - ext.width, cur_ry, 0.5, 0.5, 0.5, FALSE);
 
    // Divider
    cur_ry += 20;
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9); cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, summary_x + pad, cur_ry); cairo_line_to(cr, summary_x + SUMMARY_W - pad, cur_ry); cairo_stroke(cr);
 
    // Subtotal
    cur_ry += 30;
    draw_text(cr, "Subtotal", "Inter", 14, summary_x + pad, cur_ry, 0.5, 0.5, 0.5, FALSE);
    char sub_txt[32]; snprintf(sub_txt, 32, "%s VND", format_number_with_separator(price, ','));
    cairo_set_font_size(cr, 14); cairo_text_extents(cr, sub_txt, &ext);
    draw_text(cr, sub_txt, "Inter", 14, summary_x + SUMMARY_W - pad - ext.width, cur_ry, 0.1, 0.15, 0.3, TRUE);
 
    // Voucher Discount
    cur_ry += 30;
    draw_text(cr, "Voucher Discount", "Inter", 14, summary_x + pad, cur_ry, 0.5, 0.5, 0.5, FALSE);
    draw_text(cr, "0 VND", "Inter", 14, summary_x + SUMMARY_W - pad - 45, cur_ry, 0.6, 0.6, 0.6, TRUE); // Mặc định 0 vì bỏ voucher
 
    // Divider
    cur_ry += 20;
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    cairo_move_to(cr, summary_x + pad, cur_ry); cairo_line_to(cr, summary_x + SUMMARY_W - pad, cur_ry); cairo_stroke(cr);
 
    // Total Payable (Chữ to màu xanh)
    cur_ry += 40;
    draw_text(cr, "Total Payable", "Inter", 18, summary_x + pad, cur_ry, 0.1, 0.15, 0.3, TRUE);
   
    char total_txt[32];
    snprintf(total_txt, 32, "%s VND", format_number_with_separator(price, ','));
    cairo_set_font_size(cr, 24); cairo_text_extents(cr, total_txt, &ext);
    draw_text(cr, total_txt, "Inter", 24, summary_x + SUMMARY_W - pad - ext.width, cur_ry, BLUE_R, BLUE_G, BLUE_B, TRUE);
 
    // Nút Pay with VNPAY
    btn_pay_w = SUMMARY_W - 2*pad;
    btn_pay_h = 50;
    btn_pay_x = summary_x + pad;
    btn_pay_y = summary_y + summary_h - 30 - btn_pay_h;
 
    draw_rounded_rect(cr, btn_pay_x, btn_pay_y, btn_pay_w, btn_pay_h, 25); // Pill shape
    cairo_set_source_rgb(cr, BLUE_R, BLUE_G, BLUE_B);
    cairo_fill(cr);
 
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_set_font_size(cr, 16);
    cairo_text_extents(cr, "Pay with VNPAY", &ext);
    cairo_move_to(cr, btn_pay_x + (btn_pay_w - ext.width)/2, btn_pay_y + 32);
    cairo_show_text(cr, "Pay with VNPAY");
 
    return FALSE;
}
 
static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    // Check nút Pay
    if (event->x >= btn_pay_x && event->x <= btn_pay_x + btn_pay_w &&
        event->y >= btn_pay_y && event->y <= btn_pay_y + btn_pay_h) {
       
        final_price = price; // Không giảm giá
        vnpay_payment();
        receive_result_from_vnpay();
        return TRUE;
    }
 
    // Check nút Cancel Selection
    if (event->x >= btn_cancel_x && event->x <= btn_cancel_x + btn_cancel_w &&
        event->y >= btn_cancel_y && event->y <= btn_cancel_y + btn_cancel_h) {
       
        GtkWidget *book_seat_window = create_book_seat_window();
        set_content(book_seat_window);
        return TRUE;
    }
 
    return FALSE;
}
 
GtkWidget* create_payment_window() {
    GtkWidget *main_box, *drawing_area;
 
    selected_voucher = -1;    
    discounted_price = price;  
 
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    drawing_area = gtk_drawing_area_new();
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_payment_draw), NULL);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_button_press), NULL);
    gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    gtk_box_pack_start(GTK_BOX(main_box), drawing_area, TRUE, TRUE, 0);
 
    return main_box;
}
 