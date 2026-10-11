#include <stdio.h>
#include <string.h>
#include "storage.h"

// file format:
// line 1 -> target percentage
// next lines -> name,attended,total
int save_data(Tracker *t, const char *path) {
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        return 0;
    }

    fprintf(fp, "%.1f\n", t->target);
    for (int i = 0; i < t->count; i++) {
        fprintf(fp, "%s,%d,%d\n", t->list[i].name, t->list[i].attended, t->list[i].total);
    }

    fclose(fp);
    return 1;
}

int load_data(Tracker *t, const char *path) {
    FILE *fp = fopen(path, "r");
    char line[128];
    char name[NAME_LEN];
    int attended, total;
    int result = 1;

    if (fp == NULL) {
        return 0;   // first run, nothing saved yet
    }

    if (fgets(line, sizeof(line), fp) == NULL || sscanf(line, "%lf", &t->target) != 1) {
        result = -1;
        goto cleanup;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (line[0] == '\n') {
            continue;
        }

        // 49 = NAME_LEN - 1, reads everything till the comma
        if (sscanf(line, "%49[^,],%d,%d", name, &attended, &total) != 3
            || attended < 0 || attended > total) {
            result = -1;
            goto cleanup;
        }

        if (add_subject(t, name) != 1) {
            result = -1;
            goto cleanup;
        }
        t->list[t->count - 1].attended = attended;
        t->list[t->count - 1].total = total;
    }

cleanup:
    fclose(fp);
    return result;
}
