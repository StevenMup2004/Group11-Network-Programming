#include <gtk/gtk.h>
#include "component.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include "../global/global.h"
#include "../homepage/homepage.h"
#include "../server_com/server_com.h"
#include "../booklist/booklist.h"

// --- CSS CHO NOTIFICATION ---
static void apply_notification_css() {
    static gboolean css_loaded = FALSE;
    if (css_loaded) return; 

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        ".notification-popover { background-color: white; padding: 15px; border-radius: 12px; border: 1px solid #eaeaea; box-shadow: 0 5px 15px rgba(0,0,0,0.1); }\n"
        ".noti-header { font-weight: 800; font-size: 16px; color: #223A60; margin-bottom: 10px; border-bottom: 2px solid #f0f0f0; padding-bottom: 5px; }\n"
        ".noti-item { background-color: white; padding: 12px; border-bottom: 1px solid #f0f0f0; transition: background 0.2s; }\n"
        ".noti-item:hover { background-color: #f9fbfd; }\n"
        ".noti-content { color: #333; font-size: 14px; line-height: 1.4; }\n"
        ".noti-date { color: #999; font-size: 11px; margin-top: 5px; font-weight: 600; }\n"
        ".no-noti { color: #888; font-style: italic; padding: 20px; text-align: center; }\n"
        , -1, NULL);
    
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), 
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    css_loaded = TRUE;
}

// --- LOGIC HIỂN THỊ THÔNG BÁO ---
void get_notifications_data() {
    send(sock, "GET ANNOUNCES", strlen("GET ANNOUNCES"), 0);
    recv(sock, &announce_count, sizeof(announce_count), 0);
    
    int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        parse_buffer_to_announces(buffer, list_announces);
    }
}

void show_notification(GtkWidget *widget, gpointer data) {
    if (widget == NULL) return;

    apply_notification_css(); 

    // --- SỬA LỖI TẠI ĐÂY: Luôn lấy danh sách vé mới nhất của user ---
    // Điều này đảm bảo dù ở màn hình Home chưa load vé, ta vẫn có dữ liệu để lọc thông báo
    get_list_tickets_ordered(); 
    // ----------------------------------------------------------------

    // Lấy danh sách thông báo từ server
    get_notifications_data();
    
    // Lọc thông báo theo vé của user
    filtered_announce_count = filter_announces_by_tickets(list_announces, announce_count, list_tickets, ticket_count, filtered_announces);

    // Tạo Popover
    GtkWidget *popover = gtk_popover_new(widget);
    gtk_style_context_add_class(gtk_widget_get_style_context(popover), "notification-popover");
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    gtk_container_set_border_width(GTK_CONTAINER(popover), 0);

    // Main Box
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(main_box, 320, -1); 

    // Header
    GtkWidget *header_lbl = gtk_label_new("Notifications");
    gtk_widget_set_halign(header_lbl, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(header_lbl), "noti-header");
    gtk_box_pack_start(GTK_BOX(main_box), header_lbl, FALSE, FALSE, 0);

    // Scrolled Window
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled, -1, 350); 
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    
    GtkWidget *list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    if (filtered_announce_count == 0) {
        GtkWidget *no_noti = gtk_label_new("You have no new notifications.");
        gtk_style_context_add_class(gtk_widget_get_style_context(no_noti), "no-noti");
        gtk_box_pack_start(GTK_BOX(list_box), no_noti, TRUE, TRUE, 20);
    } else {
        for (int i = 0; i < filtered_announce_count; i++) {
            GtkWidget *item_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_style_context_add_class(gtk_widget_get_style_context(item_box), "noti-item");

            // Nội dung
            GtkWidget *message_label = gtk_label_new(filtered_announces[i].content);
            gtk_label_set_line_wrap(GTK_LABEL(message_label), TRUE);
            gtk_label_set_max_width_chars(GTK_LABEL(message_label), 40);
            gtk_label_set_xalign(GTK_LABEL(message_label), 0.0);
            gtk_style_context_add_class(gtk_widget_get_style_context(message_label), "noti-content");
            
            // Ngày tháng
            GtkWidget *date_label = gtk_label_new(filtered_announces[i].updated_at);
            gtk_label_set_xalign(GTK_LABEL(date_label), 0.0);
            gtk_style_context_add_class(gtk_widget_get_style_context(date_label), "noti-date");

            gtk_box_pack_start(GTK_BOX(item_box), message_label, FALSE, FALSE, 0);
            gtk_box_pack_start(GTK_BOX(item_box), date_label, FALSE, FALSE, 0);
            
            gtk_box_pack_start(GTK_BOX(list_box), item_box, FALSE, FALSE, 0);
        }
    }

    gtk_container_add(GTK_CONTAINER(scrolled), list_box);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(popover), main_box);
    gtk_widget_show_all(popover);
    gtk_popover_popup(GTK_POPOVER(popover));
}

// --- CÁC HÀM KHÁC ---

void on_home_button_clicked(GtkWidget *widget, gpointer data) {
    if (widget == NULL) return;
    GtkWidget *homepage_widget = create_homepage_window();
    set_content(homepage_widget);
}

void show_list_tickets(GtkWidget *widget, gpointer data){
     if (widget == NULL) return;
    if (get_list_tickets_ordered() == -1){
        printf("Get list tickets error\n");
        return;
    }
    GtkWidget *book_list_window = create_booklist_window();
    set_content(book_list_window);
}

static void on_button_toggled(GtkToggleButton *button, gpointer user_data) {
    GtkWidget **buttons = (GtkWidget **)user_data;
    if (buttons == NULL) return;

    if (gtk_toggle_button_get_active(button)) {
        for (int i = 0; buttons[i] != NULL; i++) {
            if (buttons[i] != GTK_WIDGET(button)) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttons[i]), FALSE);
            }
        }
    }
}

GtkWidget* create_header(GtkWidget **buttons, GtkWidget *parent_container) {
    GtkWidget *header, *logo, *menu_box, *home_button, *ticket_button, *account_button, *notification_button;

    header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(header, 1060, 40);
    gtk_widget_set_name(header, "header");
    gtk_widget_set_margin_top(header, 10);
    gtk_widget_set_margin_start(header, 20);
    gtk_widget_set_margin_end(header, 280);
    gtk_widget_set_margin_bottom(header, 10);

    // logo = gtk_image_new_from_file("../assets/images/logo.png");
    // gtk_box_pack_start(GTK_BOX(header), logo, FALSE, FALSE, 10);
    //gtk_widget_set_margin_end(logo, 250);

    menu_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(menu_box), TRUE);

    home_button = gtk_toggle_button_new_with_label("Home");
    ticket_button = gtk_toggle_button_new_with_label("Ticket");
    account_button = gtk_toggle_button_new_with_label("Account");
    notification_button = gtk_toggle_button_new_with_label("Notification");

    GtkCssProvider *button_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(button_provider,
        "button { font-family: 'Poppins', sans-serif; font-size: 16px; font-weight: 600; letter-spacing: 0.06em; background: transparent; color: #283841; padding: 10px 10px; border-radius: 10px; text-align: center; border: none; margin-right: 20px; }"
        "button:hover { background: #223A60; font-weight: 800; cursor: pointer; color: white; }"
        "button:active { background: #223A60; color: white; }"
        "button:checked { background: #223A60; color: white; }",
        -1, NULL);

    gtk_style_context_add_provider(gtk_widget_get_style_context(home_button), GTK_STYLE_PROVIDER(button_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(gtk_widget_get_style_context(ticket_button), GTK_STYLE_PROVIDER(button_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(gtk_widget_get_style_context(account_button), GTK_STYLE_PROVIDER(button_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(gtk_widget_get_style_context(notification_button), GTK_STYLE_PROVIDER(button_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_set_size_request(home_button, 100, 40);
    gtk_widget_set_size_request(ticket_button, 100, 40);
    gtk_widget_set_size_request(account_button, 100, 40);
    gtk_widget_set_size_request(notification_button, 130, 40);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(home_button), TRUE);

    gtk_box_pack_start(GTK_BOX(menu_box), home_button, TRUE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(menu_box), ticket_button, TRUE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(menu_box), account_button, TRUE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(menu_box), notification_button, TRUE, TRUE, 10);

    gtk_box_pack_start(GTK_BOX(header), menu_box, TRUE, TRUE, 0);

    buttons[0] = home_button;
    buttons[1] = ticket_button;
    buttons[2] = account_button;
    buttons[3] = notification_button;
    buttons[4] = NULL;

    g_signal_connect(home_button, "toggled", G_CALLBACK(on_button_toggled), buttons);
    g_signal_connect(account_button, "toggled", G_CALLBACK(on_button_toggled), buttons);
    
    g_signal_connect(ticket_button, "clicked",G_CALLBACK(show_list_tickets), NULL);
    g_signal_connect(notification_button, "clicked",G_CALLBACK(show_notification), NULL);
    g_signal_connect(home_button, "clicked", G_CALLBACK(on_home_button_clicked), NULL);

    return header;
}