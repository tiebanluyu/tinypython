#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef enum {
    TOK_NUM,
    TOK_ADD,
    TOK_SUB,
    TOK_MUL,
    TOK_DIV,
    TOK_INVALID
} TokenType;

typedef struct {
    TokenType type;
    union {
        double num_val;
    };
} Token;

Token scan_token(const char **str) {
    Token tok = {TOK_INVALID};

    while (**str && isspace(**str)) {
        ++(*str);
    }

    if (**str == '+' || **str == '-') {
        tok.type = **str == '+' ? TOK_ADD : TOK_SUB;
        ++(*str);
    } else if (**str == '*' || **str == '/') {
        tok.type = **str == '*' ? TOK_MUL : TOK_DIV;
        ++(*str);
    } else {
        char *endptr;
        errno = 0;
        tok.num_val = strtod(*str, &endptr);
        if (errno == 0 && endptr != *str) {
            tok.type = TOK_NUM;
            *str = endptr;
        }
    }

    return tok;
}

double parse_expr(const char **str) {
    double left = parse_term(str);
    Token tok;

    while ((tok = scan_token(str)).type == TOK_ADD || tok.type == TOK_SUB) {
        double right = parse_term(str);
        switch (tok.type) {
            case TOK_ADD:
                left += right;
                break;
            case TOK_SUB:
                left -= right;
                break;
            default:
                // Should never happen
                exit(EXIT_FAILURE);
        }
    }

    return left;
}

double parse_term(const char **str) {
    double left = parse_factor(str);
    Token tok;

    while ((tok = scan_token(str)).type == TOK_MUL || tok.type == TOK_DIV) {
        double right = parse_factor(str);
        switch (tok.type) {
            case TOK_MUL:
                left *= right;
                break;
            case TOK_DIV:
                if (right == 0.0) {
                    fprintf(stderr, "Division by zero encountered!\n");
                    exit(EXIT_FAILURE);
                }
                left /= right;
                break;
            default:
                // Should never happen
                exit(EXIT_FAILURE);
        }
    }

    return left;
}

double parse_factor(const char **str) {
    Token tok = scan_token(str);
    if (tok.type == TOK_NUM) {
        return tok.num_val;
    } else {
        fprintf(stderr, "Unexpected token at position %td: expected number, found '%c'\n",
                *str - str[0], tok.type == TOK_INVALID ? '\0' : tok.type);
        exit(EXIT_FAILURE);
    }
}

int main() {
    const char *input = "3.14 + 2 - 0.5 * 4 / 2";  // Example input string

    double result = parse_expr(&input);
    if (*input != '\0') {
        fprintf(stderr, "Input expression not fully consumed. Remaining: '%s'\n", input);
        exit(EXIT_FAILURE);
    }

    printf("Result: %.6f\n", result);

    return 0;
}