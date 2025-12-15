#include "quiz.h"

char showMenu(void) {
    char ch;
    
    printf("\n===== Welcome to Discrete Mathematics Quiz =====\n");
    printf("\n===== Topics =====\n");
    printf("1. Logic\n");
    printf("2. BigO\n");
    printf("3. Relations\n");
    printf("4. Counting\n");
    printf("5. Advanced Counting\n");
    printf("\n===== Leaderboard =====\n");
    printf("6. View leaderboard only\n");
    printf("\n0. Exit\n");


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
    printf("\n===== We hope you guys will enjoying this project =====\n");
    wait_ms(DELAY_MED);
    printf("\n===== So letss goo!!!! =====\n");
    pauseAndClear(DELAY_MED);

    while (1) {
        clearScreen();
        char choice = showMenu();
        if (choice == '0') {
            clearScreen();
            printf("\nThank you for your attendtion, See ya!!!\n");
            pauseAndClear(DELAY_MED);
            break;
        }

        // ---------- กรณีเลือกดู leaderboard อย่างเดียว ----------
        if (choice == '6') {
            char lbChoice;
            while (1) {
                clearScreen();
                printf("\n=== View Leaderboard ===\n");
                printf("1. Logic\n");
                printf("2. BigO\n");
                printf("3. Relations\n");
                printf("4. Counting\n");
                printf("5. Advanced Counting\n");
                printf("\n0. Back to main menu\n");
                printf("Choose topic: ");
                scanf(" %c", &lbChoice);

                int chFlush;
                while ((chFlush = getchar()) != '\n' && chFlush != EOF) { }

                if (lbChoice == '0') {
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

                char leaderboardFile[64];
                buildLeaderboardFilename(topicName, leaderboardFile, sizeof(leaderboardFile));

                int leaderCount = loadLeaderboard(leaderboardFile, leaderboard, MAX_LEADER);
                showLeaderboard(topicName, leaderboard, leaderCount, NULL);

                waitForNext("Press (y) then (Enter) to go back to View Leaderboard menu: ");
            }

            continue;
        }

        // ---------- กรณีเลือกหัวข้อเพื่อทำ quiz (1–5) ----------
        switch (choice) {
            case '1':
                strcpy(questionFile, "data/logic.txt");
                strcpy(topicName, "Logic");
                break;
            case '2':
                strcpy(questionFile, "data/bigo.txt");
                strcpy(topicName, "BigO");
                break;
            case '3':
                strcpy(questionFile, "data/relations.txt");
                strcpy(topicName, "Relations");
                break;
            case '4':
                strcpy(questionFile, "data/counting.txt");
                strcpy(topicName, "Counting");
                break;
            case '5':
                strcpy(questionFile, "data/advcounting.txt");
                strcpy(topicName, "AdvCounting");
                break;
            default:
                printf("Invalid choice.\n");
                wait_ms(DELAY_MED);
                continue;
        }

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

        char leaderboardFile[64];
        buildLeaderboardFilename(topicName, leaderboardFile, sizeof(leaderboardFile));

        saveScore(leaderboardFile, playerName, topicName, score);

        int leaderCount = loadLeaderboard(leaderboardFile, leaderboard, MAX_LEADER);
        showLeaderboard(topicName, leaderboard, leaderCount, playerName);

        showWrongAndRetry(questions, attempts, numAsked);
    }

    return 0;
}
