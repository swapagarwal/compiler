# Designing a language and building a compiler to generate its MIPS code

The language designed has the following features:

* Global declarations for both functions and variables
* Usual mathematical expressions (bracketing, +, -, *, /, unary negation)
* Integers and Boolean types
* Looping options
* Conditionals
* Nesting
* Type checking
* Input/Output commands
* Recursion

![lexer/language.txt](https://github.com/swapagarwal/compiler/blob/master/lexer/language.txt) -> The grammar for the language designed

![lexer/tokenizer.lex](https://github.com/swapagarwal/compiler/blob/master/lexer/tokenizer.lex) -> A lex code which identifies the tokens for the language

![sample/](https://github.com/swapagarwal/compiler/tree/master/sample) -> Sample programs

The parser identifies multiple syntax errors, displays the syntax tree and produces the MIPS machine code corresponding to a syntactically correct program.

### Requirements

```
sudo apt-get install flex bison spim
```

### Usage

```
make parser
./parser < sample.txt
```

This will create a `parse_tree.txt` file containing the parse tree of the program and a `mips.s` file which contains the MIPS code.

![Demo](https://github.com/swapagarwal/compiler/blob/master/demo.gif)
