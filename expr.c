#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
double parse_term(const char **str);
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
/**
 * 解析因子为双精度浮点数
 * 
 * 此函数用于解析字符串中的第一个因子，并将其作为双精度浮点数返回。
 * 因子预期为一个数字。如果遇到非数字字符，将输出错误信息并终止程序。
 *
 * @param str 指向要解析的字符串的指针的地址。函数会更新此指针，使其指向解析后的位置。
 * @return 返回解析得到的双精度浮点数。
 */
double parse_factor(const char **str) {
    Token tok = scan_token(str); // 扫描并获取第一个标记

    if (tok.type == TOK_NUM) {
        // 如果标记为数字类型，返回该数字的值
        return tok.num_val;
    } else {
        // 如果标记不是数字，输出错误信息并终止程序
        fprintf(stderr, "Unexpected token at position %td: expected number, found '%c'\n",
                *str - str[0], tok.type == TOK_INVALID ? '\0' : tok.type);
        exit(EXIT_FAILURE);
    }
}
/**
 * 解析表达式字符串，并返回计算结果。
 * 
 * @param str 指向要解析的表达式字符串的指针的地址。在函数执行过程中，该指针将被更新为下一个待解析的位置。
 * @return 返回表达式的计算结果。
 */
double parse_expr(const char **str) {
    // 解析第一个术语
    double left = parse_term(str);
    Token tok;

    // 循环，处理加法或减法操作符
    while ((tok = scan_token(str)).type == TOK_ADD || tok.type == TOK_SUB) {
        // 解析右侧的术语
        double right = parse_term(str);
        switch (tok.type) {
            case TOK_ADD:
                left += right; // 加法操作
                break;
            case TOK_SUB:
                left -= right; // 减法操作
                break;
            default:
                // 理论上不应该发生，如果发生，则退出程序
                exit(EXIT_FAILURE);
        }
    }

    return left; // 返回计算结果
}

/**
 * 解析项
 * 该函数用于解析数学表达式中的一个项，支持乘法和除法运算。
 * 
 * @param str 指向当前解析字符串的指针的地址，函数会更新这个指针指向下一个待解析的字符。
 * @return 返回解析出的项的值。
 */
double parse_term(const char **str) {
    // 解析第一个因子
    double left = parse_factor(str);
    Token tok;

    // 循环处理乘法和除法运算，直到遇到其他类型的运算符
    while ((tok = scan_token(str)).type == TOK_MUL || tok.type == TOK_DIV) {
        // 解析右侧的因子
        double right = parse_factor(str);
        switch (tok.type) {
            case TOK_MUL:
                // 执行乘法运算
                left *= right;
                break;
            case TOK_DIV:
                // 执行除法运算，检查除数是否为零
                if (right == 0.0) {
                    fprintf(stderr, "Division by zero encountered!\n");
                    exit(EXIT_FAILURE);
                }
                left /= right;
                break;
            default:
                // 应该不会发生，如果发生则退出程序
                exit(EXIT_FAILURE);
        }
    }
    //printf("left=%f\n",left);
    // 返回解析后的值
    return left;
}



int main() {
    const char *input = "3.14 * 2";  // Example input string

    double result = parse_expr(&input);
    if (*input != '\0') {
        fprintf(stderr, "Input expression not fully consumed. Remaining: '%s'\n", input);
        exit(EXIT_FAILURE);
    }

    printf("Result: %.6f\n", result);

    return 0;
}