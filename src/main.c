#include <stdio.h>
#include <string.h>
#include "subject.h"
#include "storage.h"
#include "utils.h"

#define DATA_FILE "data/attendance.txt"

static void print_usage(const char *prog) {
    printf("Usage:\n");
    printf("  %s          open the menu\n", prog);
    printf("  %s report   only print the attendance report\n", prog);
    printf("  %s help     show this message\n", prog);
}

static void save_or_warn(Tracker *t) {
    if (!save_data(t, DATA_FILE)) {
        printf("Warning: couldn't save to %s (run the program from the project folder).\n", DATA_FILE);
    }
}

// prints the list and returns the index of the chosen subject, -1 if there are none
static int choose_subject(Tracker *t) {
    if (t->count == 0) {
        printf("\nNo subjects yet, add one first.\n");
        return -1;
    }

    printf("\n");
    for (int i = 0; i < t->count; i++) {
        printf("  %d. %s\n", i + 1, t->list[i].name);
    }
    return read_int("Pick a subject number: ", 1, t->count) - 1;
}

static void show_report(Tracker *t) {
    int sum_attended = 0;
    int sum_total = 0;

    if (t->count == 0) {
        printf("\nNothing to show yet, add a subject first.\n");
        return;
    }

    printf("\nTarget attendance: %.0f%%\n\n", t->target);
    printf("%-3s %-20s %-10s %-8s %s\n", "No", "Subject", "Attended", "Percent", "Status");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < t->count; i++) {
        Subject *s = &t->list[i];
        double pct = get_percentage(s);
        Status st = get_status(s, t->target);

        printf("%-3d %-20s %3d/%-6d %6.1f%%  %s\n",
               i + 1, s->name, s->attended, s->total, pct, status_text(st));

        printf("    ");
        print_bar(pct);
        if (s->total == 0) {
            printf("  no classes yet\n");
        } else if (pct >= t->target) {
            printf("  can skip %d more\n", classes_can_skip(s, t->target));
        } else {
            printf("  attend next %d in a row\n", classes_needed(s, t->target));
        }

        sum_attended += s->attended;
        sum_total += s->total;
    }

    printf("------------------------------------------------------------\n");
    if (sum_total > 0) {
        printf("Overall: %d/%d (%.1f%%)\n", sum_attended, sum_total,
               (double)sum_attended / sum_total * 100);
    }
}

static void add_subject_menu(Tracker *t) {
    char name[NAME_LEN];

    printf("\nSubject name: ");
    read_line(name, sizeof(name));

    if (strlen(name) == 0) {
        printf("Name can't be empty.\n");
        return;
    }
    if (strchr(name, ',') != NULL) {
        printf("Please don't use commas in the name (it breaks the save file).\n");
        return;
    }

    int result = add_subject(t, name);
    if (result == 1) {
        printf("Added %s.\n", name);
        save_or_warn(t);
    } else if (result == 0) {
        printf("%s is already in the list.\n", name);
    } else {
        printf("Ran out of memory, couldn't add it.\n");
    }
}

static void mark_menu(Tracker *t) {
    int idx = choose_subject(t);
    if (idx == -1) {
        return;
    }

    Subject *s = &t->list[idx];
    int present = read_int("Were you present? (1 = present, 0 = absent): ", 0, 1);
    mark_class(s, present);

    printf("%s -> %d/%d (%.1f%%)\n", s->name, s->attended, s->total, get_percentage(s));
    save_or_warn(t);
}

static void bunk_calculator(Tracker *t) {
    int idx = choose_subject(t);
    if (idx == -1) {
        return;
    }

    Subject *s = &t->list[idx];
    if (s->total == 0) {
        printf("No classes recorded for %s yet.\n", s->name);
        return;
    }

    double pct = get_percentage(s);
    printf("\n%s: %d/%d (%.1f%%)\n", s->name, s->attended, s->total, pct);

    if (pct >= t->target) {
        printf("You can skip %d more class(es) and still stay at or above %.0f%%.\n",
               classes_can_skip(s, t->target), t->target);
    } else {
        printf("You are below %.0f%%. Attend the next %d class(es) in a row to get back.\n",
               t->target, classes_needed(s, t->target));
    }

    int n = read_int("\nWhat if you skip the next N classes? (0 to skip this): ", 0, 500);
    if (n > 0) {
        double after = (double)s->attended / (s->total + n) * 100;
        printf("If you skip %d, you will be at %.1f%%.\n", n, after);
        if (after < t->target) {
            printf("That is below your target!\n");
        }
    }
}

// for classes that happened before you started using the tracker
static void set_manually(Tracker *t) {
    int idx = choose_subject(t);
    if (idx == -1) {
        return;
    }

    Subject *s = &t->list[idx];
    int total = read_int("Total classes held so far: ", 0, 1000);
    int attended = read_int("Classes you attended: ", 0, total);

    s->total = total;
    s->attended = attended;
    printf("%s updated -> %d/%d\n", s->name, s->attended, s->total);
    save_or_warn(t);
}

static void change_target(Tracker *t) {
    t->target = read_double("New target percentage (50 - 99): ", 50, 99);
    printf("Target is now %.0f%%.\n", t->target);
    save_or_warn(t);
}

static void delete_menu(Tracker *t) {
    int idx = choose_subject(t);
    if (idx == -1) {
        return;
    }

    printf("Delete %s and all its attendance?\n", t->list[idx].name);
    if (read_int("1 = yes, 0 = no: ", 0, 1) == 1) {
        remove_subject(t, idx);
        printf("Deleted.\n");
        save_or_warn(t);
    }
}

static void run_menu(Tracker *t) {
    int choice;

    do {
        printf("\n===== ATTENDANCE TRACKER =====\n");
        printf("1. Add subject\n");
        printf("2. Mark a class\n");
        printf("3. View report\n");
        printf("4. Bunk calculator\n");
        printf("5. Set past attendance manually\n");
        printf("6. Change target percentage\n");
        printf("7. Delete subject\n");
        printf("0. Exit\n");

        choice = read_int("Choice: ", 0, 7);

        switch (choice) {
            case 1: add_subject_menu(t); break;
            case 2: mark_menu(t); break;
            case 3: show_report(t); break;
            case 4: bunk_calculator(t); break;
            case 5: set_manually(t); break;
            case 6: change_target(t); break;
            case 7: delete_menu(t); break;
            case 0: printf("Bye, go attend your classes!\n"); break;
        }
    } while (choice != 0);
}

int main(int argc, char *argv[]) {
    Tracker tracker;
    int exit_code = 0;

    init_tracker(&tracker);

    if (load_data(&tracker, DATA_FILE) == -1) {
        // not going to overwrite a file I couldn't read
        printf("Couldn't read %s, it looks corrupted. Fix or delete it and try again.\n", DATA_FILE);
        exit_code = 1;
        goto cleanup;
    }

    if (argc > 1) {
        if (strcmp(argv[1], "report") == 0) {
            show_report(&tracker);
        } else if (strcmp(argv[1], "help") == 0) {
            print_usage(argv[0]);
        } else {
            printf("Unknown command: %s\n\n", argv[1]);
            print_usage(argv[0]);
            exit_code = 1;
        }
    } else {
        run_menu(&tracker);
    }

cleanup:
    free_tracker(&tracker);
    return exit_code;
}
