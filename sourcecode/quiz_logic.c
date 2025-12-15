#include "quiz.h"

// lessons.txt loading function
int loadLessons(const char *filename, Lesson lessons[], int maxLessons) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;

    char line[1024];
    int count = 0;
    while (fgets(line, sizeof(line), fp) && count < maxLessons) {
        trimNewline(line);
        if (line[0] == '\0') continue;
        
        char *token = strtok(line, "|");
        if (!token) continue;
        strcpy(lessons[count].tag, token);
        
        token = strtok(NULL, "|");
        if (token) strcpy(lessons[count].content, token);
        else strcpy(lessons[count].content, "No content available.");
        
        count++;
    }
    fclose(fp);
    return count;
}

// Struct for compute user's Skill (use only In this file)
typedef struct {
    char tagName[100];
    int total;
    int correct;
} TagStat;

// Show Hint function
void printHints(Question *q, int hintsRevealed) {
    if (hintsRevealed >= 1) printf("\n   [HINT 1]: %s", q->hint1);
    if (hintsRevealed >= 2) printf("\n   [HINT 2]: %s", q->hint2);
    if (hintsRevealed > 0) printf("\n");
}

//  User Analysis and show lessons(In loop for many sub-topics) function
void printSkillAnalysis(Question q[], Attempt attempts[], int numAsked, Lesson lessons[], int lessonCount) {
    TagStat stats[20];
    int statCount = 0;

    // 1. statistic compute
    for (int i = 0; i < numAsked; i++) {
        Question *cur = &q[attempts[i].qIndex];
        int isCorrect = attempts[i].isCorrect;
        
        int foundIndex = -1;
        for (int k = 0; k < statCount; k++) {
            if (strcmp(stats[k].tagName, cur->tags) == 0) {
                foundIndex = k;
                break;
            }
        }

        if (foundIndex == -1) {
            if (statCount < 20) {
                strcpy(stats[statCount].tagName, cur->tags);
                stats[statCount].total = 0;
                stats[statCount].correct = 0;
                foundIndex = statCount;
                statCount++;
            } else foundIndex = 0;
        }

        stats[foundIndex].total++;
        if (isCorrect) stats[foundIndex].correct++;
    }

    // Variables to store user's weak
    int weakIndices[20]; 
    int weakCount = 0;

    // 2. Show results and store sub-topics that user need to learn more
    clearScreen();
    printf("\n========================================\n");
    printf("       AI TUTOR: SKILL ANALYSIS\n");
    printf("========================================\n");
    printf("%-20s %-10s %-10s\n", "Topic/Skill", "Score", "Status");
    printf("----------------------------------------\n");

    for (int i = 0; i < statCount; i++) {
        float percent = 0.0f;
        if (stats[i].total > 0) {
            percent = (float)stats[i].correct / stats[i].total * 100.0f;
        }
        
        char status[50];
        // Scoring Criterias (can adapting 50.0f up to you)
        if (percent == 100.0f) strcpy(status, "Excellent");
        else if (percent >= 70.0f) strcpy(status, "Good");
        else if (percent >= 50.0f) strcpy(status, "Fair");
        else {
            strcpy(status, "WEAK (Review!)"); // if sub-topic's score < 50% = weak 
            weakIndices[weakCount] = i;       // store index to show lessons
            weakCount++;
        }

        printf("%-20s %d/%-2d      %s\n", 
               stats[i].tagName, stats[i].correct, stats[i].total, status);
    }
    printf("========================================\n");

    // 3. AI Tutor suggest study (Loop teach user's weak all sub-topics)
    if (weakCount > 0) {
        printf("\n[AI Suggestion]: I detected weaknesses in %d topics:\n", weakCount);
        for(int k=0; k<weakCount; k++) {
             printf(" - %s\n", stats[weakIndices[k]].tagName);
        }
        
        printf("\nWould you like to start a MINI-LESSON session for ALL these topics? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        int c; while ((c = getchar()) != '\n' && c != EOF) {} // flush

        if (tolower(choice) == 'y') {
            // Loop teach each sub-topic
            for (int k = 0; k < weakCount; k++) {
                int idx = weakIndices[k]; // Index of topic in user's stats
                char *currentTag = stats[idx].tagName;

                clearScreen();
                printf("\n===== Lesson %d/%d: %s =====\n\n", k+1, weakCount, currentTag);
                
                // search lessons
                int lessonFound = 0;
                for(int j=0; j<lessonCount; j++) {
                    if(strcmp(lessons[j].tag, currentTag) == 0) {
                        printf("%s\n", lessons[j].content);
                        lessonFound = 1;
                        break;
                    }
                }
                
                if(!lessonFound) {
                    printf("Note: No specific lesson content found for '%s'.\n", currentTag);
                    printf("Please review your lecture notes on this topic.\n");
                }
                
                printf("\n========================================\n");
                
                if (k < weakCount - 1) {
                    waitForNext("\nPress (y) for the NEXT lesson: ");
                } else {
                    waitForNext("\nThis was the last lesson. Press (y) to finish: ");
                }
            }
        }
    } else {
        printf("\n[AI Suggestion]: Great job! Your skills are solid across all tested topics.\n");
        wait_ms(DELAY_LONG);
        waitForNext("Press (y) to continue: ");
    }
}

// Starting do quiz function
int runQuiz(const char *topicName, Question q[], int totalQ, Attempt attempts[], int *outNumAsked) {
    if (totalQ <= 0) {
        printf("No questions loaded.\n");
        wait_ms(DELAY_MED);
        *outNumAsked = 0;
        return 0;
    }

    int indices[MAX_Q];
    for (int i = 0; i < totalQ; i++) indices[i] = i;
    shuffle(indices, totalQ);

    int score = 0;
    int numAsked = totalQ; 

    clearScreen();
    printf("\nStarting quiz: %s\n", topicName);
    wait_ms(DELAY_MED);

    for (int i = 0; i < numAsked; i++) {
        int idx = indices[i];
        Question *cur = &q[idx];

        attempts[i].qIndex = idx;
        attempts[i].isCorrect = 0;
        attempts[i].hintsUsed = 0;

        // --- [SHUFFLE LOGIC] create shuffle order ---
        // create array [0,1,2,3,4] and do shuffle
        for (int k = 0; k < 5; k++) attempts[i].shuffledOrder[k] = k;
        shuffle(attempts[i].shuffledOrder, 5);
        // ----------------------------------------------

        while (1) {
            clearScreen();
            printf("\n------------------------------------\n");
            printf("Question %d (ID %d) [%s]:\n", i + 1, cur->id, cur->tags); // show tags to debug problems(for programmers not user)
            printf("%s\n\n", cur->question);

            // show Choice from shuffle order (Mapping)
            // A will pull content from choices[shuffledOrder[0]]
            for (int k = 0; k < 5; k++) {
                int realIdx = attempts[i].shuffledOrder[k]; // Real index in struct
                printf("%c) %s\n", 'A' + k, cur->choices[realIdx]);
            }

            printHints(cur, attempts[i].hintsUsed);

            char inputBuf[100];
            printf("\nAnswer (A-E) or 'H' for Hint: ");
            if (!fgets(inputBuf, sizeof(inputBuf), stdin)) continue;
            
            char ans = (char)toupper((unsigned char)inputBuf[0]);

            // if user enter 'H' for hint
            if (ans == 'H') {
                if (attempts[i].hintsUsed < 2) {
                    attempts[i].hintsUsed++;
                    continue; 
                } else {
                    printf("\n>> You have used all hints for this question! <<\n");
                    wait_ms(DELAY_SHORT);
                    continue;
                }
            }

            // answer parts
            if (ans >= 'A' && ans <= 'E') {
                int selectedChoiceIndex = ans - 'A'; // User answer:  A(0), B(1)...
                
                // Convert back to real index of data
                int actualDataIndex = attempts[i].shuffledOrder[selectedChoiceIndex];

                attempts[i].userAnswerChar = ans;
                attempts[i].userAnswerIndex = selectedChoiceIndex; // stored choice position that user choose
                
                // compare user's answer with problem's correct answer(correctIndex = Real index from Database)
                if (actualDataIndex == cur->correctIndex) {
                    attempts[i].isCorrect = 1;
                    score++;
                }
                wait_ms(DELAY_SHORT);
                break; 
            }

            printf("\n>> Invalid input! Please enter A-E or H. <<\n");
            wait_ms(DELAY_SHORT);
        }
    }

    // Loading lessons
    Lesson lessons[50];
    int lessonCount = loadLessons("data/lessons.txt", lessons, 50);

    *outNumAsked = numAsked;
    pauseAndClear(DELAY_MED);
    
    // sending out skill result and ask for lessons
    printSkillAnalysis(q, attempts, numAsked, lessons, lessonCount);

    return score;
}

void showWrongAndRetry(Question q[], Attempt attempts[], int numAsked) {
    int hasWrong = 0;
    for (int i = 0; i < numAsked; i++) if (!attempts[i].isCorrect) hasWrong = 1;

    if (!hasWrong) {
        printf("\nPerfect Score! No retry needed.\n");
        wait_ms(DELAY_LONG);
        return;
    }

    printf("\nYou have incorrect answers.\n");
    printf("Do you want to retry WRONG questions? (y/n): ");
    char choice;
    scanf(" %c", &choice);
    int c; while ((c = getchar()) != '\n' && c != EOF) {}

    if (tolower(choice) != 'y') {
        // --- give the answer (Same Logic, but need to mapping correct choice ) ---
        clearScreen();
        printf("\nShowing correct answers...\n");
        wait_ms(DELAY_MED);

        for (int i = 0; i < numAsked; i++) {
            if (!attempts[i].isCorrect) {
                Question *cur = &q[attempts[i].qIndex];
                
                clearScreen();
                printf("Question ID %d:\n%s\n\n", cur->id, cur->question);
                
                // Show correct answer choice
                // show in shuffle ordering form
                for (int k=0; k<5; k++) {
                    int realIdx = attempts[i].shuffledOrder[k];
                    printf("%c) %s\n", 'A'+k, cur->choices[realIdx]);
                }

                printf("\nYour answer: %c\n", attempts[i].userAnswerChar);
                
                // find where is the correct answer of problem (cur->correctIndex) (A-E)
                char correctChar = '?';
                for(int k=0; k<5; k++) {
                    if (attempts[i].shuffledOrder[k] == cur->correctIndex) {
                        correctChar = 'A' + k;
                        break;
                    }
                }

                printf("Correct answer: %c\n", correctChar);
                printf("Explanation: %s\n", cur->explanation);
                
                waitForNext("\nPress (y) then Enter for next: ");
            }
        }
        return;
    }

    // --- Retry Mode ---
    clearScreen();
    printf("\nRetrying wrong questions...\n");
    wait_ms(DELAY_MED);

    for (int i = 0; i < numAsked; i++) {
        if (!attempts[i].isCorrect) {
            Question *cur = &q[attempts[i].qIndex];
            
            while(1) {
                clearScreen();
                printf("\n--- RETRY ---\n");
                printf("Question ID %d:\n%s\n\n", cur->id, cur->question);
                
                // Use old shuffle odering form
                for (int k=0; k<5; k++) {
                    int realIdx = attempts[i].shuffledOrder[k];
                    printf("%c) %s\n", 'A'+k, cur->choices[realIdx]);
                }
                
                printf("\nPrevious answer: %c\n", attempts[i].userAnswerChar);
                printHints(cur, attempts[i].hintsUsed);

                char inputBuf[100];
                printf("\nRetry Answer (A-E) or 'H' for more Hint: ");
                if (!fgets(inputBuf, sizeof(inputBuf), stdin)) continue;
                char ans = (char)toupper((unsigned char)inputBuf[0]);

                if (ans == 'H') {
                    if (attempts[i].hintsUsed < 2) {
                        attempts[i].hintsUsed++;
                        continue; 
                    } else {
                        printf("\n>> No more hints available! <<\n");
                        wait_ms(DELAY_SHORT);
                        continue;
                    }
                }

                if (ans >= 'A' && ans <= 'E') {
                    int selectedIdx = ans - 'A';
                    int actualDataIdx = attempts[i].shuffledOrder[selectedIdx];

                    if (actualDataIdx == cur->correctIndex) {
                        printf("\nCorrect! Great job.\n");
                        attempts[i].isCorrect = 1; 
                    } else {
                        printf("\nStill Incorrect.\n");
                        
                        // find the correct answer
                        char correctChar = '?';
                        for(int k=0; k<5; k++) {
                            if (attempts[i].shuffledOrder[k] == cur->correctIndex) {
                                correctChar = 'A' + k;
                                break;
                            }
                        }
                        
                        printf("Correct was: %c\n", correctChar);
                        printf("Explanation: %s\n", cur->explanation);
                    }
                    wait_ms(DELAY_LONG);
                    break;
                }
                printf("\n>> Invalid input! <<\n");
                wait_ms(DELAY_SHORT);
            }
            waitForNext("Press (y) to continue: ");
        }
    }
}