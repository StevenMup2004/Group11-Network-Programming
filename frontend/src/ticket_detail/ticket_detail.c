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

gint screen_width;
gint screen_height;

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

    double confirm_y = card_y + CARD_H - 30 - BTN_H; // Đã sửa khớp với giao diện
    double confirm_x = card_x + 60;
    double confirm_w = CARD_W - 120;
    double cancel_y = confirm_y - 40;
    
    // 1. CONFIRM CLICK
    if (event->x >= confirm_x && event->x <= confirm_x + confirm_w &&
        event->y >= confirm_y && event->y <= confirm_y + BTN_H) {
        
        g_print("Confirm clicked!\n");
        snprintf(buffer, sizeof(buffer), "GET ORDERED SEATS %s", detail_flight.flight_id);
        send(sock, buffer, strlen(buffer), 0);
        
        // Nhận dữ liệu từ Server (Fix lỗi treo do Packet Coalescing)
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            
            // Trường hợp 1: Chưa có ghế nào đặt
            if (strstr(buffer, "NOTHING") != NULL){
                seat_count = 0;
                if(seats_array) { free(seats_array); seats_array = NULL; }
                
                GtkWidget *book_seat_window = create_book_seat_window();
                set_content(book_seat_window);
                return TRUE;
            }

            // Trường hợp 2: Có ghế đã đặt
            if (sscanf(buffer, "SEAT COUNT: %d", &seat_count) == 1) {
                // Reset mảng ghế cũ
                if (seats_array) free(seats_array);
                
                if (seat_count > 0) {
                    seats_array = malloc(seat_count * sizeof(char *));
                    
                    // Kiểm tra xem danh sách ghế có nằm ngay sau ký tự xuống dòng '\n' không
                    char *list_start = strchr(buffer, '\n');
                    
                    if (list_start && strlen(list_start) > 1) {
                        // Dữ liệu đã có sẵn trong buffer lần 1
                        list_start += 1; // Bỏ qua ký tự '\n'
                    } else {
                        // Nếu chưa có, mới gọi recv lần 2
                        bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
                        if (bytes_received <= 0) {
                            perror("Error receiving seats data");
                            return TRUE;
                        }
                        buffer[bytes_received] = '\0';
                        list_start = buffer;
                    }

                    // Tách chuỗi ghế
                    char *token = strtok(list_start, ",");
                    int i = 0;
                    while (token && i < seat_count) {
                        // Xóa ký tự whitespace/newline thừa
                        char *clean_token = token;
                        while(*clean_token == ' ' || *clean_token == '\n' || *clean_token == '\r') clean_token++;
                        
                        seats_array[i] = strdup(clean_token);
                        token = strtok(NULL, ",");
                        i++;
                    }
                } else {
                    seats_array = NULL;
                }

                // Chuyển trang
                GtkWidget *book_seat_window = create_book_seat_window();
                set_content(book_seat_window);
                return TRUE;
            }
        }
        return TRUE;
    }

    // 2. CANCEL CLICK
    // Nút cancel nằm ở vị trí cancel_y, chiều cao khoảng 30px (text-only button)
    if (event->x >= card_x && event->x <= card_x + CARD_W &&
        event->y >= cancel_y && event->y <= cancel_y + 30) {
        GtkWidget *list_window = create_list_window();
        set_content(list_window);
        return TRUE;
    }

    return TRUE;
}

/* ============================================
   DRAWING FUNCTION
   ============================================ */

static gboolean on_ticket_detail_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    (void)user_data;
    screen_width = gtk_widget_get_allocated_width(widget);
    screen_height = gtk_widget_get_allocated_height(widget);
    
    char date_str[20], time_str[20];
    split_date_time(detail_flight.departure_time, date_str, time_str);

    // 1. NỀN GRADIENT
    cairo_pattern_t *pat = cairo_pattern_create_linear(0.0, 0.0, screen_width, screen_height);
    cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.18, 0.19, 0.57); // Tím đậm
    cairo_pattern_add_color_stop_rgb(pat, 1.0, 0.11, 1.0, 1.0);   // Xanh Cyan
    cairo_set_source(cr, pat);
    cairo_paint(cr);
    cairo_pattern_destroy(pat);

    // Center Card
    double cx = (screen_width - CARD_W) / 2.0;
    double cy = (screen_height - CARD_H) / 2.0;
    double r = 30.0;

    // 2. Bóng đổ
    cairo_save(cr);
    draw_rounded_rect(cr, cx + 10, cy + 10, CARD_W, CARD_H, r);
    cairo_set_source_rgba(cr, 0, 0, 0, 0.25);
    cairo_fill(cr);
    cairo_restore(cr);

    // 3. Thẻ Vé (Màu trắng)
    cairo_save(cr);
    draw_rounded_rect(cr, cx, cy, CARD_W, CARD_H, r);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_fill(cr);
    cairo_restore(cr);

    // 4. HEADER
    double content_y = cy + 50;
    double margin_x = 50.0;
    char dep_code[20], arr_code[20];
    get_airport_code(detail_flight.departure_airport, dep_code); 
    get_airport_code(detail_flight.arrival_airport, arr_code);   

    // Trái (Dep)
    cairo_select_font_face(cr, "Segoe UI", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 60); 
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.3);
    cairo_move_to(cr, cx + margin_x, content_y + 50);
    cairo_show_text(cr, dep_code);

    cairo_set_font_size(cr, 16);
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_move_to(cr, cx + margin_x, content_y + 80);
    cairo_show_text(cr, extract_middle_string(detail_flight.departure_airport));

    // Phải (Arr)
    cairo_text_extents_t extents;
    cairo_select_font_face(cr, "Segoe UI", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 60);
    cairo_text_extents(cr, arr_code, &extents);
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.3);
    cairo_move_to(cr, cx + CARD_W - margin_x - extents.width, content_y + 50);
    cairo_show_text(cr, arr_code);

    cairo_set_font_size(cr, 16);
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    char *arr_name = extract_middle_string(detail_flight.arrival_airport);
    cairo_text_extents(cr, arr_name, &extents);
    cairo_move_to(cr, cx + CARD_W - margin_x - extents.width, content_y + 80);
    cairo_show_text(cr, arr_name);

    // Icon máy bay
    double mid_y = content_y + 25;
    GdkPixbuf *icon = gdk_pixbuf_new_from_file("../assets/images/flight-icon.png", NULL);
    if(icon) {
        GdkPixbuf *s_icon = gdk_pixbuf_scale_simple(icon, 50, 50, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, s_icon, cx + CARD_W/2 - 25, mid_y - 25);
        cairo_paint(cr);
        g_object_unref(s_icon); g_object_unref(icon);
    }

    // 5. GRID INFO
    double grid_y = content_y + 150;
    double col1_x = cx + margin_x;
    double col2_x = cx + CARD_W / 2 + 40;
    double row_h = 80; 

    // Helper nội bộ để vẽ text cặp (Label - Value)
    void draw_pair(cairo_t *c, const char* l, const char* v, double x, double y) {
        cairo_set_source_rgb(c, 0.6, 0.6, 0.6);
        cairo_select_font_face(c, "Segoe UI", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(c, 14);
        cairo_move_to(c, x, y);
        cairo_show_text(c, l);
        cairo_set_source_rgb(c, 0.15, 0.15, 0.15);
        cairo_select_font_face(c, "Segoe UI", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(c, 22);
        cairo_move_to(c, x, y + 30);
        cairo_show_text(c, v);
    }

    draw_pair(cr, "Passenger", current_user_full_name, col1_x, grid_y);
    draw_pair(cr, "Flight No", detail_flight.flight_id, col2_x, grid_y);
    draw_pair(cr, "Date", date_str, col1_x, grid_y + row_h);
    draw_pair(cr, "Time", time_str, col2_x, grid_y + row_h);

    char price_txt[50];
    snprintf(price_txt, 50, "%s VND", format_number_with_separator(detail_flight.price, ','));
    draw_pair(cr, "Class", "Economy", col1_x, grid_y + row_h * 2);
    draw_pair(cr, "Total Price", price_txt, col2_x, grid_y + row_h * 2);

    // 6. TEAR LINE
    double tear_y = grid_y + row_h * 3 + 30;
    cairo_set_source_rgb(cr, 0.3, 0.6, 0.8); // Giả lập màu nền
    cairo_arc(cr, cx, tear_y, 25, 0, 2 * M_PI); cairo_fill(cr);
    cairo_arc(cr, cx + CARD_W, tear_y, 25, 0, 2 * M_PI); cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.8, 0.8, 0.8); cairo_set_line_width(cr, 2.0);
    const double dashes[] = {8.0}; cairo_set_dash(cr, dashes, 1, 0);
    cairo_move_to(cr, cx + 30, tear_y); cairo_line_to(cr, cx + CARD_W - 30, tear_y);
    cairo_stroke(cr); cairo_set_dash(cr, NULL, 0, 0);

    // 7. BARCODE
    draw_barcode(cr, cx + 60, tear_y + 40, CARD_W - 120, 50);

    // 8. BUTTONS
    double btn_y = cy + CARD_H - 30 - BTN_H;
    double btn_x = cx + 60;
    double btn_w = CARD_W - 120;

    // Cancel Text
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_select_font_face(cr, "Segoe UI", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 16);
    cairo_text_extents(cr, "Cancel Selection", &extents);
    cairo_move_to(cr, cx + (CARD_W - extents.width)/2, btn_y - 20);
    cairo_show_text(cr, "Cancel Selection");

    // Confirm Button
    cairo_pattern_t *pat_btn = cairo_pattern_create_linear(btn_x, btn_y, btn_x + btn_w, btn_y);
    cairo_pattern_add_color_stop_rgb(pat_btn, 0.0, 1.0, 0.73, 0.35); 
    cairo_pattern_add_color_stop_rgb(pat_btn, 1.0, 1.0, 0.37, 0.43); 
    
    cairo_new_path(cr);
    draw_rounded_rect(cr, btn_x, btn_y, btn_w, BTN_H, BTN_H/2);
    cairo_set_source(cr, pat_btn);
    cairo_fill(cr);
    cairo_pattern_destroy(pat_btn);

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
    GtkWidget *drawing_area, *main_box;
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    drawing_area = gtk_drawing_area_new();
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_ticket_detail_draw), NULL);
    gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_mouse_click_ticket_detail), NULL);
    
    gtk_box_pack_start(GTK_BOX(main_box), drawing_area, TRUE, TRUE, 0);
    
    return main_box;
}