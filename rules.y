%{
	#include "tree.h"

	extern int yylex();
	extern int yyparse();
	extern FILE *yyin;
	extern int line_no;
	void yyerror(const char *s);
	tree my_tree;
%}

%union{
	node* Node;
}

%error-verbose
%start program
%token <Node> STRING
%token <Node> NUMBER
%token <Node> ID
%token <Node> BOOLEAN INT BOOL
%token <Node> LOGICAL_NOT LOGICAL_AND LOGICAL_OR
%token <Node> BITWISE_NOT BITWISE_AND BITWISE_OR
%token <Node> DELIMITER COMMA COLON
%token <Node> LEFT_PAREN
%token <Node> PLUS MINUS MULTIPLY DIVIDE MOD
%token <Node> GREATER_EQUAL GREATER_THAN LESS_EQUAL LESS_THAN
%token <Node> ASSIGN
%token <Node> IS_EQUAL_TO IS_NOT_EQUAL_TO
%token <Node> BREAK CONTINUE
%token <Node> WHILE_LOOP
%token <Node> END_LOOP
%token <Node> INPUT OUTPUT
%token <Node> DECLARE
%token <Node> IF THEN ELSE
%token <Node> END_IF
%token <Node> FUNC RETURN
%token <Node> END_FUNC
%token <Node> MAIN
%token <Node>

%left BITWISE_NOT BITWISE_AND BITWISE_OR
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD

%left prec

%nonassoc RIGHT_PAREN
%nonassoc LOGICAL_OR
%nonassoc LOGICAL_AND
%nonassoc LOGICAL_NOT

%type <Node> program program_ main_ function statement statements input output printable declaration assignment condition loop expression term conditional relation function_call args function_header header_args term_arg

%%

program:
	program_ main_
	{ my_tree.root = my_tree.add_node("PROGRAM", "", line_no, $1, $2);
	  my_tree.build_sym_table(my_tree.root);
	  my_tree.type_check(my_tree.root);
	  my_tree.generate_mips_file(my_tree.root);
	  my_tree.better_print(my_tree.root, vector<bool>()); }
	;

program_:
	program_ function
	{ $$ = my_tree.add_node("GLOBAL", "", line_no, $1, $2); }
	| program_ declaration DELIMITER
	{ $$ = my_tree.add_node("DECLARE", "", line_no, $1, $2); }
	|
	{ $$ = NULL; }
	;

main_:
	FUNC MAIN statements END_FUNC
	{ $$ = my_tree.add_node("MAIN", "", line_no, $3); }
	;

function:
	FUNC function_header statements END_FUNC
	{ $$ = my_tree.add_node("FUNCTION", "", line_no, $2, $3); }
	;

statements:
	statement statements
	{ $$ = my_tree.add_node("STATEMENTS", "", line_no, $1, $2); }
	|
	{ $$ = NULL; }
	;

statement:
	input DELIMITER
	{ $$ = my_tree.add_node("INPUT", "", line_no, $1); }
	| output DELIMITER
	{ $$ = my_tree.add_node("OUTPUT", "", line_no, $1); }
	| declaration DELIMITER
	{ $$ = my_tree.add_node("DECLARE", "", line_no, $1); }
	| assignment DELIMITER
	{ $$ = $1; }
	| conditional DELIMITER
	{ $$ = $1; }
	| loop DELIMITER
	{ $$ = $1; }
	| function_call DELIMITER
	{ $$ = $1; }
	| RETURN expression DELIMITER
	{ $$ = my_tree.add_node("RETURN", "", line_no, $2); }
	| BREAK DELIMITER
	{ $$ = my_tree.add_node("BREAK", "", line_no); }
	| CONTINUE DELIMITER
	{ $$ = my_tree.add_node("CONTINUE", "", line_no); }
	| error DELIMITER
	{ $$ = my_tree.add_node("ERROR", "", line_no);yyerrok;yyclearin; }
	;

input:
	INPUT ID
	{ $$ = $2; }
	;

output:
	OUTPUT printable
	{ $$ = $2; }
	;

printable:
	expression
	{ $$ = $1; }
	| STRING
	{ $$ = $1; }
	|
	{ $$ = NULL; }
	;

declaration:	// check
	DECLARE INT ID
	{ $$ = my_tree.add_node("INT", "", line_no, $3); }
	| DECLARE BOOL ID
	{ $$ = my_tree.add_node("BOOL", "", line_no, $3); }
	| DECLARE INT ID ASSIGN expression
	{ $$ = my_tree.add_node("INT", "", line_no, $3, $5); }
	| DECLARE BOOL ID ASSIGN expression
	{ $$ = my_tree.add_node("BOOL", "", line_no, $3, $5); }
	;

assignment:
	ID ASSIGN expression
	{ $$ = my_tree.add_node("ASSIGNMENT", "", line_no, $1, $3); }
	;

conditional:
	IF condition THEN statements ELSE statements END_IF
	{ $$ = my_tree.add_node("IF-THEN-ELSE", "", line_no, $2, $4, $6); }
	| IF condition THEN statements END_IF
	{ $$ = my_tree.add_node("IF-THEN", "", line_no, $2, $4); }
	;

loop:
	WHILE_LOOP condition COLON statements END_LOOP
	{ $$ = my_tree.add_node("WHILE", "", line_no, $2, $4); }
	;

expression:
	LEFT_PAREN expression RIGHT_PAREN
	{ $$ = my_tree.add_node("PARENTHESIS", "", line_no, $2); }
	| BITWISE_NOT expression
	{ $$ = my_tree.add_node("BITWISE NOT", "", line_no, $2); }
	| expression BITWISE_AND expression
	{ $$ = my_tree.add_node("BITWISE AND", "", line_no, $1, $3); }
	| expression BITWISE_OR expression
	{ $$ = my_tree.add_node("BITWISE OR", "", line_no, $1, $3); }
	| expression PLUS expression
	{ $$ = my_tree.add_node("ADD", "", line_no, $1, $3); }
	| expression MINUS expression
	{ $$ = my_tree.add_node("SUBTRACT", "", line_no, $1, $3); }
	| MINUS expression %prec prec
	{ $$ = my_tree.add_node("UNARY NEGATIVE", "", line_no, $2); }
	| expression MULTIPLY expression
	{ $$ = my_tree.add_node("MULTIPLY", "", line_no, $1, $3); }
	| expression DIVIDE expression
	{ $$ = my_tree.add_node("DIVIDE", "", line_no, $1, $3); }
	| expression MOD expression
	{ $$ = my_tree.add_node("MODULO", "", line_no, $1, $3); }
	| term
	{ $$ = my_tree.add_node("TERM", "", line_no, $1); }
	;

term:
	ID
	{ $$ = $1; }
	| NUMBER
	{ $$ = $1; }
	| BOOLEAN
	{ $$ = $1; }
	| function_call
	{ $$ = $1; }
	;

condition:
	LEFT_PAREN condition RIGHT_PAREN
	{ $$ = my_tree.add_node("PARENTHESIS", "", line_no, $2); }
	| LOGICAL_NOT condition
	{ $$ = my_tree.add_node("LOGICAL NOT", "", line_no, $2); }
	| condition LOGICAL_AND condition
	{ $$ = my_tree.add_node("LOGICAL AND", "", line_no, $1, $3); }
	| condition LOGICAL_OR condition
	{ $$ = my_tree.add_node("LOGICAL OR", "", line_no, $1, $3); }
	| expression relation expression
	{ $$ = my_tree.add_node("COMPARE", "", line_no, $1, $2, $3); }
	| expression %prec prec
	{ $$ = my_tree.add_node("EXPRESSION", "", line_no, $1); }
	;

relation:
	IS_EQUAL_TO
	{ $$ = my_tree.add_node("EQUAL", "", line_no); }
	| IS_NOT_EQUAL_TO
	{ $$ = my_tree.add_node("NOT EQUAL", "", line_no); }
	| GREATER_EQUAL
	{ $$ = my_tree.add_node("GREATER EQUAL", "", line_no); }
	| GREATER_THAN
	{ $$ = my_tree.add_node("GREATER", "", line_no); }
	| LESS_EQUAL
	{ $$ = my_tree.add_node("LESS EQUAL", "", line_no); }
	| LESS_THAN
	{ $$ = my_tree.add_node("LESS", "", line_no); }
	;

function_call:
	ID LEFT_PAREN args RIGHT_PAREN
	{ $$ = my_tree.add_node("FUNCTION CALL", "", line_no, $1, $3); }
	;

args:
	expression COMMA args
	{ $$ = my_tree.add_node("ARGUMENT", "", line_no, $1, $3); }
	| expression
	{ $$ = my_tree.add_node("ARGUMENT", "", line_no, $1); }
	|
	{ $$ = NULL; }
	;

function_header:
	INT ID LEFT_PAREN header_args RIGHT_PAREN
	{ $$ = my_tree.add_node("INT", "", line_no, $2, $4); }
	| BOOL ID LEFT_PAREN header_args RIGHT_PAREN
	{ $$ = my_tree.add_node("BOOL", "", line_no, $2, $4); }
	| ID LEFT_PAREN header_args RIGHT_PAREN
	{ $$ = my_tree.add_node("VOID", "", line_no, $1, $3); }
	;

header_args:
	term_arg COMMA header_args
	{ $$ = my_tree.add_node("ARGUMENT", "", line_no, $1, $3); }
	| term_arg
	{ $$ = my_tree.add_node("ARGUMENT", "", line_no, $1); }
	|
	{ $$ = NULL; }
	;

term_arg:
	INT ID
	{ $$ = my_tree.add_node("INT", "", line_no, $2); }
	| BOOL ID
	{ $$ = my_tree.add_node("BOOL", "", line_no, $2); }
	;


%%

int main(int,char**){
	do{
		yyparse();
	}while(!feof(yyin));
}

void yyerror(const char *s){
	cout<<s<<" near line number "<<line_no<<endl;
	//cout<<yytext<<endl;
}
