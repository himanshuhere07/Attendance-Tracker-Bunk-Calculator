#ifndef STORAGE_H
#define STORAGE_H

#include "subject.h"

// returns 1 if saved, 0 if file couldn't be opened
int save_data(Tracker *t, const char *path);

// returns 1 if loaded, 0 if no file yet (first run), -1 if file is messed up
int load_data(Tracker *t, const char *path);

#endif
