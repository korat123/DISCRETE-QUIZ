#include "quiz.h"

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

                showPreviousAnswer(cur, prev);
                wait_ms(DELAY_MED);

                printf("\nCorrect answer is: %c\n", cur->correctChoice);
                wait_ms(DELAY_MED);
                printf("Explanation: %s\n", cur->explanation);
                wait_ms(DELAY_LONG);

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

            waitForNext("\nIf you got it, Press (y) then (Enter) to go next: ");
        }
    }
    clearScreen();
    printf("Finished, Returning to main menu.....");
    wait_ms(DELAY_LONG);
}
