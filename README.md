# Attendance Tracker + Bunk Calculator (C)

A command line program that keeps track of your attendance subject wise and tells you how many classes you can safely skip, or how many you need to attend in a row to get back above the minimum percentage.

Most colleges need around 75% attendance to sit the exams, and I was always doing this calculation on my phone calculator. Got tired of it, so I wrote a program for it in C.

## What it does

- Add subjects and mark each class as present or absent
- Report with percentage, status (SAFE / WARNING / DANGER) and a small progress bar for every subject
- Bunk calculator: how many more classes you can skip and still stay above the target
- "What if" mode: type a number like 3 and it shows what your percentage will become if you skip that many
- Tells you how many classes in a row you need to attend if you are already below the target
- Set past attendance manually, because you will not start using this on day one of the semester
- Change the target percentage (default is 75)
- Everything is saved in a text file automatically, so the data is there next time you open it

## Project structure

```
attendance-tracker-c/
├── src/
│   ├── main.c        menu, user interaction, report printing
│   ├── subject.c     subject logic and the attendance maths
│   ├── storage.c     saving and loading the data file
│   └── utils.c       safe input functions and the progress bar
├── include/
│   ├── subject.h
│   ├── storage.h
│   └── utils.h
├── data/             the save file (attendance.txt) is created here
├── Makefile
└── README.md
```

## How to run

You need gcc. From the project folder:

```
make
./attendance
```

If you don't have `make` (Windows for example):

```
gcc -Wall -std=c99 -Iinclude src/main.c src/subject.c src/storage.c src/utils.c -o attendance -lm
./attendance
```

Run it from the project folder, because the save file path is `data/attendance.txt`.

There are also command line options:

```
./attendance report    prints the report and exits
./attendance help      shows usage
```

## Sample output

```
Target attendance: 75%

No  Subject              Attended   Percent  Status
------------------------------------------------------------
1   Maths                 18/20       90.0%  SAFE
    [##################--]  can skip 4 more
2   Physics               13/20       65.0%  WARNING
    [#############-------]  attend next 8 in a row
------------------------------------------------------------
Overall: 31/40 (77.5%)
```

## The maths behind it

Let `a` be classes attended, `t` total classes and `p` the target as a fraction (0.75 for 75%).

**Classes you can skip (x):** I need `a / (t + x) >= p`, so `x <= a / p - t`. I take the floor of that.

**Classes to attend in a row (y):** skipping nothing and attending `y` more means `(a + y) / (t + y) >= p`. Solving for y gives `y >= (p*t - a) / (1 - p)`. I take the ceiling of that.

Example: 13 out of 20 with a 75% target gives `(0.75*20 - 13) / 0.25 = 8`. After 8 classes it is 21/28 which is exactly 75%.

I added a tiny `1e-9` in these two formulas because floating point sometimes gave results like 2.9999999 and the floor made it 2 instead of 3.


## Things I learned while making it

- `scanf` kept leaving the newline in the buffer and skipping my next input, so I made my own `read_int` and `read_line` using `fgets` and `sscanf`
- Always keep the old pointer when using `realloc`, because if it fails it returns NULL and you lose your array
- The save file is comma separated, so a subject name with a comma would break loading. I block commas when adding a subject
- If the save file is corrupted the program stops instead of overwriting it, so the data is not lost
- Compiled with `-Wall -Wextra` with no warnings, and checked with AddressSanitizer for memory errors

