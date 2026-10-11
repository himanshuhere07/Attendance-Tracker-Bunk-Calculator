#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "subject.h"

void init_tracker(Tracker *t) {
    t->capacity = 4;
    t->count = 0;
    t->target = DEFAULT_TARGET;
    t->list = malloc(t->capacity * sizeof(Subject));

    if (t->list == NULL) {
        printf("Not enough memory to start.\n");
        exit(1);
    }
}

void free_tracker(Tracker *t) {
    free(t->list);
    t->list = NULL;
    t->count = 0;
    t->capacity = 0;
}

// compares two names ignoring upper/lower case, "maths" == "Maths"
static int same_name(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == *b;
}

int find_subject(Tracker *t, const char *name) {
    for (int i = 0; i < t->count; i++) {
        if (same_name(t->list[i].name, name)) {
            return i;
        }
    }
    return -1;
}

// returns 1 if added, 0 if it already exists, -1 if out of memory
int add_subject(Tracker *t, const char *name) {
    if (find_subject(t, name) != -1) {
        return 0;
    }

    // array is full, double it
    if (t->count == t->capacity) {
        int new_capacity = t->capacity * 2;
        Subject *temp = realloc(t->list, new_capacity * sizeof(Subject));
        if (temp == NULL) {
            return -1;
        }
        t->list = temp;
        t->capacity = new_capacity;
    }

    Subject *s = &t->list[t->count];
    strncpy(s->name, name, NAME_LEN - 1);
    s->name[NAME_LEN - 1] = '\0';
    s->attended = 0;
    s->total = 0;
    t->count++;
    return 1;
}

// shift everything after it one step to the left
void remove_subject(Tracker *t, int index) {
    for (int i = index; i < t->count - 1; i++) {
        t->list[i] = t->list[i + 1];
    }
    t->count--;
}

void mark_class(Subject *s, int present) {
    s->total++;
    if (present) {
        s->attended++;
    }
}

double get_percentage(Subject *s) {
    if (s->total == 0) {
        return 0.0;
    }
    return (double)s->attended / s->total * 100;
}

Status get_status(Subject *s, double target) {
    double pct = get_percentage(s);

    if (s->total == 0 || pct >= target) {
        return SAFE;
    } else if (pct >= target - 10) {
        return WARNING;
    }
    return DANGER;
}

const char *status_text(Status st) {
    switch (st) {
        case SAFE:
            return "SAFE";
        case WARNING:
            return "WARNING";
        case DANGER:
            return "DANGER";
    }
    return "?";
}

// attended / (total + x) >= target/100  -->  x <= attended*100/target - total
// the 1e-9 is there because of floating point errors (got 2.9999999 once)
int classes_can_skip(Subject *s, double target) {
    double max_total = s->attended * 100.0 / target;
    int skip = (int)floor(max_total - s->total + 1e-9);

    if (skip < 0) {
        skip = 0;
    }
    return skip;
}

// (attended + y) / (total + y) >= t  -->  y >= (t*total - attended) / (1 - t)
int classes_needed(Subject *s, double target) {
    if (get_percentage(s) >= target) {
        return 0;
    }

    double t = target / 100.0;
    double need = (t * s->total - s->attended) / (1.0 - t);
    return (int)ceil(need - 1e-9);
}
