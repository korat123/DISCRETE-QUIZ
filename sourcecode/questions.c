#include "quiz.h"

// Format: M|id|Question|ChoiceA|ChoiceB|ChoiceC|ChoiceD|ChoiceE|CorrectLetter|Explanation|Hint1|Hint2|Tags
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
        if (line[0] == '\0' || line[0] == '#' || line[0] == '[') continue;

        char *token = strtok(line, "|");
        if (!token) continue;

        Question *cur = &q[count];
        cur->type = 1;

        // ID
        token = strtok(NULL, "|"); if (!token) continue;
        cur->id = atoi(token);

        // Question
        token = strtok(NULL, "|"); if (!token) continue;
        strncpy(cur->question, token, MAX_TEXT - 1);
        cur->question[MAX_TEXT - 1] = '\0';

        // Choices A-E
        for (int i = 0; i < 5; i++) {
            token = strtok(NULL, "|");
            if (!token) strcpy(cur->choices[i], ""); 
            else {
                strncpy(cur->choices[i], token, 199);
                cur->choices[i][199] = '\0';
            }
        }

        // Correct Letter -> Index
        token = strtok(NULL, "|"); if (!token) continue;
        char c = (char)toupper((unsigned char)token[0]);
        if (c >= 'A' && c <= 'E') cur->correctIndex = c - 'A';
        else cur->correctIndex = 0;

        // Explanation
        token = strtok(NULL, "|");
        if (token) { strncpy(cur->explanation, token, MAX_TEXT - 1); cur->explanation[MAX_TEXT - 1] = '\0'; } 
        else strcpy(cur->explanation, "");

        // Hint 1
        token = strtok(NULL, "|");
        if (token) { strncpy(cur->hint1, token, MAX_TEXT - 1); cur->hint1[MAX_TEXT - 1] = '\0'; }
        else strcpy(cur->hint1, "No hint available.");

        // Hint 2
        token = strtok(NULL, "|");
        if (token) { strncpy(cur->hint2, token, MAX_TEXT - 1); cur->hint2[MAX_TEXT - 1] = '\0'; }
        else strcpy(cur->hint2, "No second hint.");

        // Tags
        token = strtok(NULL, ""); // เอาจนจบบรรทัด
        if (token) {
            // ลบ | ตัวหน้าถ้ามีติดมา (เผื่อ strtok ทำงานต่างกันในบาง compiler)
             if (token[0] == '|') token++;
            strncpy(cur->tags, token, 99);
            cur->tags[99] = '\0';
        } else strcpy(cur->tags, "General");

        count++;
    }

    fclose(fp);
    return count;
}