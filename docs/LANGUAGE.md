# Sinful Language Reference

## 1. Language Overview

Sinful is a compiled, statically typed programming language that targets x86-64 assembly (MASM syntax). The compiler reads a `.sin` source file, produces a MASM-compatible `.asm` file, which can then be assembled and linked with the Windows kernel32 library to produce a native Windows executable.

The compilation pipeline is:

```
source.sin  ->  Lexer  ->  TokenStream  ->  Parser  ->  AST  ->  Generator  ->  output.asm
```

The output assembly is a single flat `main PROC` using the Microsoft x64 calling convention. There are no functions, imports, or modules at the language level; the entire program runs as one procedure.

---

## 2. Formal Grammar

The following EBNF grammar is derived directly from the parser. Terminal symbols are shown in `"quotes"`. Token class names (produced by the lexer) are shown in `UPPER_CASE`.

```ebnf
program         ::= statement* EOF

block           ::= "{" statement* "}"

statement       ::= block
                  | print-stmt
                  | assignment

print-stmt      ::= "print" expression ";"

assignment      ::= VARIABLE "=" [ type-keyword ] expression ";"
                  | VARIABLE "=" type-keyword ";"

type-keyword    ::= "i32"
                  | "bool"

expression      ::= term ( ("+" | "-") term )*

condition       ::= term ( ("==" | "<" | "<=" | ">" | ">=") expression )?

term            ::= factor ( ("*" | "/") factor )*

factor          ::= INT-LITERAL
                  | "true"
                  | "false"
                  | VARIABLE
                  | "(" expression ")"

VARIABLE        ::= ALPHA ALNUM*
INT-LITERAL     ::= DIGIT+
ALPHA           ::= [a-zA-Z]
ALNUM           ::= [a-zA-Z0-9]
DIGIT           ::= [0-9]
```

### Notes on the grammar

- `condition` is defined in the parser source (`parseCondition`) but is not currently reachable from `statement`. It is present for future use (e.g., conditionals or loops) but no statement form calls it.
- The `!` and `!=` tokens are recognised by the lexer but not consumed by any production in the current parser.
- `//` begins a line comment; everything from `//` to the end of the line is discarded.
- The lexer is line-oriented: each source line is scanned independently.

---

## 3. Type System

### Primitive types

| Type   | Keyword | Size   | Default value |
|--------|---------|--------|---------------|
| 32-bit signed integer | `i32`  | 4 bytes | `0`     |
| Boolean               | `bool` | 1 byte  | `false` |

There are no other types. Strings, floats, arrays, and pointers do not exist in the language.

### Type inference

A variable's type is inferred from the right-hand side of its first assignment. No annotation is required if the value is a literal or an expression whose type is already resolved.

```
x = 42;       // inferred: i32
y = true;     // inferred: bool
```

### Explicit type annotation

A type keyword placed between `=` and the expression forces the expression to match that type. If it does not, a compile error is emitted.

```
z = i32 42;       // valid
w = bool true;    // valid
bad = i32 true;   // error: expected 'i32' but found 'bool'
```

### Default initialisation

When a type keyword appears with no following expression (i.e., the next token is `;`), the variable is declared and initialised to the type's default value.

```
x = i32;    // x is declared as i32 and set to 0
y = bool;   // y is declared as bool and set to false
```

### Type checking rules

- Both operands of a binary arithmetic expression (`+`, `-`, `*`, `/`) must have the same resolved type. If one operand is an unresolved variable reference and the other has a known type, the unresolved side is inferred to match.
- If both operands are already resolved to different types, a compile error is emitted: `"Expression operand types did not match"`.
- `print` accepts only `i32` expressions. Passing a `bool` expression to `print` is a compile error.
- Comparison operators (`==`, `<`, `<=`, `>`, `>=`) produce a `bool` result regardless of the operand types.

### The `Unresolved` internal state

When a variable reference is first parsed, its type is marked `Unresolved` because the symbol table is not consulted during parsing. Type resolution happens during the `resolveNodeTypes` pass, which propagates the known type of one operand to an unresolved operand. If both sides of a binary expression are `Unresolved`, no type information is propagated at parse time; the generator will then use whatever value is in the register.

---

## 4. Operators

### Arithmetic operators

| Operator | Symbol | Associativity | Precedence (higher = tighter) |
|----------|--------|---------------|-------------------------------|
| Addition | `+`    | Left          | 1 (lowest)                    |
| Subtraction | `-` | Left          | 1                             |
| Multiplication | `*` | Left       | 2                             |
| Division | `/`    | Left          | 2                             |

Precedence is enforced by the recursive-descent structure: `parseExpression` handles `+` and `-`, then calls `parseTerm` which handles `*` and `/`.

Example: `5 + 2 * 3` parses as `5 + (2 * 3)`.

### Comparison operators

Comparison operators are only reachable via `parseCondition`, which is not currently connected to any statement form. They are listed here for completeness.

| Operator         | Symbol | Result type |
|------------------|--------|-------------|
| Equal            | `==`   | `bool`      |
| Not equal        | `!=`   | (lexed, not parsed) |
| Less than        | `<`    | `bool`      |
| Less or equal    | `<=`   | `bool`      |
| Greater than     | `>`    | `bool`      |
| Greater or equal | `>=`   | `bool`      |

### Assignment

`=` is not an expression operator. It is a statement-level construct only and does not produce a value.

### Grouping

Parentheses `(` `)` override precedence in any expression context.

```
x = (2 + 3) * 4;   // x = 20
```

---

## 5. Scoping Rules

### Blocks create nested scopes

A block `{ ... }` introduces a new lexical scope. Variables declared inside a block are local to that block and are not accessible outside it.

```
x = 10;
{
    y = 20;
    print x;    // valid: x is visible from the outer scope
}
print y;        // error: 'y' is not in scope here (unrecognised variable)
```

### Variable lookup

Variable lookup traverses parent scopes. If a name is not found in the current scope, each enclosing scope is searched in order until the global scope is reached. If the name is not found anywhere, the error `"Unrecognised variable '<name>'"` is emitted.

### Variable shadowing

A variable declared in an inner scope may share a name with a variable in an outer scope only if the inner declaration is a new declaration (sets `mustDeclare = true`). The inner declaration shadows the outer one for the duration of the block. Attempting to redeclare a name that already exists in the same scope is an error: `"Variable '<name>' is already defined in this scope"`.

### Lifetime

Variables exist on the stack for the duration of their enclosing block. Because the generator allocates all stack space up front in the function prologue (computing total stack size from a pre-pass), variable slots are not dynamically pushed or popped at block entry/exit. The scoping restriction is enforced by the symbol table at compile time rather than by stack manipulation at run time.

### Initialisation

A variable that has been declared but never assigned produces the error `"Variable '<name>' has not been initialised"` when it is read. Assignment to an existing variable marks it as initialised in the scope that owns the symbol entry, including when the assignment occurs in a nested scope.

---

## 6. Built-in Statements

### `print`

```
print <expression> ;
```

Evaluates `<expression>`, converts the resulting `i32` value to its decimal ASCII representation (with a trailing newline), and writes it to the standard output handle using the Windows API `WriteConsoleA`.

- The operand must be an `i32` expression. Passing any other type is a compile error.
- Negative values are supported; a leading `-` character is prepended.
- Output is always followed by a newline character (`0x0A`).
- The internal conversion buffer is 24 bytes; values outside the range of a signed 64-bit integer as handled by the `idiv` instruction could overflow this buffer (no bounds check is performed at runtime).

There are no other built-in statements. There is no `read`, `if`, `while`, function call syntax, or any other control-flow construct at this time.

---

## 7. Error Reporting

### Error format

All errors are printed to `stderr` in the following format:

```
<filename>: line <line>: col <col>: error: <message>
 <line-number> | <source line text>
               | <caret pointing to column>
```

The filename, line number, and column are embedded in every token at lex time and propagated through AST nodes. The caret (`^`) is rendered in green; the word `error:` and the message are rendered in bold red (using ANSI escape codes).

### Lexer errors

| Condition | Message |
|-----------|---------|
| Digit immediately followed by a letter | `"Variable name cannot start with numeric value"` |
| Unrecognised character | `"Unrecognised character encountered while lexing: <char>"` |

### Parser errors

| Condition | Message |
|-----------|---------|
| Token does not start a recognised statement | `"Statement does not conform to recognised pattern"` |
| Expected a specific token but found another | `"<context message>\nExpected '<token>' but found '<lexeme>'"` |
| Unclosed `{` block | `"Unclosed scope"` |
| Unclosed `(` group | `"Unclosed parentheses"` |
| Factor position is not a literal, variable, or `(` | `"Expected literal or variable but found '<lexeme>'"` |
| Type annotation does not match expression | `"Expression did not match stated type\nExpected '<type>' but found '<type>'"` |
| `print` given a non-`i32` expression | `"May only print numeric values\nExpected 'i32' but found '<type>'"` |
| Binary expression operand types mismatch | `"Expression operand types did not match\nExpected '<type>' but found '<type>'"` |
| Type keyword expected for default init but something else found | `"Expected type token but received '<token>'"` |

### Code-generation / symbol-table errors

| Condition | Message |
|-----------|---------|
| Reading an uninitialised variable | `"Variable '<name>' has not been initialised"` |
| Reading an undefined variable | `"Unrecognised variable '<name>'"` |
| Redeclaring a variable in the same scope | `"Variable '<name>' is already defined in this scope"` |

### Internal errors

Any `std::exception` that is not a `CompilerException` is caught and printed to `stderr` as:

```
Internal Compiler Error: <what()>
```

These indicate bugs in the compiler itself rather than errors in the source program.

---

## 8. Code Generation Target

### Architecture and ABI

| Property | Value |
|----------|-------|
| Target architecture | x86-64 |
| Calling convention | Microsoft x64 (Windows) |
| Output format | MASM (Microsoft Macro Assembler) syntax |
| Output file extension | `.asm` |

### Runtime dependencies

The generated assembly links against `kernel32.lib` and uses three Win32 API functions:

| Function | Purpose |
|----------|---------|
| `GetStdHandle` | Retrieves the standard output handle once at program start (only emitted when `print` is used) |
| `WriteConsoleA` | Writes ASCII bytes to the console |
| `ExitProcess` | Terminates the process with exit code `0` |

`kernel32.lib` must be available to the assembler/linker. No other libraries are used.

### Stack frame layout

The generator creates a single stack frame for `main`:

1. `rbp` is saved and set to `rsp` (standard frame pointer setup).
2. `rdi` and `rbx` are saved (non-volatile registers used internally).
3. Stack space is reserved in one `sub rsp, N` instruction. `N` is calculated as `32` (shadow space for Win32 calls) plus 8 bytes per declared variable, rounded up to the nearest 16-byte boundary.
4. All local variables are placed at negative offsets from `rbp` (`[rbp-8]`, `[rbp-16]`, etc.) in declaration order. Each slot is 8 bytes regardless of the variable's logical type size.
5. On exit the frame is torn down in reverse order before calling `ExitProcess`.

### Expression evaluation convention

Arithmetic expressions are evaluated using `rax` as the accumulator. When a binary expression is computed, the left operand is pushed to the stack, the right operand is evaluated into `rax`, then the left operand is popped back into `rax` and the operation is performed. The result is always left in `rax`.

- `+` uses `add rax, rbx`
- `-` uses `sub rax, rbx`
- `*` uses `imul rax, rbx`
- `/` uses `idiv rbx` with `rdx` zeroed beforehand (unsigned upper half)

### Integer-to-string conversion

The `print` statement performs conversion inline (no libc dependency). The algorithm fills a 24-byte global buffer (`global_buffer`) from right to left using repeated division by 10. A separate `negative_flag` byte records whether the input was negative; if so, a `-` character is prepended. A newline byte (`0x0A`) is always appended after the last digit.

### Data section

The `.data` section is only non-empty when `print` appears in the source. When present it contains:

```asm
STD_OUTPUT_HANDLE equ -11
hStdOut           dq 0
negative_flag     db 0
global_buffer     db 24 dup(0)
```

No string literals or other data are emitted by statements other than `print`.
