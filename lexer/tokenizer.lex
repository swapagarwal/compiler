digits [0-9]+
identifier [A-Za-z_][A-Za-z_0-9]*

%{
    int token_no=1;
    int line_no=1;
%}

%%
endtask             printf("%d: %s -> %s\n",token_no++,yytext,"end_function");
task                printf("%d: %s -> %s\n",token_no++,yytext,"function");
start               printf("%d: %s -> %s\n",token_no++,yytext,"start");
submit              printf("%d: %s -> %s\n",token_no++,yytext,"return");
endtest             printf("%d: %s -> %s\n",token_no++,yytext,"end_if");
test                printf("%d: %s -> %s\n",token_no++,yytext,"if");
pass                printf("%d: %s -> %s\n",token_no++,yytext,"then");
fail                printf("%d: %s -> %s\n",token_no++,yytext,"else");
enroll              printf("%d: %s -> %s\n",token_no++,yytext,"declare");
read                printf("%d: %s -> %s\n",token_no++,yytext,"input");
write               printf("%d: %s -> %s\n",token_no++,yytext,"output");
endcycle            printf("%d: %s -> %s\n",token_no++,yytext,"end_loop");
cycle               printf("%d: %s -> %s\n",token_no++,yytext,"while_loop");
repeat              printf("%d: %s -> %s\n",token_no++,yytext,"for_loop");
rest                printf("%d: %s -> %s\n",token_no++,yytext,"break");
next                printf("%d: %s -> %s\n",token_no++,yytext,"continue");
"=="                printf("%d: %s -> %s\n",token_no++,yytext,"is_equal_to");
"!="                printf("%d: %s -> %s\n",token_no++,yytext,"is_not_equal_to");
"="                 printf("%d: %s -> %s\n",token_no++,yytext,"assign");
">="                printf("%d: %s -> %s\n",token_no++,yytext,"greater_equal");
">"                 printf("%d: %s -> %s\n",token_no++,yytext,"greater_than");
"<="                printf("%d: %s -> %s\n",token_no++,yytext,"less_equal");
"<"                 printf("%d: %s -> %s\n",token_no++,yytext,"less_than");
"+"                 printf("%d: %s -> %s\n",token_no++,yytext,"plus");
"-"                 printf("%d: %s -> %s\n",token_no++,yytext,"minus");
"*"                 printf("%d: %s -> %s\n",token_no++,yytext,"multiply");
"/"                 printf("%d: %s -> %s\n",token_no++,yytext,"divide");
"%"                 printf("%d: %s -> %s\n",token_no++,yytext,"mod");
"("                 printf("%d: %s -> %s\n",token_no++,yytext,"left_parenthesis");
")"                 printf("%d: %s -> %s\n",token_no++,yytext,"right_parenthesis");
";"                 printf("%d: %s -> %s\n",token_no++,yytext,"delimiter");
"~"                 printf("%d: %s -> %s\n",token_no++,yytext,"bitwise_not");
"&"                 printf("%d: %s -> %s\n",token_no++,yytext,"bitwise_and");
"|"                 printf("%d: %s -> %s\n",token_no++,yytext,"bitwise_or");
not                 printf("%d: %s -> %s\n",token_no++,yytext,"logical_not");
and                 printf("%d: %s -> %s\n",token_no++,yytext,"logical_and");
or                  printf("%d: %s -> %s\n",token_no++,yytext,"logical_or");
true                printf("%d: %s -> %s\n",token_no++,yytext,"boolean_true");
false               printf("%d: %s -> %s\n",token_no++,yytext,"boolean_false");
[#@]?{identifier}   printf("%d: %s -> %s\n",token_no++,yytext,"id");
{digits}            printf("%d: %s -> %s\n",token_no++,yytext,"number");
\".*[^\\]\"         printf("%d: %s -> %s\n",token_no++,yytext,"string");
\n                  line_no++;
.
%%

yywrap(){}

main()
{
    yylex();
    printf("\n%s = %d\n","Total lines",line_no);
}
