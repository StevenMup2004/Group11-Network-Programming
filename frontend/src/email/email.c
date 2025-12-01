#include "email.h"
#include <string.h>
#include <curl/curl.h>
#include <gtk/gtk.h>
#include <time.h>
#include <stdlib.h>
#include "../global/global.h" // [Quan trọng] Include để lấy thông tin vé và user

// =======================================================================
// CẤU HÌNH NGƯỜI GỬI (SENDER)
// Lưu ý: Thay đổi thông tin này nếu bạn muốn gửi từ tài khoản của chính mình.
// Mật khẩu ở đây là "App Password" (Mật khẩu ứng dụng) của Google.
// =======================================================================
#define SENDER_EMAIL "dvuhai36@gmail.com"      
#define SENDER_PASSWORD "pnrd mpsa pkbc iekv"       

// Cấu trúc để quản lý việc upload nội dung email
struct upload_status {
    const char *payload; // Con trỏ tới nội dung email đã soạn sẵn
    size_t bytes_read;   // Đếm số byte đã gửi
};

// Callback đọc dữ liệu để gửi đi qua CURL
static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    struct upload_status *upload_ctx = (struct upload_status *)userp;
    const char *data = upload_ctx->payload;
    size_t room = size * nmemb;
    size_t len = strlen(data);

    if (upload_ctx->bytes_read < len) {
        size_t to_copy = len - upload_ctx->bytes_read;
        if (to_copy > room) {
            to_copy = room;
        }
        memcpy(ptr, data + upload_ctx->bytes_read, to_copy);
        upload_ctx->bytes_read += to_copy;
        return to_copy;
    }
    return 0;
}

// Hàm chính để gửi email
int send_email() {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx = {0};
    
    // -----------------------------------------------------------
    // 1. CHUẨN BỊ DỮ LIỆU (DATA PREPARATION)
    // -----------------------------------------------------------
    
    // Xử lý danh sách ghế
    char seats_str[1024] = "N/A";
    if (temp_seats && tem_seats_size > 0) {
        char *joined = join_strings(temp_seats, tem_seats_size, ", ");
        if (joined) {
            strncpy(seats_str, joined, sizeof(seats_str) - 1);
            free(joined);
        }
    }

    // Định dạng tiền tệ
    char *price_str = format_number_with_separator(final_price, ',');
    
    // Lấy tên sân bay (để hiển thị đẹp hơn mã sân bay)
    char *dep_name = extract_middle_string(detail_flight.departure_airport);
    char *arr_name = extract_middle_string(detail_flight.arrival_airport);

    // -----------------------------------------------------------
    // 2. SOẠN NỘI DUNG EMAIL (PROFESSIONAL ENGLISH FORMAT)
    // -----------------------------------------------------------
    size_t payload_len = 4096;
    char *payload_text = (char *)malloc(payload_len);
    if (!payload_text) return 1;

    // Sử dụng Text-based layout để giả lập giao diện vé điện tử
    snprintf(payload_text, payload_len,
        "To: %s\r\n"
        "From: Flight Booking <%s>\r\n"
        "Subject: Booking Confirmation: Flight %s to %s\r\n"
        "\r\n" // Dòng trống bắt buộc phân cách Header và Body
        "Dear Customer,\r\n"
        "\r\n"
        "Thank you for choosing VienamTravel. Your booking is confirmed. We are ready for your departure!\r\n"
        "\r\n"
        "Here is your e-ticket itinerary:\r\n"
        "\r\n"
        "================================================\r\n"
        "              FLIGHT CONFIRMATION               \r\n"
        "================================================\r\n"
        " Booking Reference: #%d\r\n"
        " Flight Number:     %s\r\n"
        "================================================\r\n"
        "\r\n"
        " DEPARTURE\r\n"
        " [ %s ]\r\n" // Tên sân bay đi
        " Time: %s\r\n"
        "\r\n"
        "      |\r\n"
        "      |  Non-stop\r\n"
        "      V\r\n"
        "\r\n"
        " ARRIVAL\r\n"
        " [ %s ]\r\n" // Tên sân bay đến
        "\r\n"
        "================================================\r\n"
        " PASSENGER & SEAT DETAILS\r\n"
        "================================================\r\n"
        " Seat(s):         %s\r\n"
        " Total Payment:   %s VND\r\n"
        " Status:          Confirmed / Paid\r\n"
        "================================================\r\n"
        "\r\n"
        "IMPORTANT NOTES:\r\n"
        "* Please arrive at the airport at least 2 hours before your scheduled departure time.\r\n"
        "* Have your valid ID/Passport and this confirmation email ready for check-in.\r\n"
        "\r\n"
        "Need help? Contact us at support@VietnamTravel.com\r\n"
        "\r\n"
        "Safe Travels,\r\n"
        "VietnamTravel Team\r\n",
        
        // --- Mapping Variables ---
        email_user,                 // To: Email người dùng đăng nhập
        SENDER_EMAIL,               // From: Email hệ thống
        detail_flight.flight_id,    // Subject: Flight ID
        arr_name ? arr_name : "Destination", // Subject: Destination
        
        booking_id,                 // Booking Ref
        detail_flight.flight_id,    // Flight Number
        
        dep_name ? dep_name : detail_flight.departure_airport, // From
        detail_flight.departure_time,                          // Time
        
        arr_name ? arr_name : detail_flight.arrival_airport,   // To
        
        seats_str,                  // Seats
        price_str ? price_str : "0" // Price
    );

    // Dọn dẹp bộ nhớ tạm
    if (price_str) free(price_str);
    if (dep_name) free(dep_name);
    if (arr_name) free(arr_name);

    // Gán vào upload context
    upload_ctx.payload = payload_text;
    upload_ctx.bytes_read = 0;

    // -----------------------------------------------------------
    // 3. CẤU HÌNH CURL VÀ GỬI MAIL
    // -----------------------------------------------------------
    curl = curl_easy_init();
    if (curl) {
        // Cấu hình Server SMTP Gmail
        curl_easy_setopt(curl, CURLOPT_USERNAME, SENDER_EMAIL);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, SENDER_PASSWORD);
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        
        // Bảo mật
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        
        // Người gửi
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, SENDER_EMAIL);

        // Người nhận (Thêm email người dùng vào danh sách nhận)
        recipients = curl_slist_append(recipients, email_user);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Đọc dữ liệu từ callback payload_source
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        // Thực hiện gửi
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Dọn dẹp CURL
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    free(payload_text); // Giải phóng nội dung email
    return (res == CURLE_OK) ? 0 : 1;
}

// Xử lý sự kiện click nút Email (Giữ lại cho mục đích test riêng lẻ)
static void on_email_button_clicked(GtkWidget *widget, gpointer user_data) {
    if (send_email() == 0) {
        g_print("Email sent successfully!\n");
    } else {
        g_print("Failed to send email.\n");
    }
}

// Hàm vẽ nền giao diện (cho màn hình test email riêng lẻ)
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9); // Màu xám nhạt
    cairo_paint(cr);
    return FALSE;
}

// Tạo màn hình test email (nếu cần dùng độc lập)
GtkWidget *create_email_screen() {
    GtkWidget *overlay = gtk_overlay_new();

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), drawing_area);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw_event), NULL);

    GtkWidget *button = gtk_button_new_with_label("Email");
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), button);
    gtk_widget_set_size_request(button, 150, 50);
    gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(button, GTK_ALIGN_CENTER);

    g_signal_connect(button, "clicked", G_CALLBACK(on_email_button_clicked), NULL);

    return overlay;
}