CC=gcc
FLEX=flex
BISON=bison

.lex: src/lex.l 
	$(FLEX) -o src/lex.yy.c src/lex.l

.syntax: src/syntax.y
	$(BISON) -t -d -v -o src/syntax.tab.c src/syntax.y

.syntax_debug: src/syntax.y
	$(BISON) -t -d -v -o src/syntax.tab.c src/syntax.y --debug

splc: .lex .syntax
	$(CC) src/syntax.tab.c src/tokentree.c -lfl -o bin/splc

debug: .lex .syntax_debug
	$(CC) src/syntax.tab.c src/tokentree.c -lfl -o bin/splc.debug

.lex_preprocess: src/preprocess.l
	$(FLEX) -o src/preprocess.yy.c src/preprocess.l

preprocess: .lex_preprocess
	$(CC) -lfl -o bin/preprocess src/preprocess.yy.c src/preprocess.c


# splc:
# 	@mkdir bin
# 	touch bin/splc
# 	@chmod +x bin/splc

clean:
	@rm -rf bin/* src/lex.yy.c src/syntax.tab.c src/syntax.tab.h src/syntax.output src/a.out

.PHONY: splc