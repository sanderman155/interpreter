interpreter:
	g++ src/interpreter.cpp -fpic -shared -o bin/libinterpreter.so -I ./header/
%:
	g++ src/$@.cpp -o bin/$@ -I ./header -L ./bin -linterpreter
