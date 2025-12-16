#include "quiz.h"


// loading data from leaderboard.txt function
int loadLeaderboard(const char *filename, ScoreEntry entries[], int maxEntries) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return 0; // No files found
    }

    char line[256];
    int count = 0;

    // read user until last line and strtok = tokenizer that divide format type
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

// Save score in leaderboard file function
void saveScore(const char *filename, const char *name, const char *topic, int score) {
    FILE *fp = fopen(filename, "a");
    if (!fp) {
        printf(C_RED "Cannot open leaderboard file for writing.\n" C_RESET);
        wait_ms(DELAY_MED);
        return;
    }
    fprintf(fp, "%s;%s;%d\n", name, topic, score);
    fclose(fp);
}

// Show leaderboard from any topic's leaderboard function
void showLeaderboard(const char *topicName,
                     ScoreEntry entries[], int count,
                     const char *currentPlayerName) {
    if (count == 0) {
        clearScreen();
        printf(C_RED "\nNo leaderboard data yet for topic '%s'.\n" C_RESET, topicName);
        wait_ms(DELAY_MED);
        return;
    }

    // sort score up on points from high tp low
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
    printf(C_BOLD "\n========== Leaderboard - %s ==========\n" C_RESET, topicName);
    printf("\n%-2s %-4s %-20s %-6s\n", "", "Rank", "Name", "Score");

    int maxShow = count;
    if (maxShow > 10) maxShow = 10;

    // Loop for Display name
    for (int i = 0; i < maxShow; i++) {
        int isCurrent = (currentPlayerName != NULL &&
                         strcmp(entries[i].name, currentPlayerName) == 0);
        
        // Display user name with green color in leaderboard 
        if (isCurrent) {
             printf(C_GREEN ">> %-4d %-20s %-6d" C_RESET "\n",
               i + 1, entries[i].name, entries[i].score);
        } else {
             printf("   %-4d %-20s %-6d\n",
               i + 1, entries[i].name, entries[i].score);
        }
    }
    printf("\n'>>' will be your ranking. But if you can't see your ranking, you are not in top 10!\n");
}

// create leaderboard file and named it each topics
// Example: topicName = "Logic" -> "Logic_leaderboard.txt"
void buildLeaderboardFilename(const char *topicName, char *out, size_t outSize) {
    snprintf(out, outSize, "data/%s_leaderboard.txt", topicName);
}
