%{
#include "parser.tab.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>

extern int flag_err_type;
extern void yyerror(char *message);

int lineno = 1;
int line_init = -1;

char str_buf[256];
char* str_buf_ptr;
FILE *vitacora_tokens_file = NULL;

%}

%option noyywrap
%option case-sensitive
%option yylineno

%x STATE_COMMENT SKIP_COMMENT
%x STATE_INLINE_COMMENT
%x STATE_STRING
%x STRING

LETTER [a-zA-Z]
LETTERS [LETTER]+
DIGIT [0-9]
NZDIGIT [1-9]{DIGIT}*
ALPHANUM ({LETTER}|{DIGIT})
ALPHANUM_ ({ALPHANUM}|_)
ASCII [ !"#$%&'()*+,-./:;<=>?@\[\\\]^_`{|}~]
WHITESPACE [ \t]

%%

"<AUTOMATA_AFN>" { token_print(T_AUTOMATA_AFN_OP); yylval.strval = strdup(yytext); close_vitacora_file(); return T_AUTOMATA_AFN_OP; }
"</AUTOMATA_AFN>" { token_print(T_AUTOMATA_AFN_END); yylval.strval = strdup(yytext); close_vitacora_file(); return T_AUTOMATA_AFN_END; }
"<ALFABETO>" { token_print(T_ALFABETO_OP); yylval.strval = strdup(yytext); close_vitacora_file(); return T_ALFABETO_OP; }
"</ALFABETO>" { token_print(T_ALFABETO_END); yylval.strval = strdup(yytext); close_vitacora_file(); return T_ALFABETO_END; }
"<ESTADO>" { token_print(T_ESTADO_OP); yylval.strval = strdup(yytext); close_vitacora_file(); return T_ESTADO_OP; }
"</ESTADO>" { token_print(T_ESTADO_END); yylval.strval = strdup(yytext); close_vitacora_file(); return T_ESTADO_END; }
"<INICIAL>" { token_print(T_INICIAL_OP); yylval.strval = strdup(yytext); close_vitacora_file(); return T_INICIAL_OP; }
"</INICIAL>" { token_print(T_INICIAL_END); yylval.strval = strdup(yytext); close_vitacora_file(); return T_INICIAL_END; }
"<FINAL>" { token_print(T_FINAL_OP); yylval.strval = strdup(yytext); close_vitacora_file(); return T_FINAL_OP; }
"</FINAL>" { token_print(T_FINAL_END); yylval.strval = strdup(yytext); close_vitacora_file(); return T_FINAL_END; }
"<TRANSICIONES>" { token_print(T_TRANSICIONES_OP); yylval.strval = strdup(yytext); close_vitacora_file(); return T_TRANSICIONES_OP; }
"</TRANSICIONES>" { token_print(T_TRANSICIONES_END); yylval.strval = strdup(yytext); close_vitacora_file(); return T_TRANSICIONES_END; }
"&" { token_print(T_EPSILON); yylval.strval = strdup(yytext); close_vitacora_file(); return T_EPSILON; }
"," { token_print(T_COMMA); yylval.strval = strdup(yytext); close_vitacora_file(); return T_COMMA; }

[0-9]+ { token_print(T_INT); yylval.strval = strdup(yytext); close_vitacora_file(); return T_INT; }
[a-zA-Z] { token_print(T_STRING); yylval.strval = strdup(yytext); close_vitacora_file(); return T_STRING; }

[ \t]+ {}
<<EOF>> { token_print(T_EOF); yylval.strval = strdup(yytext); return T_EOF; }
. { printf("UNRECOGNIZED_CHAR: %s\n", yytext); return ERROR_TOKEN; }
[\n]+ { lineno++; }

"<!--" { BEGIN(STATE_COMMENT); str_buf_ptr = str_buf; *str_buf_ptr = '\0'; }
<STATE_COMMENT>"-->" { BEGIN(INITIAL); if (strstr(str_buf, "--") != NULL) { fprintf(stderr, "Invalid comment on line %d: comment contains '--'\n", lineno); yyerror("Wrong token"); } else { comment_print(); } }
<STATE_COMMENT>.|\n { *str_buf_ptr = *yytext; str_buf_ptr++; *str_buf_ptr = '\0'; ECHO; }

%%

<<<<<<< Updated upstream
void token_print(int token_type) {
    if (FLEX_DEBUG && SHOW_TOKENS) {
        printf("TOKEN FOUND: %s  | LINE: %d\n", yytext, lineno);
=======
void token_print(){
    if(FLEX_DEBUG && SHOW_TOKENS){
        printf("LINE: %d | %s\n", lineno, yytext);
>>>>>>> Stashed changes

        if (vitacora_tokens_file == NULL) {
            vitacora_tokens_file = fopen("vitacora_tokens.html", "a");
            if (vitacora_tokens_file == NULL) {
                perror("Error al abrir el archivo vitacora_tokens.html");
                return;
            }
        }

<<<<<<< Updated upstream
        fprintf(vitacora_tokens_file, "TOKEN FOUND: %s  | LINE: %d\n", yytext, lineno);
=======

        fprintf(vitacora_tokens_file, "LINE: %d | %s\n", lineno, yytext);
>>>>>>> Stashed changes
    }
}

void close_vitacora_file() {
    if (vitacora_tokens_file != NULL) {
        fclose(vitacora_tokens_file);
        vitacora_tokens_file = NULL;
    }
}

void comment_print() {
    if (FLEX_DEBUG and SHOW_COMMENTS) {
        printf("COMMENT FOUND: %s  | LINE: %d\n", yytext, lineno);
    }
}