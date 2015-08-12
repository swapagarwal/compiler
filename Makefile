rules.tab.c rules.tab.h: rules.y
	bison -d rules.y -v

lex.yy.c: tok.l rules.tab.h
	flex tok.l

parser: lex.yy.c rules.tab.c rules.tab.h
	g++ rules.tab.c lex.yy.c -lfl -o parser

clean:
	rm lex.yy.c parser rules.tab.c rules.tab.h rules.output
