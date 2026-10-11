#ifndef UTILS_H
#define UTILS_H

void read_line(char *buf, int size);
int read_int(const char *prompt, int min, int max);
double read_double(const char *prompt, double min, double max);
void print_bar(double percent);

#endif
