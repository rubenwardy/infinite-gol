/* Test a colony which has been output from a game-of-life program. */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// The number of rows or columns allowed for an example, plus one to allow for
// terminators.
enum { MAX=10 };

// A colony is a 2D array of characters.  Each row is a null-terminated string,
// and the rows are terminated by an empty string.
typedef char colony[MAX][MAX];

// Define the example colonies, each one tick further on than the corresponding
// one in colony.c.
colony colonies[] = {
  {
    ".....",
    "..#..",
    "..#..",
    "..#..",
    ".....",
    ""
  }, {
    ".....",
    ".....",
    ".###.",
    ".....",
    ".....",
    ""
  }, {
    "....",
    ".##.",
    ".##.",
    "....",
    ""
  }, {
    "......",
    "......",
    ".#.#..",
    "..##..",
    "..#...",
    "......",
    ""
  }, {
    "......",
    "......",
    "...#..",
    ".#.#..",
    "..##..",
    "......",
    ""
  }, {
    "......",
    "......",
    "..#...",
    "...##.",
    "..##..",
    "......",
    ""
  }, {
    "......",
    "......",
    "...#..",
    "....#.",
    "..###.",
    "......",
    ""
  }
};

// Find the number of colonies in the above list.
int ncolonies = sizeof(colonies) / sizeof(colony);

void fail() {
    fprintf(stderr, "Use e.g.: ./check 3\n");
    fprintf(stderr, "to check example 3 of a life colony\n");
    exit(1);
}

// Read a line of text from the standard input into the given character array.
// Make sure that any pending text on the standard output has been printed
// first.  Remove the newline (\r or \n or both).
void readline(int n, char line[n]) {
    fflush(stdout);
    fgets(line, n, stdin);
    line[strcspn(line, "\r\n")] = '\0';
}

// Read a colony from the standard input into the given matrix, with the rows
// terminated by an empty string.
void readColony(colony col) {
    for (int i=0; ! feof(stdin); i++) {
        readline(MAX, col[i]);
        if (feof(stdin)) strcpy(col[i], "");
    }
}

// Print out a colony.
void print(colony col) {
    for (int i=0; strlen(col[i]) != 0; i++) {
        printf("%s\n", col[i]);
    } 
}

// Check a colony against the n'th example.
void check(colony actual, int n) {
    bool ok = true, end = false;
    for (int i=0; ! end; i++) {
        if (strcmp(actual[i], colonies[n][i]) != 0) ok = false;
        if (strlen(colonies[n][i]) == 0) end = true;
    }
    if (ok) {
        printf("OK\n");
    }
    else {
        printf("actual colony read in:\n");
        print(actual);
        printf("colony expected:\n");
        print(colonies[n]);
    }
}

// Get the colony number from the command line, then check to see if
// the input colony matches the expected one.
int main(int n, char *args[n]) {
    if (n != 2) fail();
    char *s = args[1];
    int len = strlen(s);
    if (len < 1) fail();
    for (int i=0; i<len; i++) if (! isdigit(s[i])) fail();
    int index = atoi(s);
    if (index < 0 | index >= ncolonies) fail();
    colony col;
    readColony(col);
    check(col, index);
}
