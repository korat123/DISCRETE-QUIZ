#include "quiz.h"

// รูปแบบบรรทัดในไฟล์ .txt (5 ตัวเลือก)
// M|id|question|A|B|C|D|E|correctLetter|explanation
int loadQuestions(const char *filename, Question q[], int maxQ) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Cannot open question file: %s\n", filename);
        wait_ms(DELAY_MED);
        return 0;
    }

    char line[2048];
    int count = 0;

    while (fgets(line, sizeof(line), fp) && count < maxQ) {
        trimNewline(line);
        if (line[0] == '\0' || line[0] == '#')
            continue;

        char *token = strtok(line, "|");
        if (!token) continue;

        Question *cur = &q[count];

        // ใช้แต่แบบ MCQ
        cur->type = 1;

        // id
        token = strtok(NULL, "|");
        if (!token) continue;
        cur->id = atoi(token);

        // question
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(cur->question, token, MAX_TEXT - 1);
        cur->question[MAX_TEXT - 1] = '\0';

        // A
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(cur->choiceA, token, sizeof(cur->choiceA) - 1);
        cur->choiceA[sizeof(cur->choiceA) - 1] = '\0';

        // B
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(cur->choiceB, token, sizeof(cur->choiceB) - 1);
        cur->choiceB[sizeof(cur->choiceB) - 1] = '\0';

        // C
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(cur->choiceC, token, sizeof(cur->choiceC) - 1);
        cur->choiceC[sizeof(cur->choiceC) - 1] = '\0';

        // D
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(cur->choiceD, token, sizeof(cur->choiceD) - 1);
        cur->choiceD[sizeof(cur->choiceD) - 1] = '\0';

        // E
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(cur->choiceE, token, sizeof(cur->choiceE) - 1);
        cur->choiceE[sizeof(cur->choiceE) - 1] = '\0';

        // correct letter
        token = strtok(NULL, "|");
        if (!token) continue;
        cur->correctChoice = (char)toupper((unsigned char)token[0]);

        // explanation
        token = strtok(NULL, "");
        if (token) {
            strncpy(cur->explanation, token, MAX_TEXT - 1);
            cur->explanation[MAX_TEXT - 1] = '\0';
        } else {
            cur->explanation[0] = '\0';
        }

        cur->correctText[0] = '\0';

        count++;
    }

    fclose(fp);
    return count;
}
