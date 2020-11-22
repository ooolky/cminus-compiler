cminus compiler: lex.l parser.y
	bison -d -v parser.y
	flex lex.l 
	g++ -o ./$@ parser.tab.c utils.c lex.yy.c ast.c analysis.cpp -lfl