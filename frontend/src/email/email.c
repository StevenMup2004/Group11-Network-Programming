#include "email.h"
#include <string.h>
#include <curl/curl.h>
#include <gtk/gtk.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <cairo/cairo-pdf.h> 
#include "../global/global.h"

// =======================================================================
// CẤU HÌNH GỬI EMAIL
// =======================================================================
#define SENDER_EMAIL "dvuhai36@gmail.com"      
#define SENDER_PASSWORD "pnrd mpsa pkbc iekv"       
#define PDF_FILENAME "ticket.pdf"

#define PDF_WIDTH 600
#define PDF_HEIGHT 750

struct upload_status {
    const char *payload;
    size_t bytes_read;
};

// =======================================================================
// PHẦN 1: HÀM HỖ TRỢ VẼ PDF (Giao diện Boarding Pass)
// =======================================================================

// Vẽ đường nét đứt
static void draw_dashed_line(cairo_t *cr, double x1, double y1, double x2, double y2) {
    cairo_save(cr);
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5); 
    cairo_set_line_width(cr, 1.0);
    const double dashes[] = {4.0};
    cairo_set_dash(cr, dashes, 1, 0);
    cairo_move_to(cr, x1, y1);
    cairo_line_to(cr, x2, y2);
    cairo_stroke(cr);
    cairo_restore(cr);
}

// Vẽ nội dung vé lên PDF
static void draw_ticket_for_pdf(cairo_t *cr, double width, double height) {
    char buffer[512];
    char date_only[20], time_only[10];
    split_date_time(detail_flight.departure_time, date_only, time_only);

    // Xử lý chuỗi ghế cho PDF
    char seats_str[1024] = "N/A";
    if (temp_seats && tem_seats_size > 0) {
        char *joined = join_strings(temp_seats, tem_seats_size, ", ");
        if (joined) {
            strncpy(seats_str, joined, sizeof(seats_str) - 1);
            free(joined);
        }
    }

    // 1. NỀN VÉ
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);

    // 2. HEADER
    cairo_set_source_rgb(cr, 0.13, 0.23, 0.38); // Màu xanh #223A60
    cairo_rectangle(cr, 0, 0, width, 70);
    cairo_fill(cr);

    // Text Header
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 28);
    cairo_move_to(cr, 30, 45);
    cairo_show_text(cr, "BOARDING PASS");

    // Booking Ref
    cairo_set_font_size(cr, 14);
    snprintf(buffer, sizeof(buffer), "Ref: #%d", booking_id);
    cairo_move_to(cr, width - 150, 45);
    cairo_show_text(cr, buffer);

    // Tên hãng bay
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2); 
    cairo_set_font_size(cr, 18);
    cairo_move_to(cr, 30, 100);
    cairo_show_text(cr, "VietnamTravel Airlines"); 

    // 3. FROM (Ở trên)
    cairo_set_font_size(cr, 12);
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5); // Màu xám
    cairo_move_to(cr, 30, 135);
    cairo_show_text(cr, "FROM");

    cairo_set_font_size(cr, 30); // Chữ to
    cairo_set_source_rgb(cr, 0.13, 0.23, 0.38); // Màu xanh
    cairo_move_to(cr, 30, 165);
    cairo_show_text(cr, extract_middle_string(detail_flight.departure_airport));

    // 4. TO (Xuống dòng dưới)
    cairo_set_font_size(cr, 12);
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_move_to(cr, 30, 205); 
    cairo_show_text(cr, "TO");

    cairo_set_font_size(cr, 30);
    cairo_set_source_rgb(cr, 0.13, 0.23, 0.38);
    cairo_move_to(cr, 30, 235);
    cairo_show_text(cr, extract_middle_string(detail_flight.arrival_airport));

    // 5. KHỐI GHẾ (SEAT)
    cairo_set_source_rgb(cr, 0.95, 0.95, 0.95);
    cairo_rectangle(cr, 400, 120, 160, 100); 
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, 420, 150);
    cairo_show_text(cr, "SEAT NUMBER");

    cairo_set_source_rgb(cr, 0.8, 0.2, 0.2); // Màu đỏ
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 40); 
    cairo_move_to(cr, 420, 195);
    cairo_show_text(cr, seats_str);

    // 6. THÔNG TIN CHI TIẾT
    double row_y = 290;
    
    // Cột Flight
    cairo_set_font_size(cr, 10);
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_move_to(cr, 30, row_y);
    cairo_show_text(cr, "FLIGHT");
    cairo_set_font_size(cr, 16);
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_move_to(cr, 30, row_y + 20);
    cairo_show_text(cr, detail_flight.flight_id);

    // Cột Date
    cairo_set_font_size(cr, 10);
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_move_to(cr, 150, row_y);
    cairo_show_text(cr, "DATE");
    cairo_set_font_size(cr, 16);
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_move_to(cr, 150, row_y + 20);
    cairo_show_text(cr, date_only);

    // Cột Time
    cairo_set_font_size(cr, 10);
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_move_to(cr, 300, row_y);
    cairo_show_text(cr, "TIME");
    cairo_set_font_size(cr, 16);
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_move_to(cr, 300, row_y + 20);
    cairo_show_text(cr, time_only);

    // 7. FOOTER
    draw_dashed_line(cr, 0, height - 60, width, height - 60);

    cairo_set_source_rgb(cr, 0.2, 0.6, 0.2); // Xanh lá
    cairo_set_font_size(cr, 20);
    snprintf(buffer, sizeof(buffer), "TOTAL: %s VND", format_number_with_separator(final_price, ','));
    cairo_move_to(cr, 30, height - 25);
    cairo_show_text(cr, buffer);
}

// Tạo file PDF
void generate_ticket_pdf(const char *filename) {
    cairo_surface_t *surface;
    cairo_t *cr;
    surface = cairo_pdf_surface_create(filename, PDF_WIDTH, PDF_HEIGHT);
    cr = cairo_create(surface);
    draw_ticket_for_pdf(cr, PDF_WIDTH, PDF_HEIGHT);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

// =======================================================================
// PHẦN 2: HELPER GỬI EMAIL (Base64 & Callback)
// =======================================================================

static char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {
    static const char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
    *output_length = 4 * ((input_length + 2) / 3);
    char *encoded_data = malloc(*output_length + 1);
    if (encoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }
    for (int i = 0; i < (3 - input_length % 3) % 3; i++) encoded_data[*output_length - 1 - i] = '=';
    encoded_data[*output_length] = '\0';
    return encoded_data;
}

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    struct upload_status *upload_ctx = (struct upload_status *)userp;
    const char *data = upload_ctx->payload;
    size_t room = size * nmemb;
    size_t len = strlen(data);
    if (upload_ctx->bytes_read < len) {
        size_t to_copy = len - upload_ctx->bytes_read;
        if (to_copy > room) to_copy = room;
        memcpy(ptr, data + upload_ctx->bytes_read, to_copy);
        upload_ctx->bytes_read += to_copy;
        return to_copy;
    }
    return 0;
}

// =======================================================================
// PHẦN 3: LOGIC GỬI EMAIL (MULTIPART: TEXT + PDF)
// =======================================================================

int send_email() {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx = {0};

    // -----------------------------------------------------------
    // 1. CHUẨN BỊ DỮ LIỆU
    // -----------------------------------------------------------
    char seats_str[1024] = "N/A";
    if (temp_seats && tem_seats_size > 0) {
        char *joined = join_strings(temp_seats, tem_seats_size, ", ");
        if (joined) {
            strncpy(seats_str, joined, sizeof(seats_str) - 1);
            free(joined);
        }
    }
    char *price_str = format_number_with_separator(final_price, ',');
    char *dep_name = extract_middle_string(detail_flight.departure_airport);
    char *arr_name = extract_middle_string(detail_flight.arrival_airport);

    // -----------------------------------------------------------
    // 2. TẠO & ĐỌC FILE PDF
    // -----------------------------------------------------------
    generate_ticket_pdf(PDF_FILENAME);
    
    FILE *pdf_file = fopen(PDF_FILENAME, "rb");
    if (!pdf_file) return 1;
    fseek(pdf_file, 0, SEEK_END);
    long pdf_size = ftell(pdf_file);
    fseek(pdf_file, 0, SEEK_SET);
    unsigned char *pdf_data = malloc(pdf_size);
    fread(pdf_data, 1, pdf_size, pdf_file);
    fclose(pdf_file);

    size_t b64_len;
    char *pdf_base64 = base64_encode(pdf_data, pdf_size, &b64_len);
    free(pdf_data);

    // -----------------------------------------------------------
    // 3. SOẠN NỘI DUNG EMAIL (MULTIPART)
    // -----------------------------------------------------------
    char *boundary = "------------------------Boundary123456789";
    
    // a. Headers Phần đầu
    char headers_part[2048];
    snprintf(headers_part, sizeof(headers_part),
        "To: %s\r\n"
        "From: Flight Booking <%s>\r\n"
        "Subject: Booking Confirmed: Flight %s (E-Ticket Attached)\r\n"
        "MIME-Version: 1.0\r\n"
        "Content-Type: multipart/mixed; boundary=\"%s\"\r\n"
        "\r\n"
        "--%s\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "\r\n",
        email_user, SENDER_EMAIL, detail_flight.flight_id, boundary, boundary
    );

    // b. Body Phần Text (Đã xóa đoạn "Non-stop" visual art)
    char body_text[4096];
    snprintf(body_text, sizeof(body_text),
        "Dear %s,\r\n"
        "\r\n"
        "Thank you for choosing VietnamTravel. Your booking is confirmed.\r\n"
        "Please find your official E-Ticket (PDF) attached to this email for check-in.\r\n"
        "\r\n"
        "Here is your itinerary summary:\r\n"
        "\r\n"
        "================================================\r\n"
        "              FLIGHT CONFIRMATION               \r\n"
        "================================================\r\n"
        " Booking Reference: #%d\r\n"
        " Flight Number:     %s\r\n"
        "================================================\r\n"
        "\r\n"
        " DEPARTURE\r\n"
        " [ %s ]\r\n" 
        " Time: %s\r\n"
        "\r\n"
        " ARRIVAL\r\n"
        " [ %s ]\r\n" 
        "\r\n"
        "================================================\r\n"
        " PASSENGER & SEAT DETAILS\r\n"
        "================================================\r\n"
        " Seat(s):         %s\r\n"
        " Total Payment:   %s VND\r\n"
        " Status:          Confirmed / Paid\r\n"
        "================================================\r\n"
        "\r\n"
        "Safe Travels,\r\n"
        "VietnamTravel Team\r\n"
        "\r\n"
        "--%s\r\n", // Kết thúc phần Text, bắt đầu phần đính kèm
        
        // Variables
        current_user_full_name,
        booking_id,
        detail_flight.flight_id,
        dep_name ? dep_name : detail_flight.departure_airport,
        detail_flight.departure_time,
        arr_name ? arr_name : detail_flight.arrival_airport,
        seats_str,
        price_str ? price_str : "0",
        boundary
    );

    // c. Headers Phần Đính Kèm (PDF)
    char attachment_header[1024];
    snprintf(attachment_header, sizeof(attachment_header),
        "Content-Type: application/pdf; name=\"%s\"\r\n"
        "Content-Disposition: attachment; filename=\"%s\"\r\n"
        "Content-Transfer-Encoding: base64\r\n"
        "\r\n",
        PDF_FILENAME, PDF_FILENAME
    );

    // d. Kết thúc Email
    char end_boundary[100];
    snprintf(end_boundary, sizeof(end_boundary), "\r\n--%s--\r\n", boundary);

    // Tổng hợp Payload
    size_t payload_size = strlen(headers_part) + strlen(body_text) + strlen(attachment_header) + b64_len + strlen(end_boundary) + 100;
    char *payload_full = malloc(payload_size);
    
    strcpy(payload_full, headers_part);
    strcat(payload_full, body_text);
    strcat(payload_full, attachment_header);
    strcat(payload_full, pdf_base64);
    strcat(payload_full, end_boundary);

    // Dọn dẹp biến tạm
    if (price_str) free(price_str);
    if (dep_name) free(dep_name);
    if (arr_name) free(arr_name);
    free(pdf_base64);

    upload_ctx.payload = payload_full;
    upload_ctx.bytes_read = 0;

    // -----------------------------------------------------------
    // 4. GỬI CURL
    // -----------------------------------------------------------
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, SENDER_EMAIL);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, SENDER_PASSWORD);
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, SENDER_EMAIL);
        
        recipients = curl_slist_append(recipients, email_user);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Email sending failed: %s\n", curl_easy_strerror(res));
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    free(payload_full);
    return (res == CURLE_OK) ? 0 : 1;
}

// Xử lý sự kiện click nút Email (Test)
static void on_email_button_clicked(GtkWidget *widget, gpointer user_data) {
    if (send_email() == 0) {
        g_print("Email (Text + PDF) sent successfully!\n");
    } else {
        g_print("Failed to send email.\n");
    }
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    cairo_paint(cr);
    return FALSE;
}

GtkWidget *create_email_screen() {
    GtkWidget *overlay = gtk_overlay_new();
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), drawing_area);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw_event), NULL);

    GtkWidget *button = gtk_button_new_with_label("Send Email");
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), button);
    gtk_widget_set_size_request(button, 150, 50);
    gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(button, GTK_ALIGN_CENTER);

    g_signal_connect(button, "clicked", G_CALLBACK(on_email_button_clicked), NULL);

    return overlay;
}