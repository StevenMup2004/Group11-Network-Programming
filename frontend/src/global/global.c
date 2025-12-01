#include "global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <curl/curl.h>
#include <signal.h>
#include <microhttpd.h>
#include "../booklist/booklist.h"
#include "../email/email.h"

#define PORT 8888
#define RUNTIME_SECONDS 900 

char buffer[MAX_LENGTH];
int sock;
GtkWidget *window;
GtkWidget *content_area;
const int airport_count;
Flight flights[MAX_LENGTH];
int flight_count;
Flight tem_flights[MAX_LENGTH];
int tem_flight_count;
Flight detail_flight;
Ticket list_tickets[MAX_LENGTH];
Announce list_announces[MAX_LENGTH];
Announce filtered_announces[MAX_LENGTH];
int filtered_announce_count; 
int announce_count;
int ticket_count;
int seat_count;
char **seats_array;
int user_id;
int price = 0;
int number_seat_order;
int final_price;
char class[30];
char **temp_seats = NULL;
int tem_seats_size = 0;
char email_user[50];
char date_tem_flight[50];
char airport_from[50];
char airport_to[50];
int booking_id;
const char *airports[] = {
       "SGN - Tân Sơn Nhất - Hồ Chí Minh",
        "HAN - Nội Bài - Hà Nội",
        "CXR - Cam Ranh - Khánh Hòa",
        "DAD - Đà Nẵng - Đà Nẵng",
        "HUI - Phú Bài - Thừa Thiên Huế",
        "UIH - Phù Cát - Bình Định",
        "VCL - Chu Lai - Quảng Nam",
        "VCS - Côn Đảo - Bà Rịa-Vũng Tàu",
        "THD - Trường Sa - Khánh Hòa",
        "DLI - Liên Khương - Lâm Đồng",
        "VKG - Quy Nhơn - Bình Định",
        "PQC - Phú Quốc - Kiên Giang",
        "VCA - Cần Thơ - Cần Thơ",
        "CAH - Cát Bi - Hải Phòng",
        "CJJ - Chu Lai - Quảng Nam"
    };

const int airport_count = sizeof(airports) / sizeof(airports[0]);

// Biến cờ kiểm soát thanh toán
volatile sig_atomic_t stop_server = 0;
bool is_payment_success = false;

// --- CÁC HÀM GIAO DIỆN CHÍNH ---

GtkWidget* create_main_window() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "TicketTrail");
    
    // Đặt kích thước cố định để tránh lỗi tràn màn hình
    gtk_window_set_default_size(GTK_WINDOW(window), 1280, 800);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkCssProvider *provider = gtk_css_provider_new();
    if (!gtk_css_provider_load_from_data(provider,
        "window { background-image: linear-gradient(to bottom, #f0f8ff, #e6e9f0); color: #2c3e50; font-family: 'Segoe UI', sans-serif; }\n"
        "#register-box, #login-box { background-color: #ffffff; border-radius: 16px; box-shadow: 0 10px 30px rgba(0,0,0,0.1); padding: 40px; }\n"
        "#register-title, #login-title { font-size: 26px; font-weight: 800; color: #223A60; margin-bottom: 20px; }\n"
        "entry { background-color: #f7f9fc; border: 1px solid #dfe3e8; border-radius: 8px; padding: 10px; color: #333333; }\n"
        "entry:focus { border-color: #223A60; box-shadow: 0 0 0 2px rgba(34, 58, 96, 0.1); }\n"
        "button { background-image: linear-gradient(to right, #223A60, #2c5364); color: white; border-radius: 8px; font-weight: bold; border: none; box-shadow: 0 4px 6px rgba(0,0,0,0.15); }\n"
        "button:hover { background-image: linear-gradient(to right, #2c5364, #223A60); box-shadow: 0 6px 12px rgba(0,0,0,0.2); }\n",
        -1, NULL)) {
        g_print("Failed to load CSS\n");
    }

    GtkStyleContext *context = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    content_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(main_box), content_area, TRUE, TRUE, 0);

    return window;
}

void set_content(GtkWidget *new_content) {
    gtk_container_forall(GTK_CONTAINER(content_area), (GtkCallback)gtk_widget_destroy, NULL);
    gtk_box_pack_start(GTK_BOX(content_area), new_content, TRUE, TRUE, 0);
    gtk_widget_show_all(window);
}

// --- CÁC HÀM TIỆN ÍCH (HELPER FUNCTIONS) ---

int is_number(const char *str) {
    char *endptr;
    strtol(str, &endptr, 10);
    return *endptr == '\0';
}

char* format_number_with_separator(int number, char separator) {
    char temp[50];
    snprintf(temp, sizeof(temp), "%d", number);
    int length = strlen(temp);
    int is_negative = (temp[0] == '-');
    int groups = (length - is_negative - 1) / 3;
    char* result = (char*)malloc(length + groups + 1);
    if (!result) return NULL;

    int src = length - 1;
    int dest = length + groups;
    result[dest--] = '\0';
    int count = 0;
    while (src >= is_negative) {
        result[dest--] = temp[src--];
        count++;
        if (count == 3 && src >= is_negative) {
            result[dest--] = separator;
            count = 0;
        }
    }
    if (is_negative) result[dest] = '-';
    return result;
}

char* extract_middle_string(const char *input) {
    char temp[256];
    strcpy(temp, input);
    strtok(temp, " -");
    char *middle_part = strtok(NULL, "-");
    if (middle_part != NULL) {
        while (*middle_part == ' ') middle_part++;
        char *end = middle_part + strlen(middle_part) - 1;
        while (end > middle_part && *end == ' ') end--;
        *(end + 1) = '\0';
        return strdup(middle_part);
    }
    return NULL;
}

void split_date_time(const char *input, char *date, char *time) {
    const char *space_ptr = strchr(input, ' ');
    if (space_ptr == NULL || strlen(input) < 19) {
        strcpy(date, "Invalid");
        strcpy(time, "Format");
        return;
    }
    size_t date_length = space_ptr - input;
    strncpy(date, input, date_length);
    date[date_length] = '\0';
    strcpy(time, space_ptr + 1);
}

bool checkHaveSeat(int available_economy, int available_business, int available_first, const char *type, int number_seat){
    if (strcmp(type, "Economy") == 0) return available_economy >= number_seat;
    else if (strcmp(type, "Business") == 0) return available_business >= number_seat;
    else return available_first >= number_seat;
}

void filter_flights(const Flight *flights, int flight_count, Flight *temp_flight, int *temp_count_flight, const char *departure, const char *arrival, const char *date, const char *class, const int number_seat) {
    *temp_count_flight = 0;
    char date_split[20];
    char time_split[20];
    for (int i = 0; i < flight_count; i++) {
        split_date_time(flights[i].departure_time, date_split, time_split);
        if (strcmp(extract_middle_string(flights[i].departure_airport), extract_middle_string(departure)) == 0 &&
            strcmp(extract_middle_string(flights[i].arrival_airport), extract_middle_string(arrival)) == 0 &&
            strcmp(date_split, date) == 0 && checkHaveSeat(flights[i].available_economy, flights[i].available_business, flights[i].available_first_class, class, number_seat)) {
            temp_flight[*temp_count_flight] = flights[i];
            (*temp_count_flight)++;
        }
    }
}

int get_seat_position(const char *seat_code, int *i, int *j) {
    const char *columns = "ABCDEFGHIJ";
    int row;
    char column;
    if (sscanf(seat_code, "%d%c", &row, &column) != 2) return -1;
    *i = row - 1;
    const char *column_ptr = strchr(columns, column);
    if (column_ptr == NULL) return -1;
    *j = column_ptr - columns;
    return 0;
}

char **add_string_to_array(char **array, int *size, const char *new_string) {
    for (int i = 0; i < *size; i++) {
        if (strcmp(array[i], new_string) == 0) return array;
    }
    char **new_array = realloc(array, (*size + 1) * sizeof(char *));
    if (!new_array) return array;
    new_array[*size] = strdup(new_string);
    (*size)++;
    return new_array;
}

char **remove_string_from_array(char **array, int *size, const char *target) {
    int index = -1;
    for (int i = 0; i < *size; i++) {
        if (strcmp(array[i], target) == 0) { index = i; break; }
    }
    if (index == -1) return array;
    free(array[index]);
    for (int i = index; i < *size - 1; i++) array[i] = array[i + 1];
    char **new_array = realloc(array, (*size - 1) * sizeof(char *));
    (*size)--;
    return new_array;
}

void print_array(char **array, int size) {
    printf("Array:\n");
    for (int i = 0; i < size; i++) printf("  [%d]: %s\n", i, array[i]);
}

char *join_strings(char **array, int size, const char *delimiter) {
    if (size == 0) return strdup("");
    int total_length = 0;
    int delimiter_length = strlen(delimiter);
    for (int i = 0; i < size; i++) {
        total_length += strlen(array[i]);
        if (i < size - 1) total_length += delimiter_length;
    }
    char *result = malloc(total_length + 1);
    result[0] = '\0';
    for (int i = 0; i < size; i++) {
        strcat(result, array[i]);
        if (i < size - 1) strcat(result, delimiter);
    }
    return result;
}

int time_to_seconds(const char *time) {
    int hh, mm, ss;
    sscanf(time, "%d:%d:%d", &hh, &mm, &ss);
    return hh * 3600 + mm * 60 + ss;
}

void seconds_to_time(int total_seconds, char *time_str) {
    int hh = (total_seconds / 3600) % 24;
    int mm = (total_seconds % 3600) / 60;
    int ss = total_seconds % 60;
    sprintf(time_str, "%02d:%02d:%02d", hh, mm, ss);
}

char *calculate_end_time(const char *start_time, int duration) {
    int start_seconds = time_to_seconds(start_time);
    int end_seconds = start_seconds + duration;
    char *end_time = malloc(9);
    seconds_to_time(end_seconds, end_time);
    return end_time;
}

int is_valid_date(const char *date_str) {
    struct tm tm;
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    if (strptime(date_str, "%Y-%m-%d", &tm) == NULL) return 0;
    if (tm.tm_year < current_time->tm_year) return 0;
    if (tm.tm_year == current_time->tm_year) {
        if (tm.tm_mon < current_time->tm_mon) return 0;
        if (tm.tm_mon == current_time->tm_mon) {
            if (tm.tm_mday < current_time->tm_mday) return 0;
        }
    }
    return 1;
}

int get_list_tickets_ordered () {
    snprintf(buffer, MAX_LENGTH, "GET LIST TICKETS: %d", user_id);
    send(sock, buffer, strlen(buffer), 0);
    if (recv(sock, &ticket_count, sizeof(ticket_count), 0) <= 0) return -1;
    int bytes_received = recv(sock, buffer, MAX_LENGTH - 1, 0);
    if (bytes_received <= 0) return -1;
    buffer[bytes_received] = '\0';
    if (strcmp(buffer, "NO TICKETS") == 0) return 0;
    parse_buffer_to_tickets(buffer, list_tickets);
    return 1;
}

int filter_announces_by_tickets(Announce *announces, int announce_count, Ticket *tickets, int ticket_count, Announce *filtered_announces) {
    int filtered_count = 0;
    for (int i = 0; i < announce_count; i++) {
        for (int j = 0; j < ticket_count; j++) {
            if (strcmp(announces[i].flight_id, tickets[j].flight_id) == 0) {
                filtered_announces[filtered_count++] = announces[i];
                break;
            }
        }
    }
    return filtered_count;
}

void generate_next_5_days(const char *input_date, char **days, int *num_days) {
    struct tm date = {0};
    if (strptime(input_date, "%Y-%m-%d", &date) == NULL) return;
    *num_days = 5;
    date.tm_mday -= 2;
    mktime(&date);
    for (int i = 0; i < *num_days; i++) {
        days[i] = malloc(11);
        strftime(days[i], 11, "%Y-%m-%d", &date);
        date.tm_mday += 1;
        mktime(&date);
    }
}

char* convert_minutes_to_time_format(int total_minutes) {
    int hours = total_minutes / 60;
    int minutes = total_minutes % 60;
    char *time_str = malloc(10);
    snprintf(time_str, 10, "%dh%dm", hours, minutes);
    return time_str;
}

// --- THANH TOÁN VNPAY & HTTP SERVER ---

void url_encode(const char *src, char *dest, size_t dest_len) {
    size_t i, j = 0;
    for (i = 0; src[i] != '\0' && j < dest_len - 1; i++) {
        if (isalnum((unsigned char)src[i]) || src[i] == '-' || src[i] == '_' || src[i] == '.' || src[i] == '~') {
            dest[j++] = src[i];
        } else {
            if (j + 3 >= dest_len) break;
            snprintf(&dest[j], 4, "%%%02X", (unsigned char)src[i]);
            j += 3;
        }
    }
    dest[j] = '\0';
}

char *hmac_sha512(const char *key, const char *data) {
    unsigned char *result = malloc(64);
    unsigned int len = 64;
    HMAC(EVP_sha512(), key, strlen(key), (unsigned char *)data, strlen(data), result, &len);
    char *hex_result = malloc(129);
    for (unsigned int i = 0; i < len; i++) sprintf(hex_result + (i * 2), "%02x", result[i]);
    hex_result[len * 2] = '\0';
    free(result);
    return hex_result;
}

char* generate_random_number(size_t length) {
    char *output = malloc(length + 1);
    srand((unsigned) time(NULL));
    for (size_t i = 0; i < length; i++) output[i] = '0' + (rand() % 10);
    output[length] = '\0';
    return output;
}

char* convert_to_string_amount(int number_money) {
    char *output = malloc(50);
    snprintf(output, 50, "%d", number_money);
    return output;
}

char* get_current_timeV2() {
    char *output = malloc(15);
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(output, 15, "%04d%02d%02d%02d%02d%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    return output;
}

void open_browser(const char *url) {
    char command[2048];
    #ifdef _WIN32
        snprintf(command, sizeof(command), "start %s", url);
    #elif __APPLE__
        snprintf(command, sizeof(command), "open %s", url);
    #else
        snprintf(command, sizeof(command), "xdg-open \"%s\"", url);
    #endif
    system(command);
}

void vnpay_payment() {
    const char *vnp_TmnCode = "U18LZ7EN";
    const char *vnp_HashSecret = "78RRKJGJDS1OUU8EDIFWMABTMJNFLJWP";
    const char *vnp_Url = "https://sandbox.vnpayment.vn/paymentv2/vpcpay.html";

    char *vnp_TxnRef = generate_random_number(8);
    char *vnp_Amount = convert_to_string_amount(final_price * 100);
    char *vnp_CreateDate = get_current_timeV2();
    const char *vnp_CurrCode = "VND";
    const char *vnp_IpAddr = "127.0.0.1";
    const char *vnp_Locale = "vn";
    const char *vnp_OrderInfo = "Thanhtoan";
    const char *vnp_OrderType = "other";
    const char *vnp_ReturnUrl = "http://localhost:8888/vnpay_return";
    const char *vnp_BankCode = "NCB";

    char encoded_OrderInfo[512];
    char encoded_ReturnUrl[512];
    url_encode(vnp_OrderInfo, encoded_OrderInfo, sizeof(encoded_OrderInfo));
    url_encode(vnp_ReturnUrl, encoded_ReturnUrl, sizeof(encoded_ReturnUrl));

    char params[2048];
    snprintf(params, sizeof(params),
        "vnp_Amount=%s&vnp_BankCode=%s&vnp_Command=pay&vnp_CreateDate=%s&vnp_CurrCode=%s"
        "&vnp_IpAddr=%s&vnp_Locale=%s&vnp_OrderInfo=%s&vnp_OrderType=%s&vnp_ReturnUrl=%s&vnp_TmnCode=%s"
        "&vnp_TxnRef=%s&vnp_Version=2.1.0",
        vnp_Amount, vnp_BankCode, vnp_CreateDate, vnp_CurrCode, vnp_IpAddr, vnp_Locale,
        encoded_OrderInfo, vnp_OrderType, encoded_ReturnUrl, vnp_TmnCode, vnp_TxnRef);

    char *secure_hash = hmac_sha512(vnp_HashSecret, params);
    char payment_url[4096];
    snprintf(payment_url, sizeof(payment_url), "%s?%s&vnp_SecureHash=%s", vnp_Url, params, secure_hash);

    printf("Payment URL: %s\n", payment_url);
    open_browser(payment_url);
    free(secure_hash);
    free(vnp_TxnRef);
    free(vnp_Amount);
    free(vnp_CreateDate);
}

void handle_signal(int signal) {
    stop_server = 1;
}

int iterate_querystring(void *cls, enum MHD_ValueKind kind, const char *key, const char *value) {
    int *condition_met = (int *)cls; 
    if (key && value) {
        if (strcmp(key, "vnp_ResponseCode") == 0 && strcmp(value, "00") == 0) {
            *condition_met = 1;
            // Chỉ đánh dấu thành công, không gọi đặt vé ở đây
            if (!is_payment_success) {
                is_payment_success = true;
                stop_server = 1; // Dừng vòng lặp chờ
            }
        }
    }
    return MHD_YES; 
}

int handle_request(void *cls, struct MHD_Connection *connection, 
                   const char *url, const char *method, const char *version, 
                   const char *upload_data, size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") != 0) return MHD_NO; 

    int condition_met = 0; 
    MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, &iterate_querystring, &condition_met);

    const char *response_text;
    if (condition_met) {
        response_text = "Thanh toan thanh cong! Vui long quay lai ung dung."; 
    } else {
        response_text = "Dang xu ly thanh toan...";
    }

    struct MHD_Response *http_response = MHD_create_response_from_buffer(strlen(response_text), (void *)response_text, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, http_response);
    MHD_destroy_response(http_response);

    return ret;
}

void receive_result_from_vnpay(){
    struct MHD_Daemon *daemon;
    
    // Reset trạng thái
    stop_server = 0;
    is_payment_success = false;

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &handle_request, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start HTTP server.\n");
        return;
    }

    printf("HTTP server running on port %d...\n", PORT);
    time_t start_time = time(NULL); 

    // Vòng lặp chờ tín hiệu (Non-blocking GUI)
    while (!stop_server) {
        if (time(NULL) - start_time >= RUNTIME_SECONDS) break;
        
        // Xử lý sự kiện GTK để app không bị treo
        while (gtk_events_pending()) gtk_main_iteration();
        
        struct timespec ts = {0, 100000000}; // 100ms
        nanosleep(&ts, NULL);
    }

    MHD_stop_daemon(daemon);
    printf("HTTP server stopped.\n");

    // --- LOGIC ĐẶT VÉ: CHỈ CHẠY 1 LẦN TẠI ĐÂY ---
    if (is_payment_success) {
        printf("Processing Payment Success...\n");
        
        // 1. Gửi yêu cầu tạo Booking
        snprintf(buffer, sizeof(buffer), "CREATE NEW BOOKING %d %d %s", final_price, user_id, detail_flight.flight_id);
        send(sock, buffer, strlen(buffer), 0);
        
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; 
            if (strncmp(buffer, "SUCCESS BOOKING_ID:", 19) == 0) {
                if (sscanf(buffer + 19, "%d", &booking_id) == 1) {
                    printf("Booking created ID: %d\n", booking_id);
                    
                    // 2. Gửi yêu cầu tạo chi tiết ghế
                    snprintf(buffer, sizeof(buffer), "CREATE DETAIL BOOKING %d %s", booking_id, join_strings(temp_seats, tem_seats_size, ","));
                    send(sock, buffer, strlen(buffer), 0);
                    recv(sock, buffer, sizeof(buffer) - 1, 0); 
                }
            }
        }

        // 3. Gửi Email
        if (send_email() == 0) printf("Email sent successfully!\n");
        
        // 4. Chuyển màn hình
        if (get_list_tickets_ordered() != -1){
             GtkWidget *book_list_window =  create_booklist_window();
             set_content(book_list_window);
        }
    }
}