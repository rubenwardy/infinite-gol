/* Generate a colony as test input to a game-of-life program. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// The number of rows or columns allowed for an example, plus one to allow for
// terminators.
enum { MAX=10 };

// A colony is a 2D array of characters.  Each row is a null-terminated string,
// and the rows are terminated by an empty string.
typedef char colony[MAX][MAX];

// Define the example colonies.
colony colonies[] = {
  {
    ".....",
    ".....",
    ".###.",
    ".....",
    ".....",
    ""
  }, {
    ".....",
    "..#..",
    "..#..",
    "..#..",
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
    "..#...",
    "...#..",
    ".###..",
    "......",
    "......",
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
  }
};

// Find the number of colonies in the above list.
int ncolonies = sizeof(colonies) / sizeof(colony);

void fail() {
    fprintf(stderr, "Use: ./colony i\n");
    fprintf(stderr, "where i is a number from 0 to %d\n", ncolonies - 1);
    exit(1);
}

// Print out the n'th colony.
void print(int n) {
    for (int i=0; strlen(colonies[n][i]) != 0; i++) {
        printf("%s\n", colonies[n][i]);
    } 
}

// Get the colony number from the command line, then call print.
int main(int n, char *args[n]) {
    if (n != 2) fail();
    char *s = args[1];
    int len = strlen(s);
    if (len < 1) fail();
    for (int i=0; i<len; i++) if (! isdigit(s[i])) fail();
    int index = atoi(s);
    if (index < 0 | index >= ncolonies) fail();
    print(index);
}
