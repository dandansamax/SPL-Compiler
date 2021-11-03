CC=gcc
FLEX=flex
BISON=bison

.lex: src/lex.l 
	$(FLEX) -o src/lex.yy.c src/lex.l

.syntax: src/syntax.y
	$(BISON) -t -d -v -o src/syntax.tab.c src/syntax.y

.syntax_debug: src/syntax.y
	$(BISON) -t -d -v -o src/syntax.tab.c src/syntax.y --debug

.lex_preprocess: src/preprocess/preprocess.l
	$(FLEX) -o src/preprocess/preprocess.yy.c -P PREPROCESS_ src/preprocess/preprocess.l

splc: .lex .syntax .lex_preprocess
	$(CC) src/syntax.tab.c src/utils/tokentree.c src/preprocess/preprocess.yy.c src/preprocess/preprocess.c src/semantic/semantic.c src/utils/symbol_table.c src/utils/type.c -o bin/splc

# preprocess: .lex_preprocess
# 	$(CC) -o bin/preprocess src/preprocess.yy.c src/preprocess.c


# splc:
# 	@mkdir bin
# 	touch bin/splc
# 	@chmod +x bin/splc

clean:
	@rm -rf bin/* src/lex.yy.c src/syntax.tab.c src/syntax.tab.h src/syntax.output src/preprocess/preprocess.yy.c

.PHONY: splc