## Interpreter

In this semester work I writed a program, which can execute code in my own simple programming language. The main part of program is my library named as "libinterpreter.so". My programming language have some similar part to the Python and C++. For example, if you want declarated function? you nedd to start with the servise word "def" and program starts with function main.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

  1. **Compile library by using makefile**

  ```
  make interpreter
  ```
  2. **Change library path to work with this library**

  ```
  export LIBRARY_PATH=$(pwd)/lib
  ```
  3. **Compile program which will use this library**(in my case, that program was named as "main")

  ```
  make main
  ```
  4. **Run program**
  ```
  bin/main
  ```
