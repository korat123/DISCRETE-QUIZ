#include "quiz.h"

char showMenu(void) {
    char ch;

    

    printf("\n" C_BOLD "======= WELCOME TO DISCRETE QUIZ =======" C_RESET "\n");
    printf("\n" C_BOLD "===== SELECT TOPIC =====" C_RESET "\n");
    printf(C_GREEN "[1]" C_RESET " Logic\n");
    printf(C_GREEN "[2]" C_RESET " BigO\n");
    printf(C_GREEN "[3]" C_RESET " Relations\n");
    printf(C_GREEN "[4]" C_RESET " Counting\n");
    printf(C_GREEN "[5]" C_RESET " Advanced Counting\n");
    
    printf("\n" C_BOLD "===== EXTRAS =====" C_RESET "\n");
    printf(C_BLUE  "[6]" C_RESET " View Leaderboard\n");
    printf(C_RED   "\n[0]" C_RESET " Exit Program\n");

    printf("\n" C_CYAN "Choose your destiny: " C_RESET);
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
    pauseAndClear(DELAY_MED);

    // โชว์ ASCII Art ชื่อ DISCRETE QUIZ
    printf(C_CYAN);
    printf(" ____    _____   _____     _____  _____    _____   _______   _____          _____     _      _   _____   ______\n");
    wait_ms(DELAY_SHORT);
    printf("|  _ \\  |_   _| | ____|  /  ___/ |  __ \\  |  ___| |__   __| |  ___|        / ___ \\   | |    | | |_   _| |___   |\n");
    wait_ms(DELAY_SHORT);
    printf("| | | |   | |   | |___  |  /     | |__) ) | |___     | |    | |___        / /   \\ \\  | |    | |   | |      /  /\n");
    wait_ms(DELAY_SHORT);
    printf("| | | |   | |   |____ | |  |     |  _  /  |  ___|    | |    |  ___|      | |     | | | |    | |   | |     /  /\n");
    wait_ms(DELAY_SHORT);
    printf("| |_| |  _| |_   ___| | |  \\___  | | \\ \\  | |___     | |    | |___       /\\ \\___/ /  |  \\__/  |  _| |_   /  /__\n");
    wait_ms(DELAY_SHORT);
    printf("|____/  |_____| |_____|  \\_____/ |_|  \\_\\ |_____|    |_|    |_____|     /_/\\_____/    \\______/  |_____| /______|\n");
    printf(C_RESET);
    pauseAndClear(DELAY_MED);

    // --------- In case: exit----------
    while (1) {
        clearScreen();
        char choice = showMenu();
        if (choice == '0') {
            clearScreen();
            printf(C_BOLD C_BLUE"\nThank you for your attendtion, See ya!!!\n" C_RESET);
            pauseAndClear(DELAY_MED);
            break;
        }

        // ---------- In Case: choose leaderboard only ----------
        if (choice == '6') {
            char lbChoice;
            while (1) {
                clearScreen();
                printf("\n" C_BOLD "=== View Leaderboard ===" C_RESET "\n");
                printf(C_BLUE  "[1]" C_RESET " Logic\n");
                printf(C_BLUE  "[2]" C_RESET " BigO\n");
                printf(C_BLUE  "[3]" C_RESET " Relations\n");
                printf(C_BLUE  "[4]" C_RESET " Counting\n");
                printf(C_BLUE  "[5]" C_RESET " Advanced Counting\n");
                printf("\n" C_RED   "[0]" C_RESET " Back to main menu\n");
                printf("\nChoose topic: ");
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
                        printf(C_RED "Invalid choice.\n" C_BOLD);
                        wait_ms(DELAY_MED);
                        continue;
                }

                char leaderboardFile[64];
                buildLeaderboardFilename(topicName, leaderboardFile, sizeof(leaderboardFile));

                int leaderCount = loadLeaderboard(leaderboardFile, leaderboard, MAX_LEADER);
                showLeaderboard(topicName, leaderboard, leaderCount, NULL);

                waitForNext("\nPress (y) then (Enter) to go back to View Leaderboard menu: ");
            }

            continue;
        }

        // ---------- In case: select quiz topics (1–5) ----------
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
                printf(C_RED "Invalid choice.\n" C_RESET);
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
            printf(C_RED "No questions found for this topic.\n" C_BOLD);
            wait_ms(DELAY_MED);
            continue;
        }

        int numAsked = 0;
        int score = runQuiz(topicName, questions, totalQ, attempts, &numAsked);

        clearScreen();
        printf(C_BOLD "\n===== Result for %s (%s) =====\n" C_RESET, playerName, topicName);
        printf(C_BOLD "\nScore: %d / %d\n" C_RESET, score, numAsked);
        wait_ms(DELAY_LONG);

        char leaderboardFile[64];
        buildLeaderboardFilename(topicName, leaderboardFile, sizeof(leaderboardFile));

        saveScore(leaderboardFile, playerName, topicName, score);

        int leaderCount = loadLeaderboard(leaderboardFile, leaderboard, MAX_LEADER);
        showLeaderboard(topicName, leaderboard, leaderCount, playerName);

        showWrongAndRetry(questions, attempts, numAsked);
    }

    return 0;
}
