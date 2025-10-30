# Bracket Programming Language - Technical Documentation

![Demo](demo.mp4)

## 1\. Introduction

**Bracket Language** is an esoteric programming language (esolang) designed to explore unconventional syntax concepts. Its primary feature, which defines its esoteric nature, is the unique combination of two paradigms:

1.  **S-expression-based syntax**, inspired by the LISP family of languages, where logic is encapsulated in nested parentheses.
2.  **Traditional infix notation** for arithmetic and logical operators, typical of languages like C++ or Python.

This hybrid creates an environment that is both structurally rigorous and familiar in its mathematical notation, leading to a unique programming experience.

This document serves as a complete technical reference for the language, intended for users who have the interpreter's executable file.

## 2\. Running the Interpreter

The interpreter is a command-line application that executes code from source files. These files must have the **`.bl`** extension.

#### **Windows**

In the Command Prompt (CMD or PowerShell), execute the command:

```bash
.\bracketLang.exe path\to\your\script.bl
```

#### **Linux / macOS**

In the terminal, after granting execution permissions (`chmod +x ./bracketLang`), execute the command:

```bash
./bracketLang path/to/your/script.bl
```

## 3\. Syntax and Core Concepts

### 3.1. S-expressions

The fundamental unit of code is the S-expression: a list of elements enclosed in parentheses `()`. The first element of the list is typically a function or keyword, and the rest are its arguments. Expressions can be nested.

```lisp
(print "Value: " (+ 10 5))
```

### 3.2. Infix Operators

A key feature of the language is its support for operators in infix notation within S-expressions. Arithmetic expressions are processed from left to right, with no operator precedence.

```lisp
; Correct usage:
(100 - 20 + 5) ; Result: 85 (100-20=80, 80+5=85)

; Complex expression:
(5 * (100 / 20)) ; Result: 25. Parentheses enforce evaluation order.
```

### 3.3. Data Types

The language supports four primary data types:

  * `number`: A 64-bit signed integer.
  * `string`: A sequence of characters.
  * `function`: A user-defined function.
  * `nil`: A representation of no value.

### 3.4. Truthiness

In conditional contexts (e.g., in `if` and `loop`), the following values are treated as **false**:

  * The number `0`
  * An empty string `""`

All other values are treated as **true**.

## 4\. Language Reference

### 4.1. Variables and Environment

-----

**`def`**

  * **Syntax**: `(def name value)`
  * **Description**: Creates a variable with the given name and assigns it the computed `value`. The variable is defined in the current scope.
  * **Example**: `(def result (10 * 2))`

### 4.2. Input / Output

-----

**`print`**

  * **Syntax**: `(print arg1 arg2 ...)`
  * **Description**: Converts all arguments to the `string` type, concatenates them, and prints the result to standard output.
  * **Example**: `(def x 10) (print "The value of x is: " x)`

**`input`**

  * **Syntax**: `(input)` or `(input "prompt")`
  * **Description**: Reads a single line of text from standard input and returns it as a `string`. Can optionally display a `prompt`.
  * **Example**: `(def name (input "Enter your name: "))`

### 4.3. Control Structures

-----

**`if`**

  * **Syntax**: `(if condition expression)`
  * **Description**: Evaluates the `condition`. If it is true, it executes the `expression` and returns its result. Otherwise, it returns `nil`. It does not have an `else` clause.
  * **Example**: `(if (x > 10) (print "x is greater than 10"))`

**`loop`**

  * **Syntax**: `(loop condition expression)`
  * **Description**: Executes the `expression` in a loop as long as the `condition` is true. Returns the value of the last execution of the `expression`.
  * **Example**: `(loop (i < 10) (def i (i + 1)))`

**`do`**

  * **Syntax**: `(do expr1 expr2 ...)`
  * **Description**: Executes a sequence of expressions and returns the result of the last one. Useful for grouping operations within `if` or `loop`.
  * **Example**: `(if (x == 1) (do (print "A") (print "B"))`

### 4.4. User-defined Functions

-----

**`fun`**

  * **Syntax**: `(fun (param1 param2 ...) function_body)`
  * **Description**: Creates an anonymous function (a closure) that can be assigned to a variable. The function "remembers" the scope in which it was created.
  * **Example**:
    ```lisp
    (def add (fun (a b) (
      a + b
    )))
    (print (add 5 3))
    ```

### 4.5. String Operations

-----

  * **`(len string)`**: Returns the length of the string as a `number`.
  * **`(get string index)`**: Returns a single-character `string` from the specified index.
  * **`(set var_name index char)`**: Modifies the character at the specified index in a string variable.
  * **`(ord string)`**: Returns the ASCII code of the first character of the string.
  * **`(chr ascii_code)`**: Returns a single-character `string` from an ASCII code.

### 4.6. Type Conversion and Inspection

-----

  * **`(typeof value)`**: Returns the type of the value as a `string` ("number", "string", "function", or "nil").
  * **`(Number string)`**: Converts a `string` to a `number`.
  * **`(String value)`**: Converts any value to a `string`.

### 4.7. System Functions

-----

**`sys`**

  * **Syntax**: `(sys "command")`
  * **Description**: Executes a command in the system shell and returns its standard output as a `string`.
  * **Example**: `(print (sys "date"))`

**`random`**

  * **Syntax**: `(random min max)`
  * **Description**: Returns a random integer from the inclusive range `[min, max]`.
  * **Example**: `(def dice_roll (random 1 6))`

## 5\. Complete Example: Number Guessing Game

The following script implements a simple game, demonstrating the use of many of the language's features.

```lisp
(def secret_number (random 1 100))
(def guess 0)
(def attempt_counter 0)

(print "Guess a number between 1 and 100.")

(loop (guess != secret_number) (
  do
    (def guess_str (input "Enter your guess: "))
    (def guess (Number guess_str))
    (def attempt_counter (attempt_counter + 1))

    (if (guess < secret_number) (
      print "Too low!"
    ))

    (if (guess > secret_number) (
      print "Too high!"
    ))
))

(print "Congratulations! You guessed the number.")
(print "It took you " + (String attempt_counter) + " attempts.")
```

## 6\. Author
This interpreter and language were created by Kamil Malicki.
