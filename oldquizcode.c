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

// ---------------------- ฟังก์ชัน util ----------------------

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

// ---------------------- อ่านคำถามจากไฟล์ ----------------------
//
// รูปแบบบรรทัดในไฟล์ .txt (5 ตัวเลือก)
// M|id|question|A|B|C|D|E|correctLetter|explanation
//
int loadQuestions(const char *filename, Question q[], int maxQ) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Cannot open question file: %s\n", filename);
        return 0;
    }

    char line[2048];
    int count = 0;

    while (fgets(line, sizeof(line), fp) && count < maxQ) {
        //printf("RAW LINE: [%s]\n", line);
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
        return;
    }
    fprintf(fp, "%s;%s;%d\n", name, topic, score);
    fclose(fp);
}

void showLeaderboard(ScoreEntry entries[], int count) {
    if (count == 0) {
        printf("\nNo leaderboard data yet.\n");
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

    printf("\n===== Leaderboard (All topics) =====\n");
    printf("%-4s %-20s %-12s %-6s\n", "Rank", "Name", "Topic", "Score");

    int maxShow = count;
    if (maxShow > 10) maxShow = 10;

    for (int i = 0; i < maxShow; i++) {
        printf("%-4d %-20s %-12s %-6d\n",
               i + 1,
               entries[i].name,
               entries[i].topic,
               entries[i].score);
    }
    Sleep(1500);
}

// ---------------------- quiz logic ----------------------

int runQuiz(const char *topicName,
            Question q[], int totalQ,
            Attempt attempts[], int *outNumAsked) {

    if (totalQ <= 0) {
        printf("No questions loaded for this topic.\n");
        *outNumAsked = 0;
        Sleep(1500);
        return 0;
    }

    int indices[MAX_Q];
    for (int i = 0; i < totalQ; i++) {
        indices[i] = i;
    }
    shuffle(indices, totalQ);

    int score = 0;
    int numAsked = totalQ;
    Sleep(1500);
    printf("\nStarting quiz for topic: %s\n", topicName);
    Sleep(1500);
    printf("Total questions: %d\n", numAsked);
    Sleep(1500);
    
    for (int i = 0; i < numAsked; i++) {
        int idx = indices[i];
        Question *cur = &q[idx];

        attempts[i].qIndex = idx;
        attempts[i].isCorrect = 0;
        attempts[i].userAnswerChar = '\0';
        attempts[i].userAnswerText[0] = '\0';

        printf("\n------------------------------------\n");
        printf("Question %d (ID %d):\n", i + 1, cur->id);
        printf("%s\n", cur->question);

        printf("A) %s\n", cur->choiceA);
        printf("B) %s\n", cur->choiceB);
        printf("C) %s\n", cur->choiceC);
        printf("D) %s\n", cur->choiceD);
        printf("E) %s\n", cur->choiceE);

        char ans;
        printf("Your answer (A-E): ");
        scanf(" %c", &ans);
        ans = (char)toupper((unsigned char)ans);

        attempts[i].userAnswerChar = ans;

        if (ans == cur->correctChoice) {
            attempts[i].isCorrect = 1;
            score++;   
        }
        Sleep(1500);
    }

    *outNumAsked = numAsked;
    return score;
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
        return;
    }

    printf("\nYou answered some questions incorrectly.\n");
    printf("Do you want to retry only the wrong questions? (y/n): ");

    char choice;
    scanf(" %c", &choice);
    choice = (char)tolower((unsigned char)choice);

    if (choice != 'y') {
        printf("\nShowing correct answers and explanations for wrong questions:\n");
        Sleep(1500);

        for (int i = 0; i < numAsked; i++) {
            if (!attempts[i].isCorrect) {
                Question *cur = &q[attempts[i].qIndex];
                printf("\nQuestion ID %d:\n", cur->id);
                printf("%s\n", cur->question);
                
                Sleep(1500);

                printf("Correct answer is: %c\n", cur->correctChoice);

                Sleep(1500);

                printf("Explanation: %s\n", cur->explanation);
            }
        }
        Sleep(1500);
        return;
    }
    Sleep(1500);
    printf("\nRetrying wrong questions...\n");
    Sleep(1500);
    for (int i = 0; i < numAsked; i++) {
        if (!attempts[i].isCorrect) {
            Question *cur = &q[attempts[i].qIndex];

            printf("\n------------------------------------\n");
            printf("RETRY - Question ID %d:\n", cur->id);
            printf("%s\n", cur->question);

            printf("A) %s\n", cur->choiceA);
            printf("B) %s\n", cur->choiceB);
            printf("C) %s\n", cur->choiceC);
            printf("D) %s\n", cur->choiceD);
            printf("E) %s\n", cur->choiceE);

            char ans;
            printf("Your answer (A-E): ");
            scanf(" %c", &ans);
            ans = (char)toupper((unsigned char)ans);
            Sleep(1500);
            if (ans == cur->correctChoice) {
                Sleep(1500);
                printf("Correct this time! Well done!\n");
                Sleep(1500);
            } else {
                Sleep(1500);
                printf("Still wrong!!!.\n");
                Sleep(1500);
                printf("Correct answer: %c\n", cur->correctChoice);
                Sleep(1500);
                printf("Explanation: %s\n", cur->explanation);
                Sleep(1500);
            }
        }
    }
}

// ---------------------- UI / main ----------------------

char showMenu() {
    char ch;
    printf("\n===== Discrete Mathematics Quiz =====\n");
    printf("1. Logic\n");
    printf("2. BigO\n");
    printf("3. Relations\n");
    printf("4. Counting\n");
    printf("5. Advanced Counting\n");
    printf("0. Exit\n");
    printf("Choose topic: ");
    scanf(" %c", &ch);
    return ch;
    Sleep(1500);
}

int main(void) {
    Question questions[MAX_Q];
    Attempt attempts[MAX_Q];
    ScoreEntry leaderboard[MAX_LEADER];

    char questionFile[64];
    char topicName[32];
    char playerName[MAX_NAME];

    srand((unsigned int)time(NULL));

    while (1) {
        char choice = showMenu();
        if (choice == '0') {
            printf("Goodbye!\n");
            break;
        }
        
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
                continue;
        }
        Sleep(1500);
        printf("Enter your name: ");
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { /* flush */ }
        if (fgets(playerName, sizeof(playerName), stdin) == NULL) {
            strcpy(playerName, "Anonymous");
        }
        trimNewline(playerName);

        Sleep(1500);
        
        int totalQ = loadQuestions(questionFile, questions, MAX_Q);
        if (totalQ <= 0) {
            printf("No questions found for this topic.\n");
            Sleep(1500);
            continue;
        }

        int numAsked = 0;
        int score = runQuiz(topicName, questions, totalQ, attempts, &numAsked);

        printf("\n===== Result for %s (%s) =====\n", playerName, topicName);
        printf("Score: %d / %d\n", score, numAsked);
        Sleep(1500);

        saveScore("leaderboard.txt", playerName, topicName, score);

        int leaderCount = loadLeaderboard("leaderboard.txt", leaderboard, MAX_LEADER);
        
        showLeaderboard(leaderboard, leaderCount);
        
        showWrongAndRetry(questions, attempts, numAsked);
    }

    return 0;
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd) {
    return main();
}
#endif
