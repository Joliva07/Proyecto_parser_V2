::rem Bison command
::rem bison -d Parser.y

::rem Flex command
::rem flex Parser.l

::rem GCC commands
::rem gcc -c lex.yy.c -o lex.yy.o
::rem gcc -c Parser.tab.c -o Parser.tab.o

::rem Linking
::rem gcc lex.yy.o Parser.tab.o -o myParser.exe
::rem Limpiar la bitácora de tokens
echo. > vitacora_tokens.html

::rem Solicitar al usuario la ruta del archivo XML a ejecutar
set /p xmlFilePath=Ingrese la ruta del archivo XML que desea ejecutar: 

::rem Ejecutar el parser con el archivo XML proporcionado por el usuario
myParser.exe "%xmlFilePath%"