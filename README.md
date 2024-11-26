# Slice Text
This is a little programming language project I've been working on for a while. There are other iterations of various success on my github.
This version is written in C++, with only the standard library.

## Building
### Dependencies
There are no dependencies other than cmake and gcc.
### From Commandline
Just run `cmake .` in the project directory and then run `make Lang`. Run with `./Lang.x86_64`!

## Features
Right now, the language lexes symbols, strings, operators, and numeric literals but only supports basic calculator-esque
features such as performing arithmatic with PEMDAS.

Operator | Function
|:-:|---|
\* | Multiplication
/ | Division
\+ | Addition
\- | Subtraction
^ | Exponents
% | Modulo

The language also supports several numeric notations.

Notation | Example | Decimal Equivalent
|---|---|:-:|
Decimal | 21.034 | 21.034
Hex | 0xFA3 | 4003
Binary | 0b11101 | 29
