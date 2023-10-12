rem Flex command
flex lexer.l

rem Bison command
bison -d Parser.y

rem GCC commands
gcc -c lex.yy.c -o lex.yy.o
gcc -c Parser.tab.c -o Parser.tab.o

rem Linking
gcc lex.yy.o Parser.tab.o -o myParser.exe

::rem Solicitar al usuario la ruta del archivo XML a ejecutar
::set /p xmlFilePath=Ingrese la ruta del archivo XML que desea ejecutar: 

::rem Ejecutar el parser con el archivo XML proporcionado por el usuario
myParser.exe Ejemplo.xml
