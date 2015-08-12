digits [0-9]+
identifier [A-Za-z_][A-Za-z_0-9]*

%{
    #include <iostream>
    using namespace std;
    #define YY_DECL extern int yylex()
    #include "tree.h"
    #include "rules.tab.h"
    extern tree my_tree;
    int line_no=1;
%}

%%

"main()"            { return MAIN; }
endtask             { return END_FUNC; }
task                { return FUNC; }
submit              { return RETURN; }
endtest             { return END_IF; }
test                { return IF; }
pass:               { return THEN; }
fail:               { return ELSE; }
enroll              { return DECLARE; }
read                { return INPUT; }
write               { return OUTPUT; }
endcycle            { return END_LOOP; }
cycle               { return WHILE_LOOP; }
rest                { return BREAK; }
next                { return CONTINUE; }
"=="                { return IS_EQUAL_TO; }
"!="                { return IS_NOT_EQUAL_TO; }
"="                 { return ASSIGN; }
">="                { return GREATER_EQUAL; }
">"                 { return GREATER_THAN; }
"<="                { return LESS_EQUAL; }
"<"                 { return LESS_THAN; }
"+"                 { return PLUS; }
"-"                 { return MINUS; }
"*"                 { return MULTIPLY; }
"/"                 { return DIVIDE; }
"%"                 { return MOD; }
"("                 { return LEFT_PAREN; }
")"                 { return RIGHT_PAREN; }
";"                 { return DELIMITER; }
","                 { return COMMA; }
":"                 { return COLON; }
"~"                 { return BITWISE_NOT; }
"&"                 { return BITWISE_AND; }
"|"                 { return BITWISE_OR; }
not                 { return LOGICAL_NOT; }
and                 { return LOGICAL_AND; }
or                  { return LOGICAL_OR; }
#                   { return INT; }
@                   { return BOOL; }
true                { yylval.Node = my_tree.add_node("BOOL", yytext, line_no); return BOOLEAN; }
false               { yylval.Node = my_tree.add_node("BOOL", yytext, line_no); return BOOLEAN; }
{identifier}        { yylval.Node = my_tree.add_node("ID", yytext, line_no); return ID; }
{digits}            { yylval.Node = my_tree.add_node("INT", yytext, line_no); return NUMBER; }
\".*[^\\]\"         { yylval.Node = my_tree.add_node("STRING", yytext, line_no); return STRING; }
\n                  line_no++;
.

%%

int yywrap(){}