#include <gtk/gtk.h>
#include "my_account.h"
#include "../global/global.h"
#include "../login/login.h" 
#include "../homepage/homepage.h" // [QUAN TRỌNG] Thêm header này
#include <string.h>

// Helper để tạo dòng thông tin (Label: Value)
GtkWidget* create_info_row(const char *label_text, const char *value_text) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    
    GtkWidget *lbl_key = gtk_label_new(label_text);
    GtkWidget *lbl_val = gtk_label_new(value_text);
    
    // CSS cho Key
    GtkStyleContext *ctx_key = gtk_widget_get_style_context(lbl_key);
    gtk_style_context_add_class(ctx_key, "info-key");
    gtk_widget_set_size_request(lbl_key, 120, -1);
    gtk_widget_set_halign(lbl_key, GTK_ALIGN_START);

    // CSS cho Value
    GtkStyleContext *ctx_val = gtk_widget_get_style_context(lbl_val);
    gtk_style_context_add_class(ctx_val, "info-val");

    gtk_box_pack_start(GTK_BOX(box), lbl_key, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), lbl_val, FALSE, FALSE, 0);
    
    return box;
}

// --- CÁC HÀM CHUYỂN TRANG AN TOÀN (SAFE SWITCH) ---
// Dùng g_idle_add để tách việc chuyển trang ra khỏi sự kiện click hiện tại

gboolean safe_switch_to_login(gpointer data) {
    (void)data;
    // Reset thông tin
    user_id = 0;
    strcpy(current_user_full_name, "GUEST USER");
    
    GtkWidget *login_window = create_login_window();
    set_content(login_window);
    return FALSE; // Chỉ chạy 1 lần rồi dừng
}

void on_logout_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;

    // --- THÊM ĐOẠN NÀY (BẮT ĐẦU) ---
    // Gửi thông báo LOGOUT lên server để server biết mà reset trạng thái
    char *msg = "LOGOUT";
    send(sock, msg, strlen(msg), 0);
    // --- THÊM ĐOẠN NÀY (KẾT THÚC) ---

    // Đợi xử lý xong sự kiện click rồi mới chuyển trang
    g_idle_add(safe_switch_to_login, NULL);
}
gboolean safe_switch_to_home(gpointer data) {
    (void)data;
    GtkWidget *home = create_homepage_window();
    set_content(home);
    return FALSE; // Chỉ chạy 1 lần rồi dừng
}

void on_back_home_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    // Đợi xử lý xong sự kiện click rồi mới chuyển trang
    g_idle_add(safe_switch_to_home, NULL);
}

// --------------------------------------------------

GtkWidget* create_my_account_window() {
    // 1. CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        // --- SỬA ĐỔI QUAN TRỌNG TẠI ĐÂY ---
        // Sử dụng !important để ép buộc đổi màu nền thành xanh (#E3F2FD)
        // và background-image: none để xóa gradient nếu bị dính từ trang khác.
        "window { background-color: #E3F2FD !important; background-image: none !important; }"
        
        "#profile-card { background-color: white; border-radius: 15px; padding: 40px; box-shadow: 0 4px 15px rgba(0,0,0,0.1); }"
        ".info-key { font-weight: bold; color: #555; font-size: 14px; }"
        ".info-val { font-weight: normal; color: #223A60; font-size: 16px; }"
        "#btn-logout { background-color: #ff4d4d; color: black; border-radius: 8px; font-weight: bold; padding: 10px; }"
        "#btn-back { background-color: #ccc; color: black; border-radius: 8px; font-weight: bold; padding: 10px; }"
        , -1, NULL);
    
    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // 2. Layout chính
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(main_box, 50);

    // 3. Profile Card
    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_name(card, "profile-card");
    gtk_widget_set_size_request(card, 500, -1);

    // Avatar
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale("../assets/images/user.png", 100, 100, TRUE, NULL);
    GtkWidget *avatar;
    if (pixbuf) {
        avatar = gtk_image_new_from_pixbuf(pixbuf);
        g_object_unref(pixbuf);
    } else {
        avatar = gtk_label_new("[AVATAR]");
    }
    gtk_box_pack_start(GTK_BOX(card), avatar, FALSE, FALSE, 0);

    // Tiêu đề
    GtkWidget *title = gtk_label_new(current_user_full_name);
    
    // Tạo Attributes cho font đậm và to
    PangoAttrList *attr = pango_attr_list_new(); 
    pango_attr_list_insert(attr, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
    pango_attr_list_insert(attr, pango_attr_scale_new(1.5)); 
    gtk_label_set_attributes(GTK_LABEL(title), attr);
    pango_attr_list_unref(attr);

    gtk_box_pack_start(GTK_BOX(card), title, FALSE, FALSE, 10);

    // Thông tin chi tiết
    gtk_box_pack_start(GTK_BOX(card), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);
    
    char uid_str[20];
    sprintf(uid_str, "%d", user_id);
    
    gtk_box_pack_start(GTK_BOX(card), create_info_row("User ID:", uid_str), FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(card), create_info_row("Email:", email_user), FALSE, FALSE, 5);

    // Nút chức năng
    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(btn_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(btn_box, 30);

    GtkWidget *btn_back = gtk_button_new_with_label("Back to Home");
    gtk_widget_set_name(btn_back, "btn-back");
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_back_home_clicked), NULL);

    GtkWidget *btn_logout = gtk_button_new_with_label("Log Out");
    gtk_widget_set_name(btn_logout, "btn-logout");
    g_signal_connect(btn_logout, "clicked", G_CALLBACK(on_logout_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(btn_box), btn_back, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(btn_box), btn_logout, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(card), btn_box, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(main_box), card, TRUE, TRUE, 0);
    return main_box;
}