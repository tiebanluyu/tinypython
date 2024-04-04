#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expr.c"


double evaluate_expression(const char *expr) {
    double value;
    if (sscanf(expr, "%d", &value) == 1) {
        return value;
    }
    fprintf(stderr, "Invalid expression: %s\n", expr);
    exit(EXIT_FAILURE);
}

void execute_command(char *command, char **args) {
    if (strcmp(command, "print") == 0) {
        
        if (args[0] != NULL) {
            
            double value = expr(&args[0]);
            printf("%f\n", value);
        } else {
            fprintf(stderr, "Missing argument for print command\n");
            exit(EXIT_FAILURE);
        }
    } else if (strcmp(command, "exit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        fprintf(stderr, "Unknown command: %s\n", command);
        exit(EXIT_FAILURE);
    }
}

int main() {
    char input[1024];

    while (printf(">>> "), fgets(input, sizeof(input), stdin)) {
        // Remove trailing newline
        input[strcspn(input, "\n")] = '\0';

        // Split the input into command and arguments
        char *command = strtok(input, " ");
        char *arg = strtok(NULL, "");

        if (command != NULL) {
            execute_command(command, &arg);
        }
    }

    return 0;
}

