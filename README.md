# DISCRETE-QUIZ
Final Project for ComProgramming 

**How to use?**
1. Create Folder in your desktop.
2. Download all files and stored it to the desktop folder that you have create.
3. Open cmd and direct to desktop folder and then to folder that you stored all files.
4. Run this command "gcc sourcecode/main.c sourcecode/util.c sourcecode/questions.c sourcecode/leaderboard.c sourcecode/quiz_logic.c -Iinclude -o discrete_quiz.exe -std=c11 -Wall -Wextra".

**Summary of Each File's Function**

quiz.h 
- Contains constant definitions (MAX_Q, DELAY_*), data structures (Question, Attempt, ScoreEntry), and all function prototypes. This allows other files to #include "quiz.h" and share common code.

util.c 
- Contains general utility functions, such as: trimming newlines, converting answers, shuffling question order, adding delays, clearing the screen, and waiting for user input ('y') to proceed.

questions.c 
- Responsible for reading question data from .txt files (e.g., logic.txt) and parsing it into an array of Question structs. Supports 5-choice questions and includes an explanation for each.

leaderboard.c 
- Manages player scores for each topic: reads/writes separate leaderboard files per topic, sorts entries by score, and displays the leaderboard, highlighting the current player's rank.

quiz_logic.c 
- The core logic of the quiz game: loops through randomized questions and stores results in the Attempt struct. Includes the showWrongAndRetry function, which allows the player to retry incorrect questions or review solutions one by one, showing their previous incorrect answers.

main.c 
- Handles the user interface (UI) via a console menu: selecting a quiz topic, opting to only view the leaderboard, reading the player's name, calling functions from other modules, and serving as the program's entry point (main()).
