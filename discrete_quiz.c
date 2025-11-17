#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#endif

// ---------------------- ค่าคงที่ / struct ----------------------

#define MAX_Q      50
#define MAX_TEXT   512
#define MAX_NAME   50
#define MAX_LEADER 200

// ระยะเวลา delay (มิลลิวินาที)
#define DELAY_SHORT  800     // ใช้หลังตอบแต่ละข้อ
#define DELAY_MED   1500     // ใช้หลังข้อความสำคัญ
#define DELAY_LONG  2200

typedef struct {
    int id;
    int type;                       // ตอนนี้ใช้ 1 = MCQ
    char question[MAX_TEXT];

    char choiceA[200];
    char choiceB[200];
    char choiceC[200];
    char choiceD[200];
    char choiceE[200];

    char correctChoice;             // A–E
    char correctText[200];          // เผื่อใช้ input-type
    char explanation[MAX_TEXT];
} Question;

typedef struct {
    int qIndex;
    int isCorrect;
    char userAnswerChar;
    char userAnswerText[200];
} Attempt;

typedef struct {
    char name[MAX_NAME];
    char topic[32];
    int score;
} ScoreEntry;

// ---------------------- ฟังก์ชัน util (เวลา + จอ) ----------------------

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

// ---------------------- อ่านคำถามจากไฟล์ ----------------------
//
// รูปแบบบรรทัดในไฟล์ .txt (5 ตัวเลือก)
// M|id|question|A|B|C|D|E|correctLetter|explanation
//
int loadQuestions(const char *filename, Question q[], int maxQ) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Cannot open question file: %s\n", filename);
        wait_ms(DELAY_MED);
        return 0;
    }

    char line[2048];
    int count = 0;

    while (fgets(line, sizeof(line), fp) && count < maxQ) {
        trimNewline(line);
        if (line[0] == '\0' || line[0] == '#')
            continue;

        char *token = strtok(line, "|");
        if (!token) continue;

        Question *cur = &q[count];

        // ใช้แต่แบบ MCQ
        cur->type = 1;

        // id
        token = strtok(NULL, "|");
        if (!token) continue;
        cur->id = atoi(token);

        // question
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(cur->question, token, MAX_TEXT - 1);
        cur->question[MAX_TEXT - 1] = '\0';

        // A
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(cur->choiceA, token, sizeof(cur->choiceA) - 1);
        cur->choiceA[sizeof(cur->choiceA) - 1] = '\0';

        // B
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(cur->choiceB, token, sizeof(cur->choiceB) - 1);
        cur->choiceB[sizeof(cur->choiceB) - 1] = '\0';

        // C
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(cur->choiceC, token, sizeof(cur->choiceC) - 1);
        cur->choiceC[sizeof(cur->choiceC) - 1] = '\0';

        // D
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(cur->choiceD, token, sizeof(cur->choiceD) - 1);
        cur->choiceD[sizeof(cur->choiceD) - 1] = '\0';

        // E
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(cur->choiceE, token, sizeof(cur->choiceE) - 1);
        cur->choiceE[sizeof(cur->choiceE) - 1] = '\0';

        // correct letter
        token = strtok(NULL, "|");
        if (!token) continue;
        cur->correctChoice = (char)toupper((unsigned char)token[0]);

        // explanation
        token = strtok(NULL, "");
        if (token) {
            strncpy(cur->explanation, token, MAX_TEXT - 1);
            cur->explanation[MAX_TEXT - 1] = '\0';
        } else {
            cur->explanation[0] = '\0';
        }

        cur->correctText[0] = '\0';

        count++;
    }

    fclose(fp);
    return count;
}

// ---------------------- leaderboard ----------------------

int loadLeaderboard(const char *filename, ScoreEntry entries[], int maxEntries) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return 0; // ยังไม่มีไฟล์
    }

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), fp) && count < maxEntries) {
        trimNewline(line);
        if (line[0] == '\0') continue;

        char *token = strtok(line, ";");
        if (!token) continue;
        strncpy(entries[count].name, token, MAX_NAME - 1);
        entries[count].name[MAX_NAME - 1] = '\0';

        token = strtok(NULL, ";");
        if (!token) continue;
        strncpy(entries[count].topic, token, sizeof(entries[count].topic) - 1);
        entries[count].topic[sizeof(entries[count].topic) - 1] = '\0';

        token = strtok(NULL, ";");
        if (!token) continue;
        entries[count].score = atoi(token);

        count++;
    }

    fclose(fp);
    return count;
}

void saveScore(const char *filename, const char *name, const char *topic, int score) {
    FILE *fp = fopen(filename, "a");
    if (!fp) {
        printf("Cannot open leaderboard file for writing.\n");
        wait_ms(DELAY_MED);
        return;
    }
    fprintf(fp, "%s;%s;%d\n", name, topic, score);
    fclose(fp);
}

void showLeaderboard(const char *topicName,
                     ScoreEntry entries[], int count,
                     const char *currentPlayerName) {
    if (count == 0) {
        clearScreen();
        printf("\nNo leaderboard data yet for topic '%s'.\n", topicName);
        wait_ms(DELAY_MED);
        return;
    }

    // sort ตามคะแนน มาก→น้อย
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            if (entries[j].score < entries[j + 1].score) {
                ScoreEntry tmp = entries[j];
                entries[j] = entries[j + 1];
                entries[j + 1] = tmp;
            }
        }
    }

    clearScreen();
    printf("\n===== Leaderboard - %s =====\n", topicName);
    printf("%-2s %-4s %-20s %-6s\n", "", "Rank", "Name", "Score");

    int maxShow = count;
    if (maxShow > 10) maxShow = 10;

    for (int i = 0; i < maxShow; i++) {
        // ถ้าชื่อซ้ำกับ user ปัจจุบัน ให้ทำ highlight
        const char *marker = (currentPlayerName != NULL &&
                              strcmp(entries[i].name, currentPlayerName) == 0)
                             ? ">>" : "  ";

        printf("%-2s %-4d %-20s %-6d\n",
               marker,
               i + 1,
               entries[i].name,
               entries[i].score);
    }

    printf("\nnote that: which rows that have '>>' mean its your position inleaderboard\n");
    wait_ms(DELAY_LONG);
}


// สร้างชื่อไฟล์ leaderboard ตามชื่อ topic
// เช่น topicName = "Logic" -> "Logic_leaderboard.txt"
void buildLeaderboardFilename(const char *topicName, char *out, size_t outSize) {
    snprintf(out, outSize, "%s_leaderboard.txt", topicName);
}



// ---------------------- quiz logic ----------------------

int runQuiz(const char *topicName,
            Question q[], int totalQ,
            Attempt attempts[], int *outNumAsked) {

    if (totalQ <= 0) {
        printf("No questions loaded for this topic.\n");
        wait_ms(DELAY_MED);
        *outNumAsked = 0;
        return 0;
    }

    int indices[MAX_Q];
    for (int i = 0; i < totalQ; i++) {
        indices[i] = i;
    }
    shuffle(indices, totalQ);

    int score = 0;
    int numAsked = totalQ;

    clearScreen();
    printf("\nStarting quiz for topic: %s\n", topicName);
    wait_ms(DELAY_MED);
    printf("Total questions: %d\n", numAsked);
    wait_ms(DELAY_LONG);

    for (int i = 0; i < numAsked; i++) {
        int idx = indices[i];
        Question *cur = &q[idx];

        attempts[i].qIndex = idx;
        attempts[i].isCorrect = 0;
        attempts[i].userAnswerChar = '\0';
        attempts[i].userAnswerText[0] = '\0';

        clearScreen();
        printf("\n------------------------------------\n");
        printf("Question %d (ID %d):\n", i + 1, cur->id);
        printf("%s\n\n", cur->question);

        printf("A) %s\n", cur->choiceA);
        printf("B) %s\n", cur->choiceB);
        printf("C) %s\n", cur->choiceC);
        printf("D) %s\n", cur->choiceD);
        printf("E) %s\n", cur->choiceE);

        char ans;
        printf("\nYour answer (A-E): ");
        scanf(" %c", &ans);
        ans = (char)toupper((unsigned char)ans);

        attempts[i].userAnswerChar = ans;

        if (ans == cur->correctChoice) {
            attempts[i].isCorrect = 1;
            score++;
        }
        wait_ms(DELAY_MED);
    }

    *outNumAsked = numAsked;
    pauseAndClear(DELAY_MED);
    return score;
}

// แสดงคำตอบเก่าของ user สำหรับคำถามข้อนั้น
void showPreviousAnswer(Question *cur, char prev) {
    if (prev == '\0') {
        printf("\nYour previous answer: (none)\n");
        return;
    }

    char p = (char)toupper((unsigned char)prev);
    printf("\nYour previous answer: %c) ", p);

    switch (p) {
        case 'A':
            printf("%s\n", cur->choiceA);
            break;
        case 'B':
            printf("%s\n", cur->choiceB);
            break;
        case 'C':
            printf("%s\n", cur->choiceC);
            break;
        case 'D':
            printf("%s\n", cur->choiceD);
            break;
        case 'E':
            printf("%s\n", cur->choiceE);
            break;
        default:
            printf("(not a valid option)\n");
            break;
    }
}


void showWrongAndRetry(Question q[], Attempt attempts[], int numAsked) {
    int hasWrong = 0;
    for (int i = 0; i < numAsked; i++) {
        if (!attempts[i].isCorrect) {
            hasWrong = 1;
            break;
        }
    }

    if (!hasWrong) {
        printf("\nYou answered all questions correctly on the first try. Well done!\n");
        wait_ms(DELAY_LONG);
        return;
    }

    printf("\nYou answered some questions incorrectly.\n");
    wait_ms(DELAY_MED);
    printf("Do you want to retry only the wrong questions? (y/n): ");

    char choice;
    scanf(" %c", &choice);
    choice = (char)tolower((unsigned char)choice);

    // --------- กรณีไม่ retry -> โชว์เฉลยทีละข้อ + คำตอบเก่า ---------
    if (choice != 'y') {
        clearScreen();
        printf("\nShowing correct answers and explanations for wrong questions.\n");
        wait_ms(DELAY_MED);
        
        // flush newline ก่อนใช้ fgets ใน waitForNext
        int chFlush;
        while ((chFlush = getchar()) != '\n' && chFlush != EOF) { }

        for (int i = 0; i < numAsked; i++) {
            if (!attempts[i].isCorrect) {
                Question *cur = &q[attempts[i].qIndex];
                char prev = attempts[i].userAnswerChar;

                clearScreen();
                printf("\nQuestion ID %d:\n", cur->id);
                printf("%s\n\n", cur->question);

                printf("A) %s\n", cur->choiceA);
                printf("B) %s\n", cur->choiceB);
                printf("C) %s\n", cur->choiceC);
                printf("D) %s\n", cur->choiceD);
                printf("E) %s\n", cur->choiceE);
                wait_ms(DELAY_MED);
                // แสดงคำตอบเก่าที่เคยตอบผิด
                showPreviousAnswer(cur, prev);
                wait_ms(DELAY_MED);
                printf("\nCorrect answer is: %c\n", cur->correctChoice);
                wait_ms(DELAY_MED);
                printf("Explanation: %s\n", cur->explanation);
                wait_ms(DELAY_LONG);
                // รอจน user พิมพ์ y ค่อยไปเฉลยข้อถัดไป
                waitForNext("\nIf you got it, Press (y) then (Enter) to go next: ");
            }
        }
        clearScreen();
        printf("Finished, Returning to main menu.....");
        wait_ms(DELAY_LONG);
        return;
    }

    // --------- กรณี retry ข้อผิด -> โชว์คำตอบเก่าก่อนให้ตอบใหม่ ---------
    clearScreen();
    printf("\nRetrying wrong questions...\n");
    wait_ms(DELAY_MED);

    int chFlush;
    while ((chFlush = getchar()) != '\n' && chFlush != EOF) { }

    for (int i = 0; i < numAsked; i++) {
        if (!attempts[i].isCorrect) {
            Question *cur = &q[attempts[i].qIndex];
            char prev = attempts[i].userAnswerChar;

            clearScreen();
            printf("\n------------------------------------\n");
            printf("RETRY - Question ID %d:\n", cur->id);
            printf("%s\n\n", cur->question);

            printf("A) %s\n", cur->choiceA);
            printf("B) %s\n", cur->choiceB);
            printf("C) %s\n", cur->choiceC);
            printf("D) %s\n", cur->choiceD);
            printf("E) %s\n", cur->choiceE);

            // แสดงคำตอบเก่าที่เคยตอบผิด
            showPreviousAnswer(cur, prev);

            char ans;
            printf("\nYour new answer (A-E): ");
            scanf(" %c", &ans);
            ans = (char)toupper((unsigned char)ans);

            if (ans == cur->correctChoice) {
                printf("\nCorrect this time! Well done!\n");
            } else {
                printf("\nStill wrong!!!.\n");
                wait_ms(DELAY_MED);
                printf("Correct answer: %c\n", cur->correctChoice);
                wait_ms(DELAY_MED);
                printf("\nExplanation: %s\n", cur->explanation);
                wait_ms(DELAY_LONG);
            }

            // flush newline ก่อนถาม y
            while ((chFlush = getchar()) != '\n' && chFlush != EOF) { }

            // ให้ user อ่านเฉลย/คำใบ้จนเข้าใจ แล้วค่อยไปข้อผิดถัดไป
            waitForNext("\nIf you got it, Press (y) then (Enter) to go next: ");
        }
    }
    clearScreen();
    printf("Finished, Returning to main menu.....");
    wait_ms(DELAY_LONG);
}






// ---------------------- UI / main ----------------------

char showMenu() {
    char ch;
    
    printf("\n===== Welcome to Discrete Mathematics Quiz =====\n");
    printf("1. Logic\n");
    printf("2. BigO\n");
    printf("3. Relations\n");
    printf("4. Counting\n");
    printf("5. Advanced Counting\n");
    printf("6. View leaderboard only\n");
    printf("0. Exit\n");
    printf("Choose topic that you want to try: ");
    scanf(" %c", &ch);
    return ch;
}


int main(void) {
    Question questions[MAX_Q];
    Attempt attempts[MAX_Q];
    ScoreEntry leaderboard[MAX_LEADER];

    char questionFile[64];
    char topicName[32];
    char playerName[MAX_NAME];

    srand((unsigned int)time(NULL));
    clearScreen();
    printf("\n===== Welcome to Etercsid group work project!!! =====\n");
    wait_ms(DELAY_MED);
    printf("\n===== Our work is creating Discrete Mathematics Quiz =====\n");
    wait_ms(DELAY_MED);
    printf("\n===== We hope you guys will enjoing this project =====\n");
    wait_ms(DELAY_MED);
    printf("\n===== So letss goo!!!! =====\n");
    pauseAndClear(DELAY_MED);

    while (1) {
        clearScreen();
        char choice = showMenu();
        if (choice == '0') {
            printf("Thank you for your attendtion, See ya!!!\n");
            pauseAndClear(DELAY_MED);
            break;
        }

        // ---------- กรณีเลือกดู leaderboard อย่างเดียว ----------
        if (choice == '6') {
            // เลือก topic ที่จะดู leaderboard
            char lbChoice;
            while (1) {
                clearScreen();
                printf("\n=== View Leaderboard ===\n");
                printf("1. Logic\n");
                printf("2. BigO\n");
                printf("3. Relations\n");
                printf("4. Counting\n");
                printf("5. Advanced Counting\n");
                printf("0. Back to main menu\n");
                printf("Choose topic: ");
                scanf(" %c", &lbChoice);

                // flush newline เผื่อมีค้าง
                int chFlush;
                while ((chFlush = getchar()) != '\n' && chFlush != EOF) { }

                if (lbChoice == '0') {
                    // กลับเมนูหลัก
                    break;
                }

                switch (lbChoice) {
                    case '1':
                        strcpy(topicName, "Logic");
                        break;
                    case '2':
                        strcpy(topicName, "BigO");
                        break;
                    case '3':
                        strcpy(topicName, "Relations");
                        break;
                    case '4':
                        strcpy(topicName, "Counting");
                        break;
                    case '5':
                        strcpy(topicName, "AdvCounting");
                        break;
                    default:
                        printf("Invalid choice.\n");
                        wait_ms(DELAY_MED);
                        continue;
                }

                // โหลดและโชว์ leaderboard ของ topic ที่เลือก
                char leaderboardFile[64];
                buildLeaderboardFilename(topicName, leaderboardFile, sizeof(leaderboardFile));

                int leaderCount = loadLeaderboard(leaderboardFile, leaderboard, MAX_LEADER);
                showLeaderboard(topicName, leaderboard, leaderCount, NULL); // NULL = ไม่ไฮไลต์ใครเป็นพิเศษ

                // รอ user ยืนยันก่อนกลับไปเลือกใหม่หรือกลับเมนูหลัก
                waitForNext("Press (y) then (Enter) to go back to View Leaderboard menu: ");
            }

            // กลับไปเมนูหลัก
            continue;
        }

        // ---------- กรณีเลือกหัวข้อเพื่อทำ quiz (1–5) ----------
        switch (choice) {
            case '1':
                strcpy(questionFile, "logic.txt");
                strcpy(topicName, "Logic");
                break;
            case '2':
                strcpy(questionFile, "bigo.txt");
                strcpy(topicName, "BigO");
                break;
            case '3':
                strcpy(questionFile, "relations.txt");
                strcpy(topicName, "Relations");
                break;
            case '4':
                strcpy(questionFile, "counting.txt");
                strcpy(topicName, "Counting");
                break;
            case '5':
                strcpy(questionFile, "advcounting.txt");
                strcpy(topicName, "AdvCounting");
                break;
            default:
                printf("Invalid choice.\n");
                wait_ms(DELAY_MED);
                continue;
        }

        // ---------- ส่วนทำ quiz เหมือนเดิม (แต่ใช้ leaderboard ต่อ topic) ----------
        printf("Enter your name: ");
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { /* flush */ }
        if (fgets(playerName, sizeof(playerName), stdin) == NULL) {
            strcpy(playerName, "Anonymous");
        }
        trimNewline(playerName);

        int totalQ = loadQuestions(questionFile, questions, MAX_Q);
        if (totalQ <= 0) {
            printf("No questions found for this topic.\n");
            wait_ms(DELAY_MED);
            continue;
        }

        int numAsked = 0;
        int score = runQuiz(topicName, questions, totalQ, attempts, &numAsked);

        clearScreen();
        printf("\n===== Result for %s (%s) =====\n", playerName, topicName);
        printf("Score: %d / %d\n", score, numAsked);
        wait_ms(DELAY_MED);

        // ใช้ leaderboard แยกตาม topic
        char leaderboardFile[64];
        buildLeaderboardFilename(topicName, leaderboardFile, sizeof(leaderboardFile));

        saveScore(leaderboardFile, playerName, topicName, score);

        int leaderCount = loadLeaderboard(leaderboardFile, leaderboard, MAX_LEADER);
        showLeaderboard(topicName, leaderboard, leaderCount, playerName);

        showWrongAndRetry(questions, attempts, numAsked);
    }

    return 0;
}