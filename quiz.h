#ifndef QUIZ_H
#define QUIZ_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

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


// ---------------------- prototypes: util ----------------------

void trimNewline(char *s);
void normalizeAnswer(char *dst, const char *src);
void shuffle(int arr[], int n);
void wait_ms(int ms);
void clearScreen(void);
void pauseAndClear(int ms);
void waitForNext(const char *prompt);

// ---------------------- prototypes: questions ----------------------

int loadQuestions(const char *filename, Question q[], int maxQ);

// ---------------------- prototypes: leaderboard -------------------

int loadLeaderboard(const char *filename, ScoreEntry entries[], int maxEntries);
void saveScore(const char *filename, const char *name, const char *topic, int score);
void showLeaderboard(const char *topicName,
                     ScoreEntry entries[], int count,
                     const char *currentPlayerName);
void buildLeaderboardFilename(const char *topicName, char *out, size_t outSize);

// ---------------------- prototypes: quiz logic -------------------

int runQuiz(const char *topicName,
            Question q[], int totalQ,
            Attempt attempts[], int *outNumAsked);

void showPreviousAnswer(Question *cur, char prev);
void showWrongAndRetry(Question q[], Attempt attempts[], int numAsked);

// ---------------------- prototypes: UI ---------------------------

char showMenu(void);

#endif // QUIZ_H
