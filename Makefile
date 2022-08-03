CC=gcc
FLEX=flex
BISON=bison
SRC=src/syntax.tab.c src/utils/tokentree.c src/preprocess/preprocess.yy.c src/preprocess/preprocess.c src/utils/hash_map.c src/utils/symbol_table.c src/utils/type.c src/tac_generator/tac_generator.c src/mips_generator/mips32.c src/utils/tac_block.c src/utils/arg_stack.c src/utils/tac.c

.lex: src/lex.l 
	$(FLEX) -o src/lex.yy.c src/lex.l

.syntax: src/syntax.y
	$(BISON) -t -d -v -o src/syntax.tab.c src/syntax.y

.syntax_debug: src/syntax.y
	$(BISON) -t -d -v -o src/syntax.tab.c src/syntax.y --debug

.lex_preprocess: src/preprocess/preprocess.l
	$(FLEX) -o src/preprocess/preprocess.yy.c -P PREPROCESS_ src/preprocess/preprocess.l

splc: clean .lex .syntax .lex_preprocess
	@mkdir -p bin
	@$(CC) -g -std=gnu99 -fcommon $(SRC) -o bin/splc 2>/dev/null

sym_test:
	cd src/utils/ && $(CC) test_symbol_table.c symbol_table.c hash_map.c type.c -o test_symbol_table && ./test_symbol_table && rm -f test_symbol_table

clean:
	@rm -rf bin src/lex.yy.c src/syntax.tab.c src/syntax.tab.h src/syntax.output src/preprocess/preprocess.yy.c

.PHONY: splc