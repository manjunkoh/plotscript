# plotscript
A kernel based notebook-style interpreter that performs numerical, graphical and symbolic computations. Similar to Jupyter Notebook and Mathematica, but much less complex. Developed with Visual Studio and Qt, and tested on a reference environemnt using Vagrant. 


Instruction
---------------------
Optional:
To have the full experience of the software, Qt framework is required to run notebook.exe
```
Clone Plot-Script repository into a folder.
Create a Build directoru
Cmake Plot-Script repository into build folder with the following command
cmake "Plot-Script directory path" "Build directory path"
cd "Build directory path"
cmake --build .
```
Plot Script Overview
---------------------

A C++ program is a collection of statements, many of which contains expressions -- sequence of characters that when evaluated give a value. For example in the following code, consisting of a single C++ statement,

```
double x = ((1.0 + 2.0)*3.0)/4.0;
```

the statement allocates a stack variable named ``x`` of type ``double`` and assigns its value to the result of the expression ``((1.0 + 2.0)*3.0)/4.0``. 

The syntax of a programming language is the rules that govern when a string of characters represents a valid sequence in the language. Related, semantics is the meaning of the sequence computationally, i.e the result it produces. Some languages have a complicated syntax -- C++ notoriously so. Others are simple but no less powerful for expressing computations. 
then make the project.
For example the syntax of most Lisp-family languages, like [scheme](http://www.schemers.org/), consist solely of expressions. This makes their syntax less complicated. Lisp uses prefix notation to represent an expression. Prefix notation puts the operator first. For example the prefix notation of the expression above is ``(/  (* (+ 1 2) 3) 4)``. In general the syntax is ``(PROC ARG1 ARG2 ... ARGN)``, where ``PROC`` is a _procedure_ with _arguments_ ``ARG1``, ``ARG2``, etc., and each argument can also be an expression. 

Simple syntax makes languages much easier to learn, since there is less to remember, and easier to program in. This makes lisp/scheme syntax a good candidate for _scripting_ _languages_, programs written to extend the run-time capabilities of larger programs. Scripting languages are generally interpreted rather than compiled. An interpreter reads the source code and computes it's result and side effects, without converting (compiling) to machine code [1]. Interpreters then are programs that read programs and produce output. They can usually be invoked a few different ways, for example reading the program to be interpreted from a file or interactively with user input. The latter is called a Read-Eval-Print-Loop or REPL.

Plot Script uses a prefix Lisp notation (also called [s-expressions](https://en.wikipedia.org/wiki/S-expression)). A plotscript program then is just one, possibly very complex, expression. For example the following program is roughly equivalent to the C++ one above.

```
(define x (/  (* (+ 1 2) 3) 4))
```

It computes the result of the numerical expression and assigns the symbol ``x`` to have that value in the _environment_. The environment is a mapping from known symbols to other expressions. When the program starts there is a default environment that gets updated as the program runs adding symbol-expression mappings.

Consider another example, a program that finds the max of two numbers:

```
(begin
 (define a 1)
 (define b pi)
 (if (< a b) b a)
 )
```
The outermost expression is a _special_ _form_ named ``begin``, that simply evaluates each argument in turn, itself evaluating to the last result. Its first argument is an expression that when evaluated adds a _symbol_ ``a`` to the _environment_ with a value of 1. The second argument of ``begin`` is another expression, this time a special form ``define``, that adds a symbol ``b`` to the environment with a value of the built-in symbol ``pi`` (that is, the symbol pi is in the default environment). The third argument is an expression that evaluates the expression ``(< a b)`` evaluating the expression ``b`` if the former evaluates to true, else evaluating the expression ``a``. In this case, since 1 < pi, the if expression evaluates to pi (3.14.159...). Notice white-space (e.g. tabs, spaces, and newlines) is unimportant in the syntax, but can be used to make the code more readable.

There are two equivalent views of the above syntax, as a list of lists or equivalently as a tree, called the _abstract_ _syntax_ _tree_ or AST.

<center>
![](ast.png)
</center>

When viewed as an AST,  the evaluation of the program (the outer-most expression), corresponds to a _post-order_ traversal of the tree, where the children are considered in order left-to-right. Each leaf expression evaluates to itself if it is a literal or, if a symbol, to the expression it maps to in the environment. Then the special-form or procedure is evaluated with its arguments. This continues in the post-order traversal until the root of the AST is evaluated, giving the final result of the program, in this case the expression consisting of a single atom, the numerical value of pi (the max of 1 and pi). If at any time during the traversal this process cannot be completed, the program is invalid and an error is emitted (this will be specified more concretely below). Such an invalid program might be syntactically correct, but not semantically correct. For example suppose the programmer forgot to define a value for ``a``, as in

```
(begin
 (define b pi)
 (if (< a b) b a)
 )
```
This is not a semantically valid program in our language and so interpreting it should produce an error.

The process of converting from the stream of text characters that constitute the candidate program to an AST is called _parsing_. This is typically broken up into two steps, tokenization and AST construction. The tokenization step converts the stream of characters into a list of tokens, where each token is a syntactically meaningful string. In our language this means splitting the stream into tokens consisting of ``(``, ``)``, or strings containing no white-space. For example the stream ``(+ a ( - 4))`` would become the list 

```
"(", "+", "a", "(", "-", "4", ")", ")"
```
This can be implemented as a finite state machine operating on the input stream to produce the token stream (see the Token module).

The process of AST construction then uses the token list to build the AST. Every time a ``(`` token is encountered a new node in the AST is created using the following token in the list. Its children are then constructed recursively in the same fashion. This is called a recursive descent parser since it builds the AST top-down in a recursive fashion. The provided parser is an iterative version of this algorithm (see the parse function).

Initial Plot Script Language Specification
--------------------------------------------

Our initial plotscript language is relatively simple (you will be extending it during the course of the semester). It can be specified as follows.

An _Atom_ has a type and a value. The type may be one of None, Number, or Symbol. The type ``None`` indicates the expression has no value. The possible values of a Number are any IEEE double floating point value, strictly parsed with no trailing characters. The possible values of a Symbol is any string, not containing white-space, not possible to parse as a Number, not beginning with a numerical digit, and not one of the _special_ _forms_ defined below.

Examples of Numbers are: ``1``, ``6.02``, ``-12``, ``1e-4``

Examples of Symbols are: ``a``, ``length``, ``start``

An _Expression_ is an Atom or a special form, followed by a (possibly empty) list of Expressions surrounded by parenthesis and separated by spaces. When an expression consists only of an atom the parenthesis may be omitted.

* ``<atom>``
* ``(<atom>)``
* ``(<atom> <expression> <expression> ...)``

There are two special-form expressions that begin with ``define``, and ``begin``. All other expressions are of the form ``(<symbol> <expression> <expression> ...)`` where the symbol names a _procedure_. Procedures take the one or more arguments and return an expression according to their name. For example the expression ``(+ a b c)`` where ``a``, ``b``, and ``c`` are atoms representing numbers or expressions evaluating to such an atom and the result is an expression consisting of a single number atom. This expression is _m-ary_ meaning it can take m arguments. Some expressions are binary, meaning they take only two arguments, i.e. ``(- a b)`` subtracts b from a. Other are unary, e.g. ``(- 1)``. Thus all procedures have an _arity_ of 1,2,...m.

The _Environment_ is a mapping from symbols to either an Expression or a built-in Procedure. The process of _evaluating_ an Expression may modify the Environment (a side-effect) and results in an expression, which consists of a single Atom.

Our language has the following special-forms:

* ``(define <symbol> <expression>)`` adds a mapping from the symbol to the result of the expression in the environment. It is an error to redefine a symbol. This evaluates to the expression the symbol is defined as (maps to in the environment).
* ``(begin <expression> <expression> ...)`` evaluates each expression in order, evaluating to the last.

Our language has the following built-in procedures:

* ``+``    , m-ary expression of Numbers, returns the sum of the arguments
* ``-``    , unary expression of Numbers, returns the negative of the argument
* ``-``    , binary expression of Numbers, return the first argument minus the second
* ``*``    , m-ary expression of Number arguments, returns the product of the arguments
* ``/``    , binary expression of Numbers, return the first argument divided by the second
* ``^``    , binary procedure (^ a b), the exponential of a to the power b. 
* ``sqrt`` , unary expression of Numbers, returns the square root of the argument
* ``ln``   , unary expression of Numbers, returns the nature log of the argument
* ``sin``  , unary expression of Numbers, returns the sine of the argument
* ``cos``  , unary expression of Numbers, returns the cosin of the argument
* ``tan``  , unary expression of Numbers, returns the tangent of the argument
* ``real`` , unary procedure real to return the real part of a Complex as a Number. 
* ``imag`` , unary procedure imag to return the imaginary part of a Complex as a Number. 
* ``mag``  , unary procedure mag to return the magnitude (absolute value) of a Complex as a Number.
* ``arg``  , unary procedure arg to return the argument (angle or phase) of a Complex as a Number in radians
* ``conj`` , unary procedure conj to return the conjugate of a Complex argument. 
* ``discrete-plot`` 
* ``continuous-plot``
* ``rest`` , unary procedure rest returning a list staring at the second element of the List argument up to and including the last element.
* ``first``, unary procedure first returning the first expression of the List argument. 
* ``list`` , producing an expression of type List, which may hold an arbitrary-sized, ordered, list of expressions of any type, including List itself 
* ``append``, binary procedure append that appends the expression of the second argument to the first List argument.
* ``length``, unary procedure length returning the number of items in a List argument as a Number Expression.
* ``join``  , binary procedure join that joins each of the List arguments into one list.
* ``range`` , binary range that produces a list of Numbers from a lower-bound (the first argument) to an upper-bound (the second argument) in positive increments specified by a third argument.
* ``lambda``, user-defined procedures 
* ``apply`` , built-in binary procedure apply. The first argument is a procedure, the second a list. It treats the elements of the list as the arguments to the procedure, returning the result after evaluation. I
* ``map``   ,binary procedure map that is similar to apply, but treats each entry of the list as a separate argument to the procedure, returning a list of the same size of results.
* ``set-property`` , is a tertiary procedure taking a String expression as it's first argument (the key), an arbitrary expression as it's second argument (the value), and an Expression as the third argument. 
* ``get-property`` , is a binary procedure taking a String expression as it's first argument (the key) and an arbitrary expression as the second argument. 
* ``%start`` , should start an interpreter kernel is a separate thread. It should have no effect if a thread is already running. 
* ``%stop`` , should stop a running interpreter kernel. It should have no effect if a thread is already stopped. 
* ``reset`` , should stop and reset a running interpreter kernel to the default state, clearing the environment.
* ``%exit`` ,should exit the plotscript REPL with EXIT_SUCCESS.
* ``Cntl-C`` (holding down the Control key and pressing the C key) should interrupt a running interpreter kernel evaluation as soon as possible. 

It is an error to evaluate a procedure with an incorrect arity or incorrect argument type.

Our language has the following built-in symbol:

* ``pi``, a Number, evaluates to the numerical value of pi, given by atan2(0, -1)
* ``e`` , defined as Euler's number, i.e. std::exp(1).
* ``I`` , define as complex number
Our language also supports comments using the traditional lisp notation. Any content after and including the character ``;`` up to a newline is considered a comment and ignored by the parser (actually the tokenizer).

See the directory ``tests`` in the repository an example plotscript program demonstrating the above syntax.

Modules
--------

The C++ code implementing the plotscript interpreter is divided into the following modules, consisting of a header and implementation pair (.hpp and .cpp). See the associated linked pages for details.

* Atom Module (``atom.hpp``, ``atom.cpp``): This module defines the variant type used to hold Atoms.
* Expression Module (``expression.hpp``, ``expression.cpp``): This module defines a class named ``Expression``, forming a node in the AST.
* Tokenize Module (``token.hpp``, ``token.cpp``): This module defines the C++ types and code for lexing (tokenizing).
* Parsing Module (``parse.hpp``, ``parse.cpp``): This defines the parse function.
* Environment Module (``environment.hpp``, ``environment.cpp``): This module defines the C++ types and code that implements the plotscript environment mapping.
* Interpreter Module (``interpreter.hpp``, ``interpreter.cpp``):  This module implements a class named "Interpreter`` for parsing and evaluation of the AST representation of the expression.
* Nodebook App Module (``notebook_app.hpp``, ``notebook_app.cpp``) This module is the body of the GUI application. It implement a class name "NotebookApp" for connecting between user input and back end of the software.
* Input Widget Module (``input_widget.hpp``, ``input_widget.cpp``) This module implements a class named "InputWidget" for capturing user input then send to notebook app for generate a result
* Output Widget Module (``output_widget.hpp``, ``output_widget.cpp``) This module implements a class name "OutputWidget" for display the calculation result.
* Key capture Module (``cntlc_tracer.hpp``) This module is to capture control C keys from user.
* Thread safe message queue Module (``message_queue.h``) This module is to create a thread safe queue to transfer data between multiple threads.
	
Driver Program Specification
-----------------------------------

The interpreter module needs some user interface code to be useful to a user. The starter code includes a command-line application that compiles to an executable named ``plotscript.exe`` on Windows and just ``plotscript`` on mac/linux. The executable is usable in one of three ways:

To execute short simple programs, pass a flag ``-e`` followed by a quoted string with the program. For example (> is the prompt):

```
> plotscript -e "(+ 1 (- 3) 12)"
```

This evaluates the program in the string and prints the result in the format below or produces an appropriate error message, beginning with "Error", if the program cannot be parsed or encounters a semantic error. If an error occurs plotscript returns ``EXIT_FAILURE`` from main, otherwise it returns ``EXIT_SUCCESS``.

To execute programs stored in external files, provide the file-name containing the plotscript program as a command-line argument. For example, assuming a file named ``mycode.pls`` is in the current working directory with the executable:

```
> plotscript mycode.pls
```

This evaluates the program in the file and prints the result in the format below or produces an appropriate error message, beginning with "Error", if the program cannot be parsed or encounters a semantic error. If an error occurs plotscript returns ``EXIT_FAILURE`` from main, otherwise it returns ``EXIT_SUCCESS``.

For interactive execution of programs using a REPL, just type the executable name:

```
> plotscript
```

This prints a prompt ``plotscript> `` to standard output and waits for the user to type an expression on standard input. It then evaluates the provided expression and prints the result in the format below, or prints an error message, beginning with "Error", if the line cannot be parsed or encounters a semantic error during evaluation. If a semantic error is encountered during evaluation the environment is _not_ reset to the default state (i.e. it retains any defines encountered before the error). After printing the result the REPL prompts again. This continues until the user types the EOF character (Control-k on Windows and Control-d on unix). Changes to the environment are persistent during the use of the REPL. If the user provides an empty line at the REPL (just types Enter) it just ignore the input and prompts again.

**Output Format**: Expressions returned from the interpreter evaluation are printed as ``(<atom>)``. Errors are printed on a single line as the string "Error: " followed by an error message describing the error.

Example transcripts of use:

Executing a simple example at the command line:

```
> plotscript -e "(* 2 3)"
(6)
```
Execute the program in a file (showing it first using cat):

```
> cat program.pls
; define and add two numbers
(begin
  (define a 1)
  (define b 2)
  (+ b a)
)
> plotscript program.pls
(3)
```

Execute some expressions in the REPL:

```
> plotscript
plotscript> (define a 12)
(12)
plotscript> (define b 10)
(10)
plotscript> (- a b c)
Error: unknown symbol
plotscript> (- a b)
(2)
plotscript> (- 12 10)
(2)
```

Execute some expressions in the NotebookApp:
plot the linear function y = f[x] = 2x+1 for x in [-2, -1.5, -1, ..., 2]
```
(begin
    (define f (lambda (x) 
        (list x (+ (* 2 x) 1))))
    (discrete-plot (map f (range -2 2 0.5))
       (list
       (list "title" "The Data")
       (list "abscissa-label" "X Label")
       (list "ordinate-label" "Y Label")
       (list "text-scale" 1))))
```
plot the linear function f(x) = 2x+1 for x in [-2, 2]
```
(begin
    (define f (lambda (x) 
        (+ (* 2 x) 1))) 
    (continuous-plot f (list -2 2)
        (list
        (list "title" "A continuous linear function")
        (list "abscissa-label" "x")
        (list "ordinate-label" "y"))))
```
Unit Tests
-------------

Each module of code above has a set of unit tests covering its functionality using Catch, e.g. basic tests for the interpreter module are included in the file ``interpreter_tests.cpp``. These tests are built as part of the overall project using CMake as described below. These are examples of the kind of tests you will be writing during the semester.

Using CMake to build and test the software
--------------------------------------------

The repository contains a ``CMakeLists.txt`` file that sets up the tests and builds the plotscript executable. 

In the virtual machine this translates to the following:

```
> cd ~
> cmake /vagrant
> make
> make test
```

This treats the source directory as the shared host directory (``/vagrant``) and places the build in the home directory of the virtual machine user (``/home/vagrant``). Using CMake on your host system will vary slightly by platform and compiler/IDE.

The reference environment also includes tools for memory and coverage analysis. To run them (after doing the above):

```
> make memtest
> make coverage
