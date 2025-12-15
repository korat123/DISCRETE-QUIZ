@echo off
echo Compiling Discrete Quiz Project...

gcc sourcecode\main.c sourcecode\quiz_logic.c sourcecode\leaderboard.c sourcecode\questions.c sourcecode\util.c -I include -o Discrete_quiz.exe

if %errorlevel% equ 0 (
    echo ===============================
    echo Build Successful! 
    echo Run the program by typing: Discrete_quiz.exe
    echo ===============================
) else (
    echo ===============================
    echo Build Failed! Please check errors above.
    echo ===============================
)

pause