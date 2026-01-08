#include <gtk/gtk.h>
#include "homepage.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h> 
#include "../global/global.h"
#include "../server_com/server_com.h"
#include "../list/list.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "../my_account/my_account.h"
#include "../component/component.h" 

#define PLANE_IMAGE_PATH "../assets/images/plane.png"

// --- KHAI BÁO HÀM TỪ BÊN NGOÀI (FIX LỖI UNDECLARED) ---
extern void show_list_tickets(GtkWidget *widget, gpointer data);
// -----------------------------------------------------

// Biến toàn cục
GtkWidget *homepage_window;
GtkWidget *input_from, *input_to, *input_departure, *input_return, *combo_box, *input_traveller;
int bytes_received; 

// --- Forward declarations ---
static void on_window_realize(GtkWidget *widget, gpointer user_data);
static gboolean on_input_box_focus_in(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static void on_calendar_day_selected(GtkCalendar *calendar, gpointer user_data);
static gboolean on_draw_background(GtkWidget *widget, cairo_t *cr, gpointer user_data);
void apply_custom_css(GtkWidget *widget);

// ============================================================
// PHẦN 1: LOGIC & CHUYỂN TRANG
// ============================================================

void on_list_link_click(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data; 
    
    send(sock, "GET FLIGHTS", strlen("GET FLIGHTS"), 0);
    recv(sock, &flight_count, sizeof(flight_count), 0);
    int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) buffer[bytes_received] = '\0';

    parse_flight_data(buffer, flights);
    
    const char *traveller = gtk_entry_get_text(GTK_ENTRY(input_traveller));
    char *selected_class = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));
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

    char *from = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input_from));
    char *to = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(input_to));

    if (from == NULL || to == NULL || selected_class == NULL || strlen(date_text) == 0 || strlen(traveller) == 0){
        gtk_label_set_text(GTK_LABEL(label_status), "All fields are required!");
        if(from) g_free(from); if(to) g_free(to); if(selected_class) g_free(selected_class);
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
        if(from) g_free(from); if(to) g_free(to); if(selected_class) g_free(selected_class);
        return;
    }
    if (strcmp(from, to) == 0){
        gtk_label_set_text(GTK_LABEL(label_status), "Departure airport and arrive airport must be different!");
        if(from) g_free(from); if(to) g_free(to); if(selected_class) g_free(selected_class);
        return;
    }
    
    filter_flights(flights, flight_count, tem_flights, &tem_flight_count, from, to, date_text, selected_class, atoi(traveller));
    
    GtkWidget *list_window = create_list_window();
    set_content(list_window);

    if(from) g_free(from);
    if(to) g_free(to);
    if(selected_class) g_free(selected_class);
}

// --- LOGIC CHUYỂN TRANG MY ACCOUNT AN TOÀN ---

gboolean safe_switch_to_account_view(gpointer data) {
    (void)data;
    GtkWidget *account_view = create_my_account_window();
    set_content(account_view);
    return FALSE; // Dừng timeout
}

void on_my_account_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    // Dùng timeout để đảm bảo xử lý sự kiện click hoàn tất trước khi hủy widget
    g_timeout_add(150, safe_switch_to_account_view, NULL);
}

// ============================================================
// PHẦN 2: UI HELPERS & DRAWING
// ============================================================

static void on_window_realize(GtkWidget *widget, gpointer user_data) {
    (void)widget; 
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
    (void)widget; (void)event;
    GtkWidget *calendar = GTK_WIDGET(user_data);
    gtk_widget_show(calendar);
    return FALSE;
}

static gboolean on_draw_background(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    (void)user_data;
    if (!GTK_IS_WIDGET(widget) || !gtk_widget_get_realized(widget)) return FALSE;

    gint width = gtk_widget_get_allocated_width(widget);
    gint height = gtk_widget_get_allocated_height(widget);

    cairo_pattern_t *pat_left = cairo_pattern_create_linear(0.0, 0.0, width, height);
    cairo_pattern_add_color_stop_rgb(pat_left, 0.0, 0.45, 0.45, 0.95);
    cairo_pattern_add_color_stop_rgb(pat_left, 1.0, 0.40, 0.35, 0.90);  
    cairo_set_source(cr, pat_left);
    cairo_rectangle(cr, 0, 0, width, height); 
    cairo_fill(cr);
    cairo_pattern_destroy(pat_left);

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); 
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

void apply_custom_css(GtkWidget *widget) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "entry, combobox button, combobox { " 
        "   background-color: #ffffffff; " 
        "   color: #333333; "
        "   border-radius: 10px; "
        "   border: 1px solid #524141ff; "
        "   padding: 8px 12px; "  
        "   min-height: 40px; "   
        "   font-size: 14px; "
        "   font-family: 'Segoe UI', sans-serif; "
        "   box-shadow: 0 4px 6px rgba(0,0,0,0.05); "
        "}\n"
        "entry:focus, combobox button:checked { "
        "   border-color: #b3b3ff; "
        "   box-shadow: 0 0 0 3px rgba(255,255,255,0.3); " 
        "}\n"
        "#btn-book { "
        "   background-image: linear-gradient(to right, #FFC371, #FF5F6D); " 
        "   color: white; font-weight: 800; font-size: 16px; "
        "   border-radius: 30px; "
        "   border: none; "
        "   padding: 10px 25px; " 
        "   box-shadow: 0 5px 15px rgba(255, 95, 109, 0.4); "
        "}\n"
        "#btn-book:hover { transform: scale(1.05); box-shadow: 0 8px 20px rgba(255, 95, 109, 0.5); }\n"
        ".lbl-white { color: rgba(255,255,255,0.95); font-weight: 600; font-size: 13px; margin-bottom: 4px; }\n"
        ".lbl-title { "
        "   color: white; "
        "   font-weight: 900; "
        "   font-size: 36px; "
        "   font-family: 'Montserrat', 'Verdana', sans-serif; " 
        "   margin-bottom: 25px; "
        "   line-height: 1.2; "
        "   letter-spacing: 1px; "
        "   text-shadow: 0 2px 4px rgba(0,0,0,0.1); "
        "}\n"
        "#header-btn { "
        "   background: transparent; border: none; box-shadow: none; "
        "   color: #000000; "       
        "   font-weight: 800; "     
        "   font-size: 15px; "      
        "   font-family: 'Segoe UI', sans-serif; "
        "   padding: 10px 20px; "   
        "   margin: 0 10px; "       
        "}\n"
        "#header-btn:hover { "
        "   color: #FFC371; "       
        "   text-shadow: 0 0 5px rgba(255, 195, 113, 0.5); " 
        "}\n"
        "#header-btn:checked { "
        "   color: #FFC371; "
        "   border-bottom: 3px solid #FFC371; " 
        "   border-radius: 0; "
        "}\n"
        , -1, NULL);
    
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
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

GtkWidget* create_left_form() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12); 
    gtk_widget_set_margin_start(vbox, 80); 
    gtk_widget_set_margin_top(vbox, 40); 
    
    gtk_widget_set_valign(vbox, GTK_ALIGN_START); 
    gtk_widget_set_halign(vbox, GTK_ALIGN_START);
    gtk_widget_set_size_request(vbox, 500, -1); 

    GtkWidget *lbl_title = gtk_label_new("WHERE WOULD YOU\nLIKE TO GO ?");
    gtk_widget_set_halign(lbl_title, GTK_ALIGN_START);
    gtk_label_set_justify(GTK_LABEL(lbl_title), GTK_JUSTIFY_LEFT);
    GtkStyleContext *context = gtk_widget_get_style_context(lbl_title);
    gtk_style_context_add_class(context, "lbl-title");
    apply_custom_css(lbl_title);
    gtk_box_pack_start(GTK_BOX(vbox), lbl_title, FALSE, FALSE, 0);

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

    GtkWidget *hbox_row1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);

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
    // Lưu ý: Đã xóa signal "realize" gây leak bộ nhớ
    input_departure = date_cont;

    gtk_box_pack_start(GTK_BOX(box_date), lbl_date, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_date), date_cont, FALSE, FALSE, 0);

    GtkWidget *box_pass = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *lbl_pass = gtk_label_new("Passenger");
    gtk_widget_set_halign(lbl_pass, GTK_ALIGN_START);
    apply_custom_css(lbl_pass); gtk_style_context_add_class(gtk_widget_get_style_context(lbl_pass), "lbl-white");
    
    input_traveller = create_styled_input("1");
    gtk_widget_set_size_request(input_traveller, 80, -1); 
    
    gtk_box_pack_start(GTK_BOX(box_pass), lbl_pass, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_pass), input_traveller, FALSE, FALSE, 0);

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

    GtkWidget *btn_book = gtk_button_new_with_label("BOOK NOW");
    gtk_widget_set_name(btn_book, "btn-book");
    gtk_widget_set_size_request(btn_book, 180, 45); 
    gtk_widget_set_halign(btn_book, GTK_ALIGN_START);
    gtk_widget_set_margin_top(btn_book, 10); 
    apply_custom_css(btn_book);
    g_signal_connect(btn_book, "clicked", G_CALLBACK(on_list_link_click), homepage_window);

    gtk_box_pack_start(GTK_BOX(vbox), btn_book, FALSE, FALSE, 5);

    label_status = gtk_label_new("");
    GtkCssProvider *err_css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(err_css, "label { color: #ffe066; font-size: 13px; }", -1, NULL); 
    gtk_style_context_add_provider(gtk_widget_get_style_context(label_status), GTK_STYLE_PROVIDER(err_css), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_box_pack_start(GTK_BOX(vbox), label_status, FALSE, FALSE, 0);

    return vbox;
}

// --- HÀM TẠO HEADER NỘI BỘ (KHÔNG DÙNG COMPONENT.C ĐỂ TRÁNH LỖI) ---
// --- HÀM TẠO HEADER NỘI BỘ (KHÔNG DÙNG COMPONENT.C ĐỂ TRÁNH LỖI) ---
// Đã sửa lỗi menu bị trôi sang phải và chìm vào nền trắng
GtkWidget* create_header_local_safe(GtkWidget *main_box) {
    (void)main_box;
    
    // Tạo container chính cho Header
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(header, 1060, 40);
    gtk_widget_set_margin_top(header, 25); 
    gtk_widget_set_margin_bottom(header, 10);
    // Giữ header ở giữa màn hình, nhưng các phần tử bên trong sẽ được xếp từ trái sang
    gtk_widget_set_halign(header, GTK_ALIGN_CENTER); 

    // --- 1. LOGO ---
    //GtkWidget *logo = gtk_image_new_from_file("../assets/images/logo.png");
    //gtk_box_pack_start(GTK_BOX(header), logo, FALSE, FALSE, 10);
    
    // [QUAN TRỌNG] Giảm khoảng cách đẩy menu ra xa (Giảm từ 200 xuống 30)
    // gtk_widget_set_margin_end(logo, 30);

    // --- 2. MENU BOX ---
    GtkWidget *menu_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(menu_box), TRUE); // Các nút to đều nhau
    
    // Tạo các nút bấm
    GtkWidget *btn_home = gtk_button_new_with_label("HOME");
    GtkWidget *btn_ticket = gtk_button_new_with_label("MY BOOKINGS");
    GtkWidget *btn_account = gtk_button_new_with_label("MY ACCOUNT");
    GtkWidget *btn_noti = gtk_button_new_with_label("NOTIFICATIONS");

    // CSS chung cho nút
    GtkWidget *buttons[] = {btn_home, btn_ticket, btn_account, btn_noti, NULL};
    for(int i=0; buttons[i]!=NULL; i++) {
        gtk_widget_set_name(buttons[i], "header-btn");
        apply_custom_css(buttons[i]); // Nhớ kiểm tra file CSS có class header-btn
        gtk_widget_set_size_request(buttons[i], 120, 40);
        gtk_box_pack_start(GTK_BOX(menu_box), buttons[i], TRUE, TRUE, 10);
    }

    // Kết nối sự kiện
    g_signal_connect(btn_ticket, "clicked", G_CALLBACK(show_list_tickets), NULL);
    g_signal_connect(btn_noti, "clicked", G_CALLBACK(show_notification), NULL);
    g_signal_connect(btn_account, "clicked", G_CALLBACK(on_my_account_clicked), NULL);

    // --- 3. ĐÓNG GÓI MENU VÀO HEADER ---
    // [QUAN TRỌNG] Đổi TRUE, TRUE thành FALSE, FALSE 
    // Việc này ngăn menu giãn ra hết phần còn lại bên phải (nơi có nền trắng)
    gtk_box_pack_start(GTK_BOX(header), menu_box, FALSE, FALSE, 0);
    
    // Đảm bảo menu dính chặt về phía bên trái (phía logo)
    gtk_widget_set_halign(menu_box, GTK_ALIGN_START);

    return header;
}
GtkWidget* create_homepage_window() {
    GtkWidget *main_overlay = gtk_overlay_new();
    
    GtkWidget *drawing_area = gtk_drawing_area_new();
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw_background), NULL);
    gtk_container_add(GTK_CONTAINER(main_overlay), drawing_area);

    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(content_box, GTK_ALIGN_FILL);
    gtk_widget_set_valign(content_box, GTK_ALIGN_FILL);

    // Dùng hàm tạo header nội bộ an toàn
    GtkWidget *header = create_header_local_safe(content_box);
    gtk_box_pack_start(GTK_BOX(content_box), header, FALSE, FALSE, 0);

    GtkWidget *form = create_left_form();
    gtk_box_pack_start(GTK_BOX(content_box), form, FALSE, FALSE, 0); 

    gtk_overlay_add_overlay(GTK_OVERLAY(main_overlay), content_box);
    homepage_window = main_overlay; 

    return main_overlay;
}