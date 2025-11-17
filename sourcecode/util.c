#include "quiz.h"

#ifdef _WIN32
#include <windows.h>
#endif

// ตัด \n / \r ท้ายสตริง
void trimNewline(char *s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}

// ยังไม่ได้ใช้ แต่เผื่ออยากเช็ค text ในอนาคต
void normalizeAnswer(char *dst, const char *src) {
    int j = 0;
    for (int i = 0; src[i] != '\0'; i++) {
        if (!isspace((unsigned char)src[i])) {
            dst[j++] = (char)tolower((unsigned char)src[i]);
        }
    }
    dst[j] = '\0';
}

// shuffle array ของ index
void shuffle(int arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

// หน่วงเวลา (ms)
void wait_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}

// เคลียร์หน้าจอ
void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// หน่วงแล้วค่อยเคลียร์จอ
void pauseAndClear(int ms) {
    wait_ms(ms);
    clearScreen();
}

// รอจนกว่า user จะกด y หรือ Y แล้วกด Enter
void waitForNext(const char *prompt) {
    char buf[16];
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            return; // ถ้าอ่านไม่ได้ก็ออกเลย
        }
        if (buf[0] == 'y' || buf[0] == 'Y') {
            break;
        }
        // ถ้าพิมพ์อย่างอื่น ก็ถามซ้ำอีกรอบ
    }
}
