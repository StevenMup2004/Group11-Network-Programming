#include <gtk/gtk.h>
#include "list.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "../ticket_detail/ticket_detail.h"
#include "../component/component.h"
#include "../global/global.h"
#include "../homepage/homepage.h"
#include "../booklist/booklist.h"   
#include "../my_account/my_account.h" 

// --- BIẾN TOÀN CỤC ---
GtkWidget *ticket_list; 
GtkWidget *main_box;    
GtkWidget *list_window;
int selected_day_index = 0; 
char *days[5];
int num_days = 0;

// --- KHAI BÁO PROTOTYPE NGOÀI ---
extern void show_notification(GtkWidget *widget, gpointer data);
extern void show_list_tickets(GtkWidget *widget, gpointer data); 

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

// THÊM 'static' ĐỂ TRÁNH LỖI MULTIPLE DEFINITION
static void on_nav_home_clicked(GtkWidget *widget, gpointer data) {
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

// THÊM 'static' ĐỂ TRÁNH LỖI MULTIPLE DEFINITION
static void on_nav_account_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    g_idle_add(safe_switch_to_account, NULL);
}

// ============================================================
// PHẦN 2: CSS STYLE
// ============================================================
void apply_list_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        // 1. NỀN GRADIENT CHUNG
        "window { "
        "   background-image: linear-gradient(135deg, #2E3192, #1BFFFF); "
        "   font-family: 'Segoe UI', 'Montserrat', sans-serif; "
        "}\n"
        
        // 2. FIX LỖI NỀN TRẮNG
        "scrolledwindow, viewport, list, listbox { "
        "   background-color: transparent; "
        "   background-image: none; "
        "   border: none; "
        "}\n"

        // 3. HEADER BUTTONS
        "#list-header-btn { "
        "   background-color: transparent; "
        "   background-image: none; "
        "   border: none; "
        "   box-shadow: none; "
        "   color: rgba(255,255,255,0.8); "
        "   font-weight: 900; font-size: 16px; "      
        "   padding: 12px 25px; margin: 0 10px; "
        "   text-transform: uppercase; letter-spacing: 1px; "
        "   border-bottom: 3px solid transparent; "
        "   border-radius: 0; "
        "}\n"
        "#list-header-btn:hover { "
        "   color: #FFD700; "
        "   text-shadow: 0 0 10px rgba(255, 215, 0, 0.6); "
        "   background-color: transparent; "
        "   box-shadow: none; "
        "}\n"
        "#list-header-btn:checked, #list-header-btn:active { "
        "   background-color: transparent; "
        "   background-image: none; "
        "   box-shadow: none; "
        "   border: none; "
        "   color: #FFD700; "
        "   border-bottom: 3px solid #FFD700; "
        "}\n"
        "#list-header-btn:focus { outline: none; box-shadow: none; }\n"

        // 4. DAY BUTTONS
        ".day-btn { "
        "   background-color: rgba(0, 0, 0, 0.2); "
        "   color: #000000; font-weight: 700; font-size: 14px; "
        "   border-radius: 12px; border: 1px solid rgba(255,255,255,0.2); "
        "   padding: 10px 15px; margin: 0 5px; "
        "}\n"
        ".day-btn:checked { "
        "   background-image: linear-gradient(to right, #FF9966, #FF5E62); "
        "   color: white; border: none; box-shadow: 0 4px 15px rgba(255, 94, 98, 0.5); "
        "   transform: scale(1.05);"
        "}\n"
        ".day-btn:hover { background-color: rgba(0,0,0,0.4); color: white; }\n"

        // 5. OTHER UI ELEMENTS
        ".filter-label { color: #ffffff; font-weight: 800; font-size: 14px; margin-right: 10px; }\n"
        "combobox button { "
        "   background-color: #ffffff; color: #333; font-weight: 700; "
        "   border-radius: 8px; min-height: 35px; border: none; "
        "   box-shadow: 0 4px 6px rgba(0,0,0,0.1); "
        "}\n"

        ".ticket-card { "
        "   background-color: #ffffff; "
        "   border-radius: 20px; "
        "   padding: 0; margin: 15px 40px; "
        "   box-shadow: 0 15px 35px rgba(0,0,0,0.2); "
        "}\n"
        ".ticket-card:hover { "
        "   transform: translateY(-3px); "
        "   box-shadow: 0 20px 45px rgba(0,0,0,0.3); "
        "}\n"

        ".airline-label { font-weight: 900; font-size: 18px; color: #223A60; }\n"
        ".time-huge { font-weight: 900; font-size: 30px; color: #2a0845; }\n"
        ".airport-code { "
        "   background-color: #f1f2f6; color: #666; font-weight: 700; font-size: 13px; "
        "   padding: 4px 8px; border-radius: 6px; "
        "}\n"
        ".price-big { font-weight: 900; font-size: 24px; color: #FF5F6D; }\n"

        ".btn-select { "
        "   background-image: linear-gradient(to right, #FF512F, #DD2476); "
        "   color: white; font-weight: 900; font-size: 15px; "
        "   border-radius: 30px; border: none; "
        "   box-shadow: 0 5px 15px rgba(221, 36, 118, 0.4); "
        "}\n"
        ".btn-select:hover { transform: scale(1.05); box-shadow: 0 8px 20px rgba(221, 36, 118, 0.6); }\n"
        , -1, NULL);
    
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), 
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

// ============================================================
// PHẦN 3: LOGIC XỬ LÝ
// ============================================================
void on_day_button_click(GtkWidget *widget, gpointer data) {
    (void)widget;
    selected_day_index = GPOINTER_TO_INT(data);
    filter_flights(flights, flight_count, tem_flights, &tem_flight_count, airport_from, airport_to, days[selected_day_index], class, number_seat_order);
    refresh_ticket_list(ticket_list);
}

void on_detail_link_click(GtkWidget *widget, gpointer data) {
    (void)widget;
    char *flight_id = (char *)data;
    for (int i = 0; i < tem_flight_count; i++) {
        if (strcmp(tem_flights[i].flight_id, flight_id) == 0) {
            detail_flight = tem_flights[i];
            break;
        }
    }
    GtkWidget *detail_window = create_ticket_detail_window();
    set_content(detail_window);
}

void on_sort_changed(GtkComboBox *combo, gpointer user_data) {
    (void)user_data;
    gint active_index = gtk_combo_box_get_active(combo);
    
    extern void sort_flights(gboolean ascending);
    extern void sort_flights_by_duration(gboolean ascending);

    if (active_index == 0) sort_flights(TRUE); 
    else if (active_index == 1) sort_flights(FALSE); 
    else if (active_index == 2) sort_flights_by_duration(TRUE); 
    else if (active_index == 3) sort_flights_by_duration(FALSE); 

    refresh_ticket_list(ticket_list);
}

// ============================================================
// PHẦN 4: UI COMPONENTS
// ============================================================

GtkWidget* create_list_header(GtkWidget *main_box) {
    (void)main_box;

    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(header, 900, 60);
    gtk_widget_set_margin_top(header, 30);
    gtk_widget_set_halign(header, GTK_ALIGN_CENTER);

    GtkWidget *menu_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(menu_box), TRUE);
    gtk_box_pack_start(GTK_BOX(header), menu_box, TRUE, TRUE, 0);

    // 1. HOME
    GtkWidget *btn_home = gtk_button_new_with_label("HOME");
    gtk_widget_set_name(btn_home, "list-header-btn");
    g_signal_connect(btn_home, "clicked", G_CALLBACK(on_nav_home_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(menu_box), btn_home, TRUE, TRUE, 10);

    // 2. MY BOOKINGS (Nút thường)
    GtkWidget *btn_booking = gtk_button_new_with_label("MY BOOKINGS");
    gtk_widget_set_name(btn_booking, "list-header-btn");
    g_signal_connect(btn_booking, "clicked", G_CALLBACK(show_list_tickets), NULL);
    gtk_box_pack_start(GTK_BOX(menu_box), btn_booking, TRUE, TRUE, 10);

    // 3. MY ACCOUNT
    GtkWidget *btn_account = gtk_button_new_with_label("MY ACCOUNT");
    gtk_widget_set_name(btn_account, "list-header-btn");
    g_signal_connect(btn_account, "clicked", G_CALLBACK(on_nav_account_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(menu_box), btn_account, TRUE, TRUE, 10);

    // 4. NOTIFICATIONS
    GtkWidget *btn_noti = gtk_button_new_with_label("NOTIFICATIONS");
    gtk_widget_set_name(btn_noti, "list-header-btn");
    g_signal_connect(btn_noti, "clicked", G_CALLBACK(show_notification), NULL);
    gtk_box_pack_start(GTK_BOX(menu_box), btn_noti, TRUE, TRUE, 10);

    return header;
}

GtkWidget* create_day_selector() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(box, 30);
    gtk_widget_set_margin_bottom(box, 15);

    generate_next_5_days(date_tem_flight, days, &num_days);
    GtkWidget *first_btn = NULL;
    
    for (int i = 0; i < num_days; i++) {
        GtkWidget *day_button;
        if (i == 0) {
            day_button = gtk_radio_button_new_with_label(NULL, days[i]);
            first_btn = day_button;
        } else {
            day_button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(first_btn), days[i]);
        }
        gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(day_button), FALSE);
        GtkStyleContext *ctx = gtk_widget_get_style_context(day_button);
        gtk_style_context_add_class(ctx, "day-btn");
        
        if (i == selected_day_index) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(day_button), TRUE);

        g_signal_connect(day_button, "clicked", G_CALLBACK(on_day_button_click), GINT_TO_POINTER(i));
        gtk_widget_set_size_request(day_button, 140, 50);
        gtk_box_pack_start(GTK_BOX(box), day_button, FALSE, FALSE, 0);
    }
    return box;
}

GtkWidget* create_filter_box() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(box, GTK_ALIGN_END);
    gtk_widget_set_margin_end(box, 50);
    gtk_widget_set_margin_bottom(box, 20);

    GtkWidget *filter_label = gtk_label_new("SORT BY:");
    gtk_style_context_add_class(gtk_widget_get_style_context(filter_label), "filter-label");

    GtkWidget *combo_box = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Price: Low to High");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Price: High to Low");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Duration: Short to Long");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Duration: Long to Short");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 0);

    g_signal_connect(combo_box, "changed", G_CALLBACK(on_sort_changed), NULL);

    gtk_box_pack_start(GTK_BOX(box), filter_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), combo_box, FALSE, FALSE, 0);
    return box;
}

GtkWidget* create_ticket_card(Flight flight) {
    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkStyleContext *ctx = gtk_widget_get_style_context(card);
    gtk_style_context_add_class(ctx, "ticket-card");
    gtk_widget_set_size_request(card, 950, 130);

    GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(left_box, 30);
    gtk_widget_set_margin_top(left_box, 20);
    gtk_widget_set_margin_bottom(left_box, 20);

    GtkWidget *row_logo = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *icon = gtk_image_new_from_icon_name("airplane-mode-symbolic", GTK_ICON_SIZE_DND);
    GtkWidget *lbl_name = gtk_label_new(flight.airplane_name);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_name), "airline-label");
    gtk_box_pack_start(GTK_BOX(row_logo), icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row_logo), lbl_name, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left_box), row_logo, FALSE, FALSE, 0);

    GtkWidget *row_time = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_top(row_time, 15);

    char time_only[10], dummy[20];
    split_date_time(flight.departure_time, dummy, time_only);
    
    GtkWidget *col_dep = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *lbl_dep = gtk_label_new(time_only);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_dep), "time-huge");
    GtkWidget *lbl_dep_code = gtk_label_new(extract_middle_string(flight.departure_airport));
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_dep_code), "airport-code");
    gtk_widget_set_halign(lbl_dep_code, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(col_dep), lbl_dep, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(col_dep), lbl_dep_code, FALSE, FALSE, 0);

    GtkWidget *col_mid = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(col_mid, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(col_mid, 200, -1);
    GtkWidget *lbl_dur = gtk_label_new(convert_minutes_to_time_format(flight.duration_minutes));
    GtkWidget *visual = gtk_label_new("------- ✈ -------");
    GdkRGBA grey; gdk_rgba_parse(&grey, "#aaaaaa");
    gtk_widget_override_color(visual, GTK_STATE_FLAG_NORMAL, &grey);
    gtk_box_pack_start(GTK_BOX(col_mid), lbl_dur, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(col_mid), visual, FALSE, FALSE, 0);

    char *end_time_str = calculate_end_time(time_only, flight.duration_minutes * 60);
    GtkWidget *col_arr = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *lbl_arr = gtk_label_new(end_time_str);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_arr), "time-huge");
    GtkWidget *lbl_arr_code = gtk_label_new(extract_middle_string(flight.arrival_airport));
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_arr_code), "airport-code");
    gtk_widget_set_halign(lbl_arr_code, GTK_ALIGN_END);
    gtk_box_pack_start(GTK_BOX(col_arr), lbl_arr, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(col_arr), lbl_arr_code, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(row_time), col_dep, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row_time), col_mid, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row_time), col_arr, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left_box), row_time, FALSE, FALSE, 0);

    GtkWidget *right_section = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(right_section, 220, -1);
    gtk_widget_set_valign(right_section, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(right_section, GTK_ALIGN_CENTER);

    char price_buff[64];
    snprintf(price_buff, sizeof(price_buff), "%s VND", format_number_with_separator(flight.price, ','));
    GtkWidget *lbl_price = gtk_label_new(price_buff);
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_price), "price-big");
    
    GtkWidget *btn_select = gtk_button_new_with_label("SELECT");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_select), "btn-select");
    gtk_widget_set_size_request(btn_select, 140, 45);
    
    char *flight_id_copy = g_strdup(flight.flight_id); 
    g_signal_connect(btn_select, "clicked", G_CALLBACK(on_detail_link_click), flight_id_copy);

    gtk_box_pack_start(GTK_BOX(right_section), lbl_price, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_section), btn_select, FALSE, FALSE, 0);

    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_margin_top(separator, 15);
    gtk_widget_set_margin_bottom(separator, 15);

    gtk_box_pack_start(GTK_BOX(card), left_box, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(card), separator, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(card), right_section, FALSE, FALSE, 0);

    return card;
}

GtkWidget* create_ticket_list() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);

    if (tem_flight_count == 0) {
        GtkWidget *no_flight_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(no_flight_label), "<span size='xx-large' color='white' weight='bold'>No flights found.</span>");
        gtk_box_pack_start(GTK_BOX(box), no_flight_label, TRUE, TRUE, 50);
        return box;
    }

    for (int i = 0; i < tem_flight_count; i++) {
        GtkWidget *card = create_ticket_card(tem_flights[i]);
        gtk_box_pack_start(GTK_BOX(box), card, FALSE, FALSE, 0);
    }
    return box;
}

void refresh_ticket_list(GtkWidget *container) {
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(ticket_list));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    GtkWidget *new_list = create_ticket_list();
    gtk_box_pack_start(GTK_BOX(container), new_list, TRUE, TRUE, 0);
    gtk_widget_show_all(container); 
}

void sort_flights(gboolean ascending) {
    for (int i = 0; i < tem_flight_count; i++) {
        for (int j = i + 1; j < tem_flight_count; j++) {
            if ((ascending && tem_flights[i].price > tem_flights[j].price) ||
                (!ascending && tem_flights[i].price < tem_flights[j].price)) {
                Flight temp = tem_flights[i];
                tem_flights[i] = tem_flights[j];
                tem_flights[j] = temp;
            }
        }
    }
}

void sort_flights_by_duration(gboolean ascending) {
    for (int i = 0; i < tem_flight_count; i++) {
        for (int j = i + 1; j < tem_flight_count; j++) {
            if ((ascending && tem_flights[i].duration_minutes > tem_flights[j].duration_minutes) ||
                (!ascending && tem_flights[i].duration_minutes < tem_flights[j].duration_minutes)) {
                Flight temp = tem_flights[i];
                tem_flights[i] = tem_flights[j];
                tem_flights[j] = temp;
            }
        }
    }
}

GtkWidget* create_list_window() {
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(scrolled_window), main_box);
    
    apply_list_css();

    GtkWidget *header = create_list_header(main_box);
    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 0);

    GtkWidget *day_selector = create_day_selector();
    gtk_box_pack_start(GTK_BOX(main_box), day_selector, FALSE, FALSE, 0);

    GtkWidget *filter_box = create_filter_box();
    gtk_box_pack_start(GTK_BOX(main_box), filter_box, FALSE, FALSE, 10);

    ticket_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *initial_list = create_ticket_list();
    gtk_box_pack_start(GTK_BOX(ticket_list), initial_list, TRUE, TRUE, 0);
    
    gtk_box_pack_start(GTK_BOX(main_box), ticket_list, TRUE, TRUE, 30);

    return scrolled_window;
}