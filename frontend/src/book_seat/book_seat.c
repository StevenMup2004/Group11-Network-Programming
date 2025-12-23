#include <gtk/gtk.h>
#include "book_seat.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <math.h>
#include <string.h>
#include <arpa/inet.h>
#include "../global/global.h"
#include "../payment/payment.h"
#include "../ticket_detail/ticket_detail.h"
 
#define ROWS 30
#define SEATS_PER_COL 6
#define AISLE_SIZE 40  
 
// Kích thước ghế
#define SEAT_SIZE 40
#define SEAT_GAP 10
 
int actual_rows = 20;
int ordered[ROWS][SEATS_PER_COL];
 
// Biến toàn cục cho nút bấm
double button_x, button_y, button_width, button_height; // Nút Confirm
double btn_back_x, btn_back_y, btn_back_w, btn_back_h;  // Nút Back
 
typedef struct {
    int row;
    int col;
    gboolean selected;
    char label[4];
    double x, y;
} Seat;
 
Seat seats[ROWS][SEATS_PER_COL];
char selected_seat_label[4] = "";  

// --- HÀM KIỂM TRA HẠNG VÉ ---
// Trả về TRUE nếu hàng ghế (row_idx) thuộc về hạng vé (ticket_class) đang chọn
static gboolean is_row_in_class(int row_idx, const char *ticket_class) {
    // Row 0 (Hàng 1): First Class
    // Row 1-2 (Hàng 2-3): Business
    // Row 3+ (Hàng 4 trở đi): Economy
    
    if (strcmp(ticket_class, "First Class") == 0) {
        return (row_idx == 0);
    } 
    else if (strcmp(ticket_class, "Business") == 0) {
        return (row_idx == 1 || row_idx == 2);
    } 
    else { // Economy
        return (row_idx >= 3);
    }
}

// --- HÀM LẤY HỆ SỐ GIÁ ---
static double get_price_multiplier(const char *ticket_class) {
    if (strcmp(ticket_class, "First Class") == 0) return 2.0;
    if (strcmp(ticket_class, "Business") == 0) return 1.5;
    return 1.0; // Economy
}
 
// Helper: Vẽ hình chữ nhật bo góc
static void draw_rounded_rect(cairo_t *cr, double x, double y, double w, double h, double r) {
    cairo_new_path(cr);
    cairo_arc(cr, x + w - r, y + r, r, -M_PI / 2, 0);
    cairo_arc(cr, x + w - r, y + h - r, r, 0, M_PI / 2);
    cairo_arc(cr, x + r, y + h - r, r, M_PI / 2, M_PI);
    cairo_arc(cr, x + r, y + r, r, M_PI, 3 * M_PI / 2);
    cairo_close_path(cr);
}
 
// Helper: Vẽ Ghế
static void draw_airplane_seat(cairo_t *cr, double x, double y, int state, const char* label) {
    // state: 0=Available, 1=Selected, 2=Unavailable
   
    // Màu nền ghế
    if (state == 2) cairo_set_source_rgb(cr, 0.85, 0.85, 0.85); // Xám (Taken)
    else if (state == 1) cairo_set_source_rgb(cr, 0.0, 0.3, 0.8); // Xanh đậm (Selected)
    else cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // Trắng (Available)
 
    draw_rounded_rect(cr, x, y, SEAT_SIZE, SEAT_SIZE, 8);
    cairo_fill_preserve(cr);
 
    // Viền ghế
    if (state == 1) cairo_set_source_rgb(cr, 0.0, 0.2, 0.6);
    else cairo_set_source_rgb(cr, 0.6, 0.7, 0.9);
    cairo_set_line_width(cr, 2.0);
    cairo_stroke(cr);
 
    // Vẽ dấu X nếu đã đặt
    if (state == 2) {
        cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
        cairo_move_to(cr, x + 10, y + 10);
        cairo_line_to(cr, x + SEAT_SIZE - 10, y + SEAT_SIZE - 10);
        cairo_move_to(cr, x + SEAT_SIZE - 10, y + 10);
        cairo_line_to(cr, x + 10, y + SEAT_SIZE - 10);
        cairo_stroke(cr);
    } else {
        // Vẽ nhãn ghế (A, B...)
        if (state == 1) cairo_set_source_rgb(cr, 1, 1, 1);
        else cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
       
        cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 14);
        cairo_text_extents_t ext;
        cairo_text_extents(cr, label, &ext);
        cairo_move_to(cr, x + (SEAT_SIZE - ext.width)/2, y + (SEAT_SIZE + ext.height)/2);
        cairo_show_text(cr, label);
    }
}
 
void initialize_seats() {
    actual_rows = detail_flight.capacity / 10;
    if (actual_rows > ROWS) actual_rows = ROWS;
    memset(ordered, 0, sizeof(ordered));
    
    // Cập nhật trạng thái ghế đã đặt từ server
    for (int k = 0; k < seat_count; k++) {
        int r, c;
        if (get_seat_position(seats_array[k], &r, &c) == 0) {
             if (r < ROWS && c < SEATS_PER_COL) ordered[r][c] = 1;
        }
    }
}
 
static gboolean on_book_seat_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    (void)user_data;
    gint w = gtk_widget_get_allocated_width(widget);
    gint h = gtk_widget_get_allocated_height(widget);
 
    // 1. Nền xám nhạt
    cairo_set_source_rgb(cr, 0.96, 0.96, 0.96);
    cairo_paint(cr);
 
    // 2. Vẽ Modal
    double modal_w = 1000;
    double modal_h = 650;
    double modal_x = (w - modal_w) / 2;
    double modal_y = (h - modal_h) / 2;
 
    // Bóng đổ
    cairo_save(cr);
    draw_rounded_rect(cr, modal_x+5, modal_y+5, modal_w, modal_h, 15);
    cairo_set_source_rgba(cr, 0, 0, 0, 0.1);
    cairo_fill(cr);
    cairo_restore(cr);
 
    // Thân modal
    draw_rounded_rect(cr, modal_x, modal_y, modal_w, modal_h, 15);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_fill(cr);
 
    // Header Xanh đậm
    cairo_new_path(cr);
    cairo_arc(cr, modal_x + modal_w - 15, modal_y + 15, 15, -M_PI/2, 0);
    cairo_line_to(cr, modal_x + modal_w, modal_y + 60);
    cairo_line_to(cr, modal_x, modal_y + 60);
    cairo_line_to(cr, modal_x, modal_y + 15);
    cairo_arc(cr, modal_x + 15, modal_y + 15, 15, M_PI, 3*M_PI/2);
    cairo_close_path(cr);
    cairo_set_source_rgb(cr, 0.1, 0.15, 0.3);
    cairo_fill(cr);
    
    // Tiêu đề Header (Hiển thị hạng vé đang chọn)
    char title_buffer[100];
    snprintf(title_buffer, sizeof(title_buffer), "Select Seat - %s Class", class);
    
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_select_font_face(cr, "Inter", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 22);
    cairo_move_to(cr, modal_x + 30, modal_y + 38);
    cairo_show_text(cr, title_buffer);
 
    // 3. Legend
    double legend_y = modal_y + 80;
    double legend_x = modal_x + (modal_w - 420) / 2;
   
    // Available
    draw_airplane_seat(cr, legend_x, legend_y, 0, "");
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2); cairo_set_font_size(cr, 14);
    cairo_move_to(cr, legend_x + 45, legend_y + 25); cairo_show_text(cr, "Available");
 
    // Selected
    draw_airplane_seat(cr, legend_x + 150, legend_y, 1, "");
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    cairo_move_to(cr, legend_x + 195, legend_y + 25); cairo_show_text(cr, "Selected");
 
    // Taken
    draw_airplane_seat(cr, legend_x + 300, legend_y, 2, "");
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    cairo_move_to(cr, legend_x + 345, legend_y + 25); cairo_show_text(cr, "Taken");
 
    // 4. Máy Bay Ngang
    double grid_start_x = modal_x + 80;
    double grid_start_y = modal_y + 200;
   
    double total_seat_w = actual_rows * (SEAT_SIZE + SEAT_GAP);
    double total_seat_h = (SEATS_PER_COL * SEAT_SIZE) + ((SEATS_PER_COL - 1) * SEAT_GAP) + AISLE_SIZE;
   
    // Vẽ khung thân máy bay
    double plane_x = grid_start_x - 40;
    double plane_y = grid_start_y - 40;
    double plane_w = total_seat_w + 100;
    double plane_h = total_seat_h + 80;
 
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    cairo_new_path(cr);
    cairo_move_to(cr, plane_x + 50, plane_y);
    cairo_curve_to(cr, plane_x - 50, plane_y, plane_x - 50, plane_y + plane_h, plane_x + 50, plane_y + plane_h);
    cairo_line_to(cr, plane_x + plane_w - 20, plane_y + plane_h);
    cairo_curve_to(cr, plane_x + plane_w + 20, plane_y + plane_h, plane_x + plane_w + 20, plane_y, plane_x + plane_w - 20, plane_y);
    cairo_close_path(cr);
    cairo_fill_preserve(cr);
    cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
    cairo_set_line_width(cr, 2.0);
    cairo_stroke(cr);
 
    // 5. Lưới Ghế (CÓ LỌC THEO HẠNG VÉ)
    const char *cols = "ABCDEF";
    for (int r = 0; r < actual_rows; r++) {
        // [QUAN TRỌNG] Chỉ vẽ ghế nếu thuộc hạng vé đang chọn
        if (!is_row_in_class(r, class)) {
            continue; 
        }

        double seat_x = grid_start_x + r * (SEAT_SIZE + SEAT_GAP);
       
        // Vẽ số hàng
        char row_label[4];
        snprintf(row_label, 4, "%d", r + 1);
        cairo_text_extents_t ext;
        cairo_text_extents(cr, row_label, &ext);
        cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
        cairo_move_to(cr, seat_x + (SEAT_SIZE - ext.width)/2, grid_start_y - 15);
        cairo_show_text(cr, row_label);
 
        for (int c = 0; c < SEATS_PER_COL; c++) {
            double seat_y = grid_start_y + c * (SEAT_SIZE + SEAT_GAP);
            if (c >= 3) seat_y += AISLE_SIZE;
 
            seats[r][c].x = seat_x;
            seats[r][c].y = seat_y;
            seats[r][c].row = r + 1;
            seats[r][c].col = c;
            snprintf(seats[r][c].label, 4, "%d%c", r + 1, cols[c]);
 
            int state = 0;
            if (ordered[r][c] == 1) state = 2;
            if (seats[r][c].selected) state = 1;
 
            char seat_char[2] = {cols[c], '\0'};
            draw_airplane_seat(cr, seat_x, seat_y, state, seat_char);
        }
    }
 
    // 6. Nút Confirm
    button_width = 140;
    button_height = 45;
    button_x = modal_x + modal_w - 180;
    button_y = modal_y + modal_h - 70;
 
    draw_rounded_rect(cr, button_x, button_y, button_width, button_height, 22);
    cairo_set_source_rgb(cr, 0.1, 0.6, 0.3); // Xanh lá
    cairo_fill(cr);
 
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_set_font_size(cr, 16);
    cairo_text_extents_t ext;
    cairo_text_extents(cr, "Confirm", &ext);
    cairo_move_to(cr, button_x + (button_width - ext.width)/2, button_y + 28);
    cairo_show_text(cr, "Confirm");
 
    // 7. Nút Back
    btn_back_w = 120;
    btn_back_h = 45;
    btn_back_x = button_x - btn_back_w - 20;
    btn_back_y = button_y;
 
    draw_rounded_rect(cr, btn_back_x, btn_back_y, btn_back_w, btn_back_h, 22);
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    cairo_fill_preserve(cr);
    cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
    cairo_set_line_width(cr, 1.0);
    cairo_stroke(cr);
 
    cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
    cairo_set_font_size(cr, 16);
    cairo_text_extents(cr, "Back", &ext);
    cairo_move_to(cr, btn_back_x + (btn_back_w - ext.width)/2, btn_back_y + 28);
    cairo_show_text(cr, "Back");
 
    return FALSE;
}
 
// Xử lý Click
gboolean on_mouse_click_book_seat(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    (void)data;
 
    // 1. Check Nút Confirm
    if (event->x >= button_x && event->x <= button_x + button_width &&
        event->y >= button_y && event->y <= button_y + button_height) {
        if (tem_seats_size == 0) return TRUE;
        GtkWidget *payment_window = create_payment_window();
        set_content(payment_window);
        return TRUE;
    }
 
    // 2. Check Nút Back
    if (event->x >= btn_back_x && event->x <= btn_back_x + btn_back_w &&
        event->y >= btn_back_y && event->y <= btn_back_y + btn_back_h) {
       
        GtkWidget *ticket_detail_window = create_ticket_detail_window();
        set_content(ticket_detail_window);
        return TRUE;
    }
 
    // 3. Check Click vào ghế (ÁP DỤNG LOGIC MỚI)
    for (int r = 0; r < actual_rows; r++) {
        // [QUAN TRỌNG] Bỏ qua các hàng không thuộc hạng vé đang chọn
        if (!is_row_in_class(r, class)) continue;

        for (int c = 0; c < SEATS_PER_COL; c++) {
            Seat *s = &seats[r][c];
            if (event->x >= s->x && event->x <= s->x + SEAT_SIZE &&
                event->y >= s->y && event->y <= s->y + SEAT_SIZE) {
               
                if (ordered[r][c] == 1) return TRUE; // Đã đặt
 
                s->selected = !s->selected;
                
                // TÍNH GIÁ TIỀN ĐỘNG
                double multiplier = get_price_multiplier(class);
                int seat_price = (int)(detail_flight.price * multiplier);

                if (s->selected) {
                    temp_seats = add_string_to_array(temp_seats, &tem_seats_size, s->label);
                    price += seat_price;
                } else {
                    temp_seats = remove_string_from_array(temp_seats, &tem_seats_size, s->label);
                    price -= seat_price;
                }
                gtk_widget_queue_draw(widget);
                return TRUE;
            }
        }
    }
    return FALSE;
}
 
GtkWidget* create_book_seat_window() {
    GtkWidget *drawing_area, *main_box;
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    initialize_seats();
    drawing_area = gtk_drawing_area_new();
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_book_seat_draw), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "button-press-event", G_CALLBACK(on_mouse_click_book_seat), NULL);
    gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    gtk_box_pack_start(GTK_BOX(main_box), drawing_area, TRUE, TRUE, 0);
    return main_box;
}