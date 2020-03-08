interpreter:
	g++ src/interpreter.cpp -fpic -shared -o lib/libinterpreter.so -I ./header/
%:
	g++ src/$@.cpp -o bin/$@ -I ./header -L ./lib -linterpreter
all:
	g++ src/interpreter.cpp -fpic -shared -o lib/libinterpreter.so -I ./header/
	g++ src/main.cpp -o bin/main -I ./header -L ./lib -linterpreter
