# PL0_C_Parser

A recursive descent parser designed for a subset of the PL0 language, written in C. The following grammar was used:
```
selector ::= {"." ident | "[" expression "]"}.
factor ::= ident selector | integer | "(" expression ")" | "not" factor.
term ::= factor {("*" | "div" | "mod" | "and") factor}.
simpleExpression ::= ["+" | "-"] term {("+" | "-" | "or") term}.
expression ::= simpleExpression
    {("=" | "<>" | "<" | "<=" | ">" | ">=") simpleExpression}.
compoundStatement = "begin" statement {";" statement} "end"
statement ::=
    ident selector ":=" expression |
    ident "(" [expression {"," expression}] ")" |
    compoundStatement |
    "if" expression "then" statement ["else"statement] |
    "while" expression "do" statement.
type ::=
    ident |
    "array" "[" expression ".." expression "]" "of" type |
    "record" typedIds {";" typedIds} "end".
typedIds ::= ident {"," ident} ":" type.
declarations ::=
    {"const" ident "=" expression ";"}
    {"type" ident "=" type ";"}
    {"var" typedIds ";"}
    {"procedure" ident ["(" [["var"] typedIds {";" ["var"] typedIds}] ")"] ";"
        declarations compoundStatement ";"}.
program ::= "program" ident ";" declarations compoundStatement.
```

compile the file with gcc
```sh
$ gcc -o parser parser.c
```
the program takes as an argument a file's contents to be evaluated in full.
```sh
$ ./parser "`cat [filename]`"
```
please note all valid PL0 programs in this implementation must be concluded with a '.'

A sample test case would be formated as follows:
```
program arithmetic;
  var x, y, q, r: integer;
  procedure QuotRem(x, y: integer; var q, r: integer);
    begin q := 0; r := x;
      while r >= y do { q*y+r=x and r>=y }
        begin r := r - y; q := q + 1
        end
    end;
  begin
    read(x); read(y);
    QuotRem(x, y, q, r);
    write(q); write(r); writeln
  end.
```
