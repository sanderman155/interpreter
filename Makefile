all:
	g++ main.cpp interpreter.cpp -o main -fsanitize=address,leak -Wall -Werror

