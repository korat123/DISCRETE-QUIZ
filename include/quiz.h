#ifndef QUIZ_H
#define QUIZ_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// ---------------------- constant / struct ----------------------

#define MAX_Q      50
#define MAX_TEXT   512
#define MAX_NAME   50
#define MAX_LEADER 200

// time's delay (milisec)
#define DELAY_SHORT  800
#define DELAY_MED   1500
#define DELAY_LONG  2200

typedef struct {
    int id;
    int type;                       
    char question[MAX_TEXT];

    // Array Choice (for Shuffle function)
    char choices[5][200]; 
    
    // Stor correct answer Index (0=A, 1=B, ..., 4=E)
    int correctIndex;             
    
    char explanation[MAX_TEXT];

    // Hint and Tags
    char hint1[MAX_TEXT];
    char hint2[MAX_TEXT];
    char tags[100];       
} Question;

typedef struct {
    int qIndex;
    int isCorrect;
    char userAnswerChar;     // stor answer A-E
    int userAnswerIndex;     // store 0-4 (๋Just in case)
    
    int hintsUsed;           // store hint used times(0, 1, 2)
    int shuffledOrder[5];    // for Shuffle function
} Attempt;

typedef struct {
    char name[MAX_NAME];
    char topic[32];
    int score;
} ScoreEntry;

// Struct for mini lessons
typedef struct {
    char tag[50];          // Name: Tag Ex. Logic_Basics
    char content[512];     // lessons informations
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

// Add prototype loading lessons
int loadLessons(const char *filename, Lesson lessons[], int maxLessons);

// Analyze user weak function
void printSkillAnalysis(Question q[], Attempt attempts[], int numAsked, Lesson lessons[], int lessonCount);

// ---------------------- prototypes: UI ---------------------------
char showMenu(void);

#endif // QUIZ_H