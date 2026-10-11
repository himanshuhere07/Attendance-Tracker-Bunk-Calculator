#ifndef SUBJECT_H
#define SUBJECT_H

#define NAME_LEN 50
#define DEFAULT_TARGET 75.0

// how safe is the attendance of a subject
typedef enum {
    SAFE,
    WARNING,
    DANGER
} Status;

typedef struct {
    char name[NAME_LEN];
    int attended;
    int total;
} Subject;

// holds all subjects. list is malloc'd and grows with realloc
typedef struct {
    Subject *list;
    int count;
    int capacity;
    double target;
} Tracker;

void init_tracker(Tracker *t);
void free_tracker(Tracker *t);

int add_subject(Tracker *t, const char *name);
int find_subject(Tracker *t, const char *name);
void remove_subject(Tracker *t, int index);

void mark_class(Subject *s, int present);
double get_percentage(Subject *s);
Status get_status(Subject *s, double target);
const char *status_text(Status st);

int classes_can_skip(Subject *s, double target);
int classes_needed(Subject *s, double target);

#endif
