#include "quiz.h"

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
        wait_ms(DELAY_MED);
        return;
    }
    fprintf(fp, "%s;%s;%d\n", name, topic, score);
    fclose(fp);
}

void showLeaderboard(const char *topicName,
                     ScoreEntry entries[], int count,
                     const char *currentPlayerName) {
    if (count == 0) {
        clearScreen();
        printf("\nNo leaderboard data yet for topic '%s'.\n", topicName);
        wait_ms(DELAY_MED);
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

    clearScreen();
    printf("\n===== Leaderboard - %s =====\n", topicName);
    printf("%-2s %-4s %-20s %-6s\n", "", "Rank", "Name", "Score");

    int maxShow = count;
    if (maxShow > 10) maxShow = 10;

    for (int i = 0; i < maxShow; i++) {
        int isCurrent = (currentPlayerName != NULL &&
                         strcmp(entries[i].name, currentPlayerName) == 0);
        const char *marker = isCurrent ? ">>" : "  ";

        printf("%-2s %-4d %-20s %-6d\n",
               marker,
               i + 1,
               entries[i].name,
               entries[i].score);
    }

    printf("\nnote that: rows with '>>' mean your position in leaderboard\n");
    wait_ms(DELAY_LONG);
}

// สร้างชื่อไฟล์ leaderboard ตามชื่อ topic
// เช่น topicName = "Logic" -> "Logic_leaderboard.txt"
void buildLeaderboardFilename(const char *topicName, char *out, size_t outSize) {
    snprintf(out, outSize, "data/%s_leaderboard.txt", topicName);
}
