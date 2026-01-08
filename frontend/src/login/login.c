#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "login.h"
#include "../auth/auth.h"
#include "../homepage/homepage.h"
#include "../register/register.h"
#include "../global/global.h"

/* -------- Biến toàn cục -------- */
GtkWidget *entry_email;
GtkWidget *entry_password;
GtkWidget *label_status;

/* Base path cho ảnh - Hãy đảm bảo đường dẫn này đúng với máy của bạn */
#define IMG(path) "../assets/images/" path
/* ===== Hàm tiện ích: tạo ô nhập có icon ===== */
static GtkWidget* add_icon_entry(const char *icon_path,
                                 const char *placeholder,
                                 GtkWidget **store_entry)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_name(box, "input-box");

    GError *err = NULL;
    GdkPixbuf *raw = gdk_pixbuf_new_from_file(icon_path, &err);
    if (err != NULL) {
        g_printerr("Load icon error: %s\n", err->message);
        g_error_free(err);
        // Fallback icon placeholder
        raw = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 22, 22);
    }
    GdkPixbuf *scaled = gdk_pixbuf_scale_simple(raw, 22, 22, GDK_INTERP_BILINEAR);
    GtkWidget *icon = gtk_image_new_from_pixbuf(scaled);

    g_object_unref(raw);
    g_object_unref(scaled);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
    gtk_widget_set_name(entry, "entry-field");

    gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 6);
    gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 6);

    *store_entry = entry;
    return box;
}

/* ===== CALLBACKS ===== */

/* [QUAN TRỌNG] Hàm này bị thiếu trong file cũ của bạn, gây ra lỗi undefined reference */
void on_register_link_click(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    GtkWidget *register_content = create_register_window();
    set_content(register_content);
}

void on_login_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    const char *email = gtk_entry_get_text(GTK_ENTRY(entry_email));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entry_password));

    if (strlen(email) == 0 || strlen(password) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Please fill in all fields!");
        return;
    }

    if (!validate_email(email)) {
        gtk_label_set_text(GTK_LABEL(label_status), "Email is not valid!");
        return;
    }

    strcpy(email_user, email);
    snprintf(buffer, MAX_LENGTH, "LOGIN %s:%s", email, password);
    send(sock, buffer, sizeof(buffer), 0);
    
    // Nhận phản hồi từ server
    int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (len > 0) buffer[len] = '\0';
    else buffer[0] = '\0';

    if (strncmp(buffer, "SUCCESS:", 8) == 0) {
        // Parse ID và Name từ chuỗi "SUCCESS:123:TenNguoiDung"
        char *p = buffer + 8;
        char *token = strtok(p, ":");
        if(token) user_id = atoi(token);
        
        token = strtok(NULL, ":");
        if(token) {
            strcpy(current_user_full_name, token);
        } else {
            strcpy(current_user_full_name, "GUEST USER");
        }
        
        printf("Logged in as: %s (ID: %d)\n", current_user_full_name, user_id);

        GtkWidget *homepage_widget = create_homepage_window();
        set_content(homepage_widget);
    } else if (strcmp(buffer, "EMAIL_NOT_FOUND") == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Email not found, please register email");
    } else {
        gtk_label_set_text(GTK_LABEL(label_status), "Login failed, please try again");
    }
}

/* ===== UI FILE ===== */
GtkWidget *create_login_window(void) {
    /* ===== CSS ===== */
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "window { background: #F3F2FF; }\n"
        "#card { background: white; border-radius: 22px; padding: 38px; min-width: 360px; box-shadow: 0 6px 28px rgba(0,0,0,0.15); }\n"
        "#title { font-size: 24px; font-weight: 700; color: #4636E3; margin-bottom: 22px; }\n"
        "#input-box { background: white; border: 2px solid #C8C4F4; border-radius: 14px; padding: 6px; margin-bottom: 12px; }\n"
        "#entry-field { border: none; background: transparent; padding: 8px; font-size: 14px; }\n"
        "#btn-login { background: #4636E3; color: white; border-radius: 16px; padding: 12px; margin-top: 10px; font-weight: 600; }\n"
        "#btn-login:hover { background: #3529C7; }\n"
        "#status-label { color: #C62828; font-size: 12px; margin-top: 8px; }\n"
        "#footer { color: #555; margin-top: 10px; }\n"
        "#register-link { background: transparent; color: #4636E3; border: none; }\n"
        "#register-link:hover { text-decoration: underline; }\n",
        -1, NULL);
    
    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    /* ===== CARD ===== */
    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_name(card, "card");
    gtk_widget_set_halign(card, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(card, GTK_ALIGN_CENTER);

    /* Avatar */
    GdkPixbuf *pbuf = gdk_pixbuf_new_from_file_at_scale(IMG("user.png"), 85, 85, TRUE, NULL);
    if (!pbuf) pbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 85, 85);
    GtkWidget *avatar = gtk_image_new_from_pixbuf(pbuf);
    g_object_unref(pbuf);
    gtk_box_pack_start(GTK_BOX(card), avatar, FALSE, FALSE, 0);

    /* Title */
    GtkWidget *title = gtk_label_new("Log in");
    gtk_widget_set_name(title, "title");
    gtk_box_pack_start(GTK_BOX(card), title, FALSE, FALSE, 0);

    /* Email field */
    gtk_box_pack_start(GTK_BOX(card), add_icon_entry(IMG("icon_user.png"), "Email", &entry_email), FALSE, FALSE, 0);

    /* Password field */
    GtkWidget *pw_box = add_icon_entry(IMG("icon_lock.png"), "Password", &entry_password);
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_box_pack_start(GTK_BOX(card), pw_box, FALSE, FALSE, 0);

    /* Status */
    label_status = gtk_label_new("");
    gtk_widget_set_name(label_status, "status-label");
    gtk_box_pack_start(GTK_BOX(card), label_status, FALSE, FALSE, 0);

    /* Login button */
    GtkWidget *btn_login = gtk_button_new_with_label("Log in");
    gtk_widget_set_name(btn_login, "btn-login");
    g_signal_connect(btn_login, "clicked", G_CALLBACK(on_login_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(card), btn_login, FALSE, FALSE, 0);

    /* Footer */
    GtkWidget *footer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *footer_text = gtk_label_new("Don't have an account?");
    gtk_widget_set_name(footer_text, "footer");

    GtkWidget *register_btn = gtk_button_new_with_label("Register");
    gtk_widget_set_name(register_btn, "register-link");
    g_signal_connect(register_btn, "clicked", G_CALLBACK(on_register_link_click), NULL);

    gtk_box_pack_start(GTK_BOX(footer), footer_text, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(footer), register_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card), footer, FALSE, FALSE, 6);

    return card;
}