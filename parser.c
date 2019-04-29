#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {ARRAY, RECORD, LPAREN, IDENT, SEMICOLON, WHILE, IF, BEGIN, CONST,
TYPE, VAR, PROCEDURE, eof, END, ELSE, TIMES, DIV, MOD, AND, OR, PLUS, MINUS,
EQ, NE, LT, GT, LE, GE, COMMA, THEN, RPAREN, RBRAK, DO, PERIOD, NUMBER, NOT, 
OF, LBRAK, COLON, BECOMES, PROGRAM } Symbol;

Symbol sym; //current symbol
char *src; // file data
int len; // length of the file
int pos; // current position in the file
char ch; // current character
int numVal;
//char kwVal;

void getChar() {
    if (pos == len) {
        ch = 0;
    } else {
        ch = src[pos];
        pos++;
    }
}

void isNumber() {
    sym = NUMBER;
    numVal = 0;
    while (('0' <= ch) && (ch <= '9')) {
        numVal = 10 * numVal + (ch - '0');
        getChar();
    }
}

void identKW() {
    int start = pos - 1;
    while (('A' <= ch) && (ch <= 'Z')
        || ('a' <= ch) && (ch <= 'z')
        || ('0' <= ch) && (ch <= '9')) {
        getChar();
    }
    int dist = (pos - 1) - start;
    char kwVal[dist+1];
    strncpy(kwVal, &src[start], dist);
    kwVal[dist] = '\0';

    if (!strcmp(kwVal, "div")) { sym = DIV; }
    else if (!strcmp(kwVal, "mod")) { sym = MOD; }
    else if (!strcmp(kwVal, "and")) { sym = AND; }
    else if (!strcmp(kwVal, "or")) { sym = OR; }
    else if (!strcmp(kwVal, "of")) { sym = OF; }
    else if (!strcmp(kwVal, "then")) { sym = THEN; }
    else if (!strcmp(kwVal, "do")) { sym = DO; }
    else if (!strcmp(kwVal, "not")) { sym = NOT; }
    else if (!strcmp(kwVal, "end")) { sym = END; }
    else if (!strcmp(kwVal, "else")) { sym = ELSE; }
    else if (!strcmp(kwVal, "if")) { sym = IF; }
    else if (!strcmp(kwVal, "while")) { sym = WHILE; }
    else if (!strcmp(kwVal, "array")) { sym = ARRAY; }
    else if (!strcmp(kwVal, "record")) { sym = RECORD; }
    else if (!strcmp(kwVal, "const")) { sym = CONST; }
    else if (!strcmp(kwVal, "type")) { sym = TYPE; }
    else if (!strcmp(kwVal, "var")) { sym = VAR; }
    else if (!strcmp(kwVal, "procedure")) { sym = PROCEDURE; }
    else if (!strcmp(kwVal, "begin")) { sym = BEGIN; }
    else if (!strcmp(kwVal, "program")) { sym = PROGRAM; }
    else {  sym = IDENT; }
}

void comment() {
    while ((0 != ch) && (ch != '}')) {
        getChar();
    }
    if (ch == 0) {
        printf("comment not terminated\n");
    } else {
        getChar();
    }
}

void getSym() {
    while ((0 < ch) && (ch <= ' ')) {
        getChar();
    }
    if (('A' <= ch) && (ch <= 'Z') || ('a' <= ch) && (ch <= 'z')) {
        identKW();
    }
    else if (('0' <= ch) && (ch <= '9')) { isNumber(); }
    else {
        switch (ch)
        {
            case '{': comment(); getSym(); break;
            case '*': getChar(); sym = TIMES; break;
            case '+': getChar(); sym = PLUS; break;
            case '-': getChar(); sym = MINUS; break;
            case '=': getChar(); sym = EQ; break;
            case '<':
                getChar();
                switch (ch)
                {
                    case '=': getChar(); sym = LE; break;
                    case '>': getChar(); sym = NE; break;
                    default:
                        sym = LT;
                }
                break;
            case '>':
                getChar();
                switch (ch)
                {
                    case '=': getChar(); sym = GE; break;
                    default:
                        sym = GT;
                }
                break;
            case ';': getChar(); sym = SEMICOLON; break;
            case ',': getChar(); sym = COMMA; break;
            case ':':
                getChar();
                switch (ch)
                {
                    case '=': getChar(); sym = BECOMES; break;
                    default:
                        sym = COLON;
                }
                break;
            case '.': getChar(); sym = PERIOD; break;
            case '(': getChar(); sym = LPAREN; break;
            case ')': getChar(); sym = RPAREN; break;
            case '[': getChar(); sym = LBRAK; break;
            case ']': getChar(); sym = RBRAK; break;
            default:
                printf("illegal character\n"); getChar(); sym = 0;
        }
    }
}

//-----------------------Symbols breakdown ends--------------------------------

void expression();
void statement();
void typedIds();

void selector() {
    while (sym == PERIOD || sym == LBRAK) {
        if (sym == PERIOD) {
            getSym();
            if (sym == IDENT) { getSym(); }
            else { printf("identifier expected\n"); }
        } else {
            getSym();
            expression();
            if (sym == RBRAK) { getSym(); }
            else { printf("] expected\n"); }
        }
    }
}

void factor() {
    if (sym != IDENT && sym != NUMBER && sym != LPAREN && sym != NOT) {
        printf("expression expected\n");
        while (sym < LPAREN && sym > NOT) {
            getSym();
        }
    }
    if (sym == IDENT) {
        getSym();
        selector();
    } else if (sym == NUMBER) {
        getSym();
    } else if (sym == LPAREN) {
        getSym();
        expression();
        if (sym == RPAREN) { getSym(); }
        else { printf(") expected\n"); }
    } else if (sym == NOT) {
        getSym();
        factor();
    }
}

void term() {
    factor();
    while (sym >= TIMES && sym <= AND) {
        getSym();
        factor();
    }
}

void simpleExpression() {
    if (sym == PLUS) { getSym(); term(); } //move term() after if else block
    else if (sym == MINUS) { getSym(); term(); }
    else { term(); }
    while (sym >= OR && sym <= MINUS) {
        getSym();
        term();
    }
}

void expression() {
    simpleExpression();
    while (sym >= EQ && sym <= GE) {
        getSym();
        simpleExpression();
    }
}

void compoundStatement() {
    if (sym == BEGIN) { getSym(); }
    else { printf("'begin' expected\n"); }
    statement();
    while (sym >= IDENT && sym <= BEGIN) {
        if (sym == SEMICOLON) { getSym(); }
        else { printf("; missing\n"); }
        statement();
    }
    if (sym == END) { getSym(); }
    else { printf("'end' expected\n"); }
}

void statement() {
    if (sym != IDENT && sym != IF && sym != WHILE && sym != BEGIN) {
        printf("statement expected\n");
        getSym();
        while (sym < IDENT && sym > ELSE) {
            getSym();
        }
    }
    if (sym == IDENT) {
        getSym();
        if (sym == LPAREN) {
            getSym();
            if (sym == PLUS || sym == MINUS || sym == IDENT || sym == NUMBER
                || sym == LPAREN || sym == NOT) {
                expression();
                while (sym == COMMA) {
                    getSym();
                    expression();
                }
            }
            if (sym == RPAREN) { getSym(); }
            else { printf("')' expected\n"); }
        } else {
            selector();
            if (sym == BECOMES) { getSym(); expression(); }
            else { printf("':=' expected\n"); }
        }
    } else if (sym == BEGIN) {
        compoundStatement();
    } else if (sym == IF) {
        getSym();
        expression();
        if (sym == THEN) { getSym(); }
        else { printf("'then' expected\n"); }
        statement();
        if (sym == ELSE) { getSym(); statement(); }
    } else if (sym == WHILE) {
        getSym();
        expression();
        if (sym == DO) { getSym(); }
        else { printf("'do' expected\n"); }
        statement();
    }
}

void type() {
    if (sym > IDENT) {
        printf("type expected\n");
        while (sym > eof) {
            getSym();
        }
    }
    if (sym == IDENT) {
        getSym();
    } else if (sym == ARRAY) {
        getSym();
        if (sym == LBRAK) { getSym(); }
        else { printf("'[' expected\n"); }
        expression();
        if (sym == PERIOD) { getSym(); }
        else { printf("'.' expected\n"); }
        if (sym == PERIOD) { getSym(); }
        else { printf("'.' expected\n"); }
        expression();
        if (sym == RBRAK) { getSym(); }
        else { printf("']' expected\n"); }
        if (sym == OF) { getSym(); }
        else { printf("'of' expected\n"); }
        type();
    } else if (sym == RECORD) {
        getSym();
        typedIds();
        while (sym == SEMICOLON) {
            getSym();
            typedIds();
        }
        if (sym == END) { getSym(); }
        else { printf("'end' expected\n"); }
    }
}

void typedIds() {
    if (sym == IDENT) { getSym(); }
    else { printf("identifier expected\n"); }
    while (sym == COMMA) {
        getSym();
        if (sym == IDENT) { getSym(); }
        else { printf("identifier expected\n"); }
    }
    if (sym == COLON) { getSym(); type(); }
    else { printf("':' expected\n"); }
}

void declarations() {
    if (sym < BEGIN && sym > PROCEDURE) {
        printf("'begin' or declaration expected\n");
        while (sym < WHILE && sym > eof) {
            getSym();
        }
    }
    while (sym == CONST) {
        getSym();
        if (sym == IDENT) {
            getSym();
            if (sym == EQ) { getSym(); }
            else { printf("= expected\n"); }
            expression();
        } else {
            printf("constant name expected\n");
        }
        if (sym == SEMICOLON) { getSym(); }
        else { printf("; expected\n"); }
    }
    while (sym == TYPE) {
        getSym();
        if (sym == IDENT) {
            getSym();
            if (sym == EQ) { getSym(); }
            else { printf("= expected\n"); }
            type();
            if (sym == SEMICOLON) { getSym(); }
             else { printf("; expected\n"); }
        } else {
            printf("type name expected\n");
        }
    }
    while (sym == VAR) {
        getSym();
        typedIds();
        if (sym == SEMICOLON) { getSym(); }
        else { printf("; expected\n"); }
    }
    while (sym == PROCEDURE) {
        getSym();
        if (sym == IDENT) { getSym(); }
        else { printf("procedure name expected\n"); }
        if (sym == LPAREN) {
            getSym();
            if (sym == VAR || sym == IDENT) {
                if (sym == VAR) { getSym(); typedIds(); }
                else { typedIds(); }
                while (sym == SEMICOLON) {
                    getSym();
                    if (sym == VAR) { getSym(); typedIds(); }
                    else { typedIds(); }
                }
            } else {
                printf("formal parameters expected\n");
            }
            if (sym == RPAREN) { getSym(); }
            else { printf(") expected\n"); }
        }
        if (sym == SEMICOLON) { getSym(); }
        else { printf("; expected\n"); }
        declarations();
        compoundStatement();
        if (sym == SEMICOLON) { getSym(); }
        else { printf("; expected\n"); }
    }
}

void program() {
    if (sym == PROGRAM) { getSym(); }
    else { printf("'program' expected\n"); }
    if (sym == IDENT) { getSym(); }
    else { printf("program name expected\n"); }
    if (sym == SEMICOLON) { getSym(); }
    else { printf("; expected\n"); }
    declarations();
    compoundStatement();
}

int main(int argc, char *argv[]) {
    src = argv[1];
    len = strlen(src);
    pos = 0;
    getChar(); //get the first char to begin
    getSym(); //and the first symbol
    program();
}
