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
#define DELAY_SHORT  800
#define DELAY_MED   1500
#define DELAY_LONG  2200

typedef struct {
    int id;
    int type;                       
    char question[MAX_TEXT];

    // Choice แบบ Array (รองรับ Phase ถัดไปเรื่อง Shuffle)
    char choices[5][200]; 
    
    // คำตอบที่ถูกเก็บเป็น Index (0=A, 1=B, ..., 4=E)
    int correctIndex;             
    
    char explanation[MAX_TEXT];

    // Hint และ Tags
    char hint1[MAX_TEXT];
    char hint2[MAX_TEXT];
    char tags[100];       
} Question;

typedef struct {
    int qIndex;
    int isCorrect;
    char userAnswerChar;     // เก็บคำตอบ A-E
    int userAnswerIndex;     // เก็บ 0-4 (เผื่อใช้)
    
    int hintsUsed;           // เก็บจำนวน hint ที่เปิดใช้ไปแล้ว (0, 1, 2)
    int shuffledOrder[5];    // เตรียมไว้สำหรับ Phase Shuffle (ตอนนี้เรียง 0-4 ไปก่อน)
} Attempt;

typedef struct {
    char name[MAX_NAME];
    char topic[32];
    int score;
} ScoreEntry;

// Struct สำหรับเก็บเนื้อหาบทเรียน
typedef struct {
    char tag[50];          // ชื่อ Tag เช่น Logic_Basics
    char content[512];     // เนื้อหาบทเรียน
} Lesson;


// ---------------------- prototypes: util ----------------------
void trimNewline(char *s);
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
void showLeaderboard(const char *topicName, ScoreEntry entries[], int count, const char *currentPlayerName);
void buildLeaderboardFilename(const char *topicName, char *out, size_t outSize);

// ---------------------- prototypes: quiz logic -------------------
int runQuiz(const char *topicName, Question q[], int totalQ, Attempt attempts[], int *outNumAsked);
void showWrongAndRetry(Question q[], Attempt attempts[], int numAsked);

// เพิ่ม prototype โหลดบทเรียน
int loadLessons(const char *filename, Lesson lessons[], int maxLessons);

// ฟังก์ชันวิเคราะห์จุดอ่อน
void printSkillAnalysis(Question q[], Attempt attempts[], int numAsked, Lesson lessons[], int lessonCount);

// ---------------------- prototypes: UI ---------------------------
char showMenu(void);

#endif // QUIZ_H