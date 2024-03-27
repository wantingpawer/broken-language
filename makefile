all: main.cpp lexer.cpp cfgReader.cpp
	g++ main.cpp lexer.cpp cfgReader.cpp -g -Wall –o compiler
clean:
	$(RM) compiler