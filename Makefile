CC=gcc
FLEX=flex
BISON=bison

.lex: src/lex.l 
	$(FLEX) -o src/lex.yy.c src/lex.l

.syntax: src/syntax.y
	$(BISON) -t -d -o src/syntax.tab.c src/syntax.y

splc: .lex .syntax
	$(CC) src/syntax.tab.c -lfl -ly -o bin/splc

# splc:
# 	@mkdir bin
# 	touch bin/splc
# 	@chmod +x bin/splc

clean:
	@rm -rf bin/* src/lex.yy.c src/syntax.tab.c src/syntax.tab.h

.PHONY: splc