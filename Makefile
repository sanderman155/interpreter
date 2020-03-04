main:
	g++ src/main.cpp src/interpreter.cpp -o bin/main -fsanitize=address,leak -Wall -Werror -I ./header
interpreter:
	g++ src/interpreter.cpp -fpic -shared -o bin/libinterpreter.so -I ./header/
user:
	g++ src/main.cpp -o main -I ./header -L ./bin -linterpreter
