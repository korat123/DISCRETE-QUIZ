#include "quiz.h"

#ifdef _WIN32
#include <windows.h>
#endif

// cut \n / \r in end of string
void trimNewline(char *s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}

// Doesn't use just for text test
void normalizeAnswer(char *dst, const char *src) {
    int j = 0;
    for (int i = 0; src[i] != '\0'; i++) {
        if (!isspace((unsigned char)src[i])) {
            dst[j++] = (char)tolower((unsigned char)src[i]);
        }
    }
    dst[j] = '\0';
}

// shuffle array of index
void shuffle(int arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

// Time's delay(ms)
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

// clear screen
void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Delay and clear
void pauseAndClear(int ms) {
    wait_ms(ms);
    clearScreen();
}

// wait until user press y or Y and then Enter
void waitForNext(const char *prompt) {
    char buf[16];
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            return; // if can't read then will exit
        }
        if (buf[0] == 'y' || buf[0] == 'Y') {
            break;
        }
        // if users type others words then re-ask again 
    }
}
