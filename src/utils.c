#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

// reads a whole line safely (scanf kept leaving newlines in the buffer for me)
void read_line(char *buf, int size) {
    if (fgets(buf, size, stdin) == NULL) {
        // stdin got closed (Ctrl+D), nothing else to do but quit
        printf("\nInput closed, exiting.\n");
        exit(0);
    }

    char *nl = strchr(buf, '\n');
    if (nl != NULL) {
        *nl = '\0';
    } else {
        // line was longer than the buffer, throw away the rest
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
            ;
        }
    }
}

int read_int(const char *prompt, int min, int max) {
    char line[50];
    int value;

    while (1) {
        printf("%s", prompt);
        read_line(line, sizeof(line));

        if (sscanf(line, "%d", &value) == 1 && value >= min && value <= max) {
            return value;
        }
        printf("Please enter a number between %d and %d.\n", min, max);
    }
}

double read_double(const char *prompt, double min, double max) {
    char line[50];
    double value;

    while (1) {
        printf("%s", prompt);
        read_line(line, sizeof(line));

        if (sscanf(line, "%lf", &value) == 1 && value >= min && value <= max) {
            return value;
        }
        printf("Please enter a number between %.0f and %.0f.\n", min, max);
    }
}

// 20 blocks = 100%
void print_bar(double percent) {
    int filled = (int)(percent / 5);
    if (filled > 20) {
        filled = 20;
    }

    printf("[");
    for (int i = 0; i < 20; i++) {
        if (i < filled) {
            printf("#");
        } else {
            printf("-");
        }
    }
    printf("]");
}
