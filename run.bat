::rem Bison command
::rem bison -d Parser.y

::rem Flex command
::rem flex Parser.l

::rem GCC commands
::rem gcc -c lex.yy.c -o lex.yy.o
::rem gcc -c Parser.tab.c -o Parser.tab.o

::rem Linking
::rem gcc lex.yy.o Parser.tab.o -o myParser.exe

::rem clear (comentario)
::rem Run the parser
::rem myParser.exe Ejemplo.xml ---

@echo off
echo Compilando el programa C++...
g++ -o Main.exe Main.cpp

if errorlevel 1 (
    echo Error al compilar el programa.
    pause
    exit /b 1
)

echo Programa compilado exitosamente. Ejecutando el programa...
Main.exe

echo Presiona Enter para salir...
pause

