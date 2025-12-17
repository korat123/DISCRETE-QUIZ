// personal helping on this modular
// Korat  68070503410 all file programmer
// Pannatorn  3439 Team lead control work flow  
// Korn 3495 Tester test all prompt and output and report bugs
// Manatsanan 3449 Documentation .txt files do readme.md

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
    if (hintsRevealed >= 1) printf("\n   " C_YELLOW "[HINT 1]: %s" C_RESET, q->hint1);
    if (hintsRevealed >= 2) printf("\n   " C_YELLOW "[HINT 2]: %s" C_RESET, q->hint2);
    if (hintsRevealed > 0) printf("\n");
}

// User Analysis and show lessons function
void printSkillAnalysis(const char *playerName, const char *topicName, int score, Question q[], Attempt attempts[], int numAsked, Lesson lessons[], int lessonCount) {
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


    clearScreen();
    printf(C_BOLD "\n===== Result for %s (%s) =====\n" C_RESET, playerName, topicName);
    printf(C_BOLD "\nScore: %d / %d\n" C_RESET, score, numAsked);
    waitForNext("\nPress (y) to go next: ");

    // Variables to store user's weak
    int weakIndices[20]; 
    int weakCount = 0;

    // 2. Show results
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
        if (percent == 100.0f) sprintf(status, C_GREEN "Excellent" C_RESET);
        else if (percent >= 80.0f) sprintf(status, C_BLUE "Very Good" C_RESET);
        else if (percent >= 70.0f) sprintf(status, C_CYAN "Good" C_RESET);
        else if (percent >= 50.0f) sprintf(status, C_YELLOW "Fair" C_RESET);
        else {
            sprintf(status, C_RED "WEAK (Review!)" C_RESET);
            weakIndices[weakCount] = i;       
            weakCount++;
        }

        printf("%-20s %d/%-2d      %s\n", 
               stats[i].tagName, stats[i].correct, stats[i].total, status);
    }
    printf("========================================\n");

    // 3. AI Tutor suggest study
    if (weakCount > 0) {
        printf("\n[AI Suggestion]: I detected weaknesses in %d topics:\n", weakCount);
        for(int k=0; k<weakCount; k++) {
             printf(" - %s\n", stats[weakIndices[k]].tagName);
        }
        
        //  Input Validation Loop for AI Lesson Choice
        char choice;
        while (1) {
            char inputBuf[32];
            printf("\nWould you like to start a MINI-LESSON session for ALL these topics? (y = yes / n = no ): ");
            
            if (fgets(inputBuf, sizeof(inputBuf), stdin) == NULL) continue;
            trimNewline(inputBuf);
            
            if (strlen(inputBuf) == 1) {
                char c = (char)tolower((unsigned char)inputBuf[0]);
                if (c == 'y' || c == 'n') {
                    choice = c;
                    break; // Valid input
                }
            }
            printf(C_RED "Invalid input! Please enter 'y' for Yes or 'n' for No.\n" C_RESET);
        }

        if (choice == 'y') {
            for (int k = 0; k < weakCount; k++) {
                int idx = weakIndices[k];
                char *currentTag = stats[idx].tagName;

                clearScreen();
                printf("\n===== Lesson %d/%d: %s =====\n\n", k+1, weakCount, currentTag);
                
                int lessonFound = 0;
                for(int j=0; j<lessonCount; j++) {
                    if(strcmp(lessons[j].tag, currentTag) == 0) {
                        printf("%s\n", lessons[j].content);
                        lessonFound = 1;
                        break;
                    }
                }
                
                if(!lessonFound) {
                    printf(C_RED "Note: No specific lesson content found for '%s'.\n" C_RESET, currentTag);
                }
                
                printf("\n========================================\n");
                
                if (k < weakCount - 1) {
                    waitForNext("\nPress (y) for the NEXT lesson: ");
                } else {
                    waitForNext("\nThis was the last lesson. Press (y) + Enter to finish: ");
                }
            }
        }
    } else {
        printf("\n[AI Suggestion]: Great job! Your skills are solid across all tested topics.\n");
        wait_ms(DELAY_LONG);
        waitForNext("Press (y) + Enter to continue: ");
    }
}

// Main Quiz Loop
int runQuiz(const char *topicName, Question q[], int totalQ, Attempt attempts[], int *outNumAsked, const char *playerName) {
    if (totalQ <= 0) {
        printf(C_RED "No questions loaded.\n" C_RESET);
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

        for (int k = 0; k < 5; k++) attempts[i].shuffledOrder[k] = k;
        shuffle(attempts[i].shuffledOrder, 5);

        while (1) {
            clearScreen();
            printf("\n------------------------------------\n");
            
            printf(C_YELLOW "Question %d" C_RESET " [%s]:\n", i + 1, cur->tags);
            printf(C_CYAN C_BOLD "%s" C_RESET "\n\n", cur->question);

            for (int k = 0; k < 5; k++) {
                int realIdx = attempts[i].shuffledOrder[k]; 
                printf(C_GREEN "%c)" C_RESET " %s\n", 'A' + k, cur->choices[realIdx]);
            }

            printHints(cur, attempts[i].hintsUsed);

            char inputBuf[100];
            printf("\nAnswer (A-E) or " C_YELLOW "'H' for Hint" C_RESET ": ");

            if (!fgets(inputBuf, sizeof(inputBuf), stdin)) continue;
            trimNewline(inputBuf);

            if (strlen(inputBuf) != 1) {
                printf("\n" C_RED ">> Invalid input! Please enter a single character (A-E or H). <<" C_RESET "\n");
                wait_ms(DELAY_SHORT);
                continue; 
            }
            
            char ans = (char)toupper((unsigned char)inputBuf[0]);

            if (ans == 'H') {
                if (attempts[i].hintsUsed < 2) {
                    attempts[i].hintsUsed++;
                    continue; 
                } else {
                    printf("\n" C_RED ">> You have used all hints for this question! <<" C_RESET "\n");
                    wait_ms(DELAY_SHORT);
                    continue;
                }
            }

            if (ans >= 'A' && ans <= 'E') {
                int selectedChoiceIndex = ans - 'A'; 
                int actualDataIndex = attempts[i].shuffledOrder[selectedChoiceIndex];

                attempts[i].userAnswerChar = ans;
                attempts[i].userAnswerIndex = selectedChoiceIndex;
                
                if (actualDataIndex == cur->correctIndex) {
                    attempts[i].isCorrect = 1;
                    score++;
                    printf("\n" C_GREEN ">>> CORRECT! Great Job! <<<" C_RESET "\n");
                } else {
                    printf("\n" C_RED ">>> WRONG ANSWER! <<<" C_RESET "\n");
                }
                
                wait_ms(DELAY_SHORT); 
                break; 
            }

            printf("\n" C_RED ">> Invalid input! Please enter A-E or H. <<" C_RESET "\n");
            wait_ms(DELAY_SHORT);
            
        }
    }

    Lesson lessons[50];
    int lessonCount = loadLessons("data/lessons.txt", lessons, 50);

    *outNumAsked = numAsked;
    pauseAndClear(DELAY_MED);
    
    printSkillAnalysis(playerName, topicName, score, q, attempts, numAsked, lessons, lessonCount);

    return score;
}   

// Retry Logic
// Retry Logic (Updated UI)
void showWrongAndRetry(Question q[], Attempt attempts[], int numAsked) {
    int hasWrong = 0;
    for (int i = 0; i < numAsked; i++) if (!attempts[i].isCorrect) hasWrong = 1;

    if (!hasWrong) {
        printf(C_GREEN "\nPerfect Score! No retry needed.\n" C_RESET);
        wait_ms(DELAY_LONG);
        return;
    }

    printf(C_RED "\nYou have incorrect answers.\n" C_RESET);
    
    char choice;
    char inputBuf[32];

    printf("Do you want to retry WRONG questions? (y = yes / n = no, show answer): ");

    while(1) {
        if (fgets(inputBuf, sizeof(inputBuf), stdin) == NULL) return; 
        
        if (strchr(inputBuf, '\n') == NULL) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
        }
        
        trimNewline(inputBuf);

        if (strlen(inputBuf) == 1) {
            char c = (char)tolower((unsigned char)inputBuf[0]);
            if (c == 'y' || c == 'n') {
                choice = c;
                break; 
            }
        }
        
        // Auto-Clear
        printf(C_RED ">> Invalid input! Please enter 'y' or 'n'." C_RESET "\n");
        wait_ms(DELAY_SHORT);
        
        // delete Error Message line
        printf("\033[1A\033[2K"); 
        
        // delete User prompt line
        printf("\033[1A\033[2K");
        
        // Reprint Prompt
        printf("Do you want to retry WRONG questions? (y = yes / n = no, show answer): ");
    }

    if (choice != 'y') {
        clearScreen();
        printf("\nShowing correct answers...\n");
        wait_ms(DELAY_MED);

        for (int i = 0; i < numAsked; i++) {
            if (!attempts[i].isCorrect) {
                Question *cur = &q[attempts[i].qIndex];
                
                clearScreen();
                printf(C_YELLOW "Question %d" C_RESET " [%s]:\n", i + 1, cur->tags);
                printf(C_CYAN C_BOLD "%s\n\n" C_RESET, cur->question);
                
                for (int k=0; k<5; k++) {
                    int realIdx = attempts[i].shuffledOrder[k];
                    printf(C_GREEN "%c)" C_RESET " %s\n", 'A'+k, cur->choices[realIdx]);
                }

                printf(C_RED "\nYour answer: %c\n" C_RESET, attempts[i].userAnswerChar);
                
                char correctChar = '?';
                for(int k=0; k<5; k++) {
                    if (attempts[i].shuffledOrder[k] == cur->correctIndex) {
                        correctChar = 'A' + k;
                        break;
                    }
                }

                printf(C_GREEN "Correct answer: %c\n" C_RESET, correctChar);
                printf(C_CYAN "Explanation: %s\n" C_RESET, cur->explanation);
                
                waitForNext("\nPress (y) then Enter for next: ");
            }
        }
        return;
    }

    
    clearScreen();
    printf("\nRetrying wrong questions...\n");
    wait_ms(DELAY_MED);

    for (int i = 0; i < numAsked; i++) {
        if (!attempts[i].isCorrect) {
            Question *cur = &q[attempts[i].qIndex];
            
            while(1) {
                clearScreen();
                printf(C_BOLD "\n--- RETRY ---\n" C_RESET);
                
                printf(C_YELLOW "Question %d" C_RESET " [%s]:\n", i + 1, cur->tags);
                printf(C_CYAN C_BOLD "%s\n\n" C_RESET, cur->question);
                
                for (int k=0; k<5; k++) {
                    int realIdx = attempts[i].shuffledOrder[k];
                    printf(C_GREEN "%c)" C_RESET " %s\n", 'A'+k, cur->choices[realIdx]);
                }
                
                printf(C_RED "\nPrevious answer: %c\n" C_RESET, attempts[i].userAnswerChar);
                printHints(cur, attempts[i].hintsUsed);

                char inputBuf[100];
                printf("\nRetry Answer (A-E) or 'H' for more Hint: ");
                
                if (!fgets(inputBuf, sizeof(inputBuf), stdin)) continue;
                trimNewline(inputBuf);

                if (strlen(inputBuf) != 1) {
                     printf(C_RED "\n>> Invalid input! Please enter single character. <<\n" C_RESET);
                     wait_ms(DELAY_SHORT);
                     continue;
                }

                char ans = (char)toupper((unsigned char)inputBuf[0]);

                if (ans == 'H') {
                    if (attempts[i].hintsUsed < 2) {
                        attempts[i].hintsUsed++;
                        continue; 
                    } else {
                        printf(C_RED "\n>> No more hints available! <<\n" C_RESET);
                        wait_ms(DELAY_SHORT);
                        continue;
                    }
                }

                if (ans >= 'A' && ans <= 'E') {
                    int selectedIdx = ans - 'A';
                    int actualDataIdx = attempts[i].shuffledOrder[selectedIdx];

                    if (actualDataIdx == cur->correctIndex) {
                        printf(C_GREEN "\nCorrect! Great job.\n" C_RESET);
                        attempts[i].isCorrect = 1; 
                    } else {
                        printf(C_RED "\nStill Incorrect.\n" C_RESET);
                        
                        char correctChar = '?';
                        for(int k=0; k<5; k++) {
                            if (attempts[i].shuffledOrder[k] == cur->correctIndex) {
                                correctChar = 'A' + k;
                                break;
                            }
                        }
                        
                        printf(C_GREEN "\nCorrect was: %c\n" C_RESET, correctChar);
                        printf(C_CYAN "Explanation: %s\n" C_RESET, cur->explanation);
                    }
                    wait_ms(DELAY_LONG);
                    break;
                }
                printf(C_RED "\n>> Invalid input! <<\n" C_RESET);
                wait_ms(DELAY_SHORT);
            }
            waitForNext("Press (y) to continue: ");
        }
    }
}