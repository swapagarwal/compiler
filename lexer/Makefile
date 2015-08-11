lex.yy.c: tokenizer.lex
	flex tokenizer.lex

lexer: lex.yy.c
	gcc lex.yy.c -o lexer

clean:
	rm lex.yy.c lexer
