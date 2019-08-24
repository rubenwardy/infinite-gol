#include "canvas.h"
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"

#define MAX_STDIN_SIZE 1024
int readFromStdin(Canvas *world, int *w, int *h)
{
	int y = 0;
	char line[MAX_STDIN_SIZE];
	fprintf(stderr, "Waiting for world input.\n   Ctrl+D to stop. '.' is dead, '#' is alive\n\n");
	while (!feof(stdin)) {
		while (fgets(line, MAX_STDIN_SIZE, stdin) == NULL && !feof(stdin));
		if (feof(stdin))
			break;

		// Process Input
		for (int x = 0; x < MAX_STDIN_SIZE; x++) {
			char ch = line[x];
			if (ch == '\0')
				break;
			else if (ch == '#')
				canvasSetCell(world, x, y, true);
			else if (ch == '.')
				canvasSetCell(world, x, y, false);
			else if (ch == '\n') // row number is here in case of long lines in files
				y++;
			else {
				fprintf(stderr, "Syntax error on line %d character %d ('%c')\n", x, y, ch);
				return 1;
			}

			if (x > (*w))
				(*w) = x;
		}
	}
	(*h) = y;

	return 0;
}

#define TEST(cond) if (!(cond)) { printf("Test failed: " #cond "\n"); return false;}

bool run_tests()
{
	Canvas *world = createCanvas();

	canvasSetCell(world, -1, -1, true);
	canvasSetCell(world, -1, 0, true);
	canvasSetCell(world, 4, -1, true);
	canvasSetCell(world, 0, -1, true);
	canvasSetCell(world, 0, 0, true);

	Chunk *cursor = world->center;
	int neighbours[CHUNK_SIZE][CHUNK_SIZE];

	TEST(cursor);
	TEST(cursor->x == 0);
	TEST(cursor->y == 0);
	TEST(cursor->around[ED_T]);
	TEST(cursor->around[ED_T]->current[0 + (CHUNK_SIZE - 1) * CHUNK_SIZE]);
	TEST(cursor->around[ED_L]);
	TEST(cursor->around[ED_TL]);
	TEST(!cursor->around[ED_TR]);
	TEST(!cursor->around[ED_R]);
	TEST(!cursor->around[ED_BR]);
	TEST(!cursor->around[ED_B]);
	TEST(!cursor->around[ED_BL]);
	{
		chunkCollectNeighbours(cursor, neighbours);
		int expected[CHUNK_SIZE][CHUNK_SIZE] = {
			{3, 2, 0, 1, 1, 1, 0, 0},
			{2, 1, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0}
		};
		for (int x = 0; x < CHUNK_SIZE; ++x)
		for (int y = 0; y < CHUNK_SIZE; ++y) {
			if (neighbours[y][x] != expected[y][x])
				fprintf(stderr, "Check (%d, %d), got %d expected %d\n",
					x, y, neighbours[y][x], expected[y][x]);

			TEST(neighbours[y][x] == expected[y][x]);
		}
	}

	cursor = cursor->around[ED_T];
	TEST(cursor);
	TEST(cursor->around[ED_B]);
	TEST(cursor->around[ED_L]);
	TEST(cursor->around[ED_BL]);
	TEST(!cursor->around[ED_TL]);
	TEST(!cursor->around[ED_T]);
	TEST(!cursor->around[ED_TR]);
	TEST(!cursor->around[ED_R]);
	TEST(!cursor->around[ED_BR]);
	{
		chunkCollectNeighbours(cursor, neighbours);
		int expected[CHUNK_SIZE][CHUNK_SIZE] = {
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{2, 1, 0, 1, 1, 1, 0, 0},
			{3, 2, 0, 1, 0, 1, 0, 0}
		};
		for (int x = 0; x < CHUNK_SIZE; ++x)
		for (int y = 0; y < CHUNK_SIZE; ++y) {
			if (neighbours[y][x] != expected[y][x])
				fprintf(stderr, "Check (%d, %d), got %d expected %d\n",
					x, y, neighbours[y][x], expected[y][x]);

			TEST(neighbours[y][x] == expected[y][x]);
		}
	}

	cursor = cursor->around[ED_L];
	TEST(cursor);
	TEST(cursor->around[ED_B]);
	TEST(!cursor->around[ED_L]);
	TEST(!cursor->around[ED_BL]);
	TEST(!cursor->around[ED_TL]);
	TEST(!cursor->around[ED_T]);
	TEST(!cursor->around[ED_TR]);
	TEST(cursor->around[ED_R]);
	TEST(cursor->around[ED_BR]);
	{
		chunkCollectNeighbours(cursor, neighbours);
		int expected[CHUNK_SIZE][CHUNK_SIZE] = {
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 1, 2},
			{0, 0, 0, 0, 0, 0, 2, 3}
		};
		for (int x = 0; x < CHUNK_SIZE; ++x)
		for (int y = 0; y < CHUNK_SIZE; ++y) {
			if (neighbours[y][x] != expected[y][x])
				fprintf(stderr, "Check (%d, %d), got %d expected %d\n",
					x, y, neighbours[y][x], expected[y][x]);

			TEST(neighbours[y][x] == expected[y][x]);
		}
	}

	cursor = cursor->around[ED_B];
	TEST(cursor);
	TEST(!cursor->around[ED_B]);
	TEST(!cursor->around[ED_L]);
	TEST(!cursor->around[ED_BL]);
	TEST(!cursor->around[ED_TL]);
	TEST(cursor->around[ED_T]);
	TEST(cursor->around[ED_TR]);
	TEST(cursor->around[ED_R]);
	TEST(!cursor->around[ED_BR]);
	{
		chunkCollectNeighbours(cursor, neighbours);
		int expected[CHUNK_SIZE][CHUNK_SIZE] = {
			{0, 0, 0, 0, 0, 0, 2, 3},
			{0, 0, 0, 0, 0, 0, 1, 2},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0}
		};
		for (int x = 0; x < CHUNK_SIZE; ++x)
		for (int y = 0; y < CHUNK_SIZE; ++y) {
			if (neighbours[y][x] != expected[y][x])
				fprintf(stderr, "Check (%d, %d), got %d expected %d\n",
					x, y, neighbours[y][x], expected[y][x]);

			TEST(neighbours[y][x] == expected[y][x]);
		}
	}

	fprintf(stderr, "All auto tests passed!\n");
	return true;
}

void usage()
{
	fprintf(stderr,	"USAGE:\n"
		"  ./life\n"
		"  ./life --player\n"
		"  ./life --player --no-stdin\n"
		"  ./life --tests\n"); // , ./life n, where n is the number of steps, defaults to 1.
}

typedef enum {
	EM_Auto = 0,
	EM_Stdout,
	EM_Player
} OutputMode;

int main(int n, char *args[n])
{
	// Parse command line input
	int steps = -1;
	char *ruleset = "B3/S23";
	OutputMode mode_output = EM_Auto;
	bool mode_from_stdin = true;
	bool mode_enable_tests = false;
	for (int i = 1; i < n; ++i) {
		char *arg = args[i];
		if (strlen(arg) >= 2 && strncmp("--", arg, 2) == 0) {
			arg += 2;
			if (strcmp(arg, "tests") == 0) {
				mode_enable_tests = true;
			} else if (strcmp(arg, "player") == 0) {
				mode_output = EM_Player;
			} else if (strcmp(arg, "stdout") == 0) {
				mode_output = EM_Stdout;
			} else if (strcmp(arg, "no-stdin") == 0) {
				mode_from_stdin = false;
			} else if (strcmp(arg, "stdin") == 0) {
				mode_from_stdin = true;
			} else {
				fprintf(stderr, "Invalid switch: %s\n", arg);
				usage();
				return 1;
			}
		} else {
			fprintf(stderr, "Mode: %s\n", arg);
			char *ptr = NULL;
			long a = strtol(arg, &ptr, 10);
			if (ptr == arg + strlen(arg) && a >= 0) {
				fprintf(stderr, "Set steps from command line: %ld.\n", a);
				steps = a;
			} else if (toupper(arg[0]) == 'B') {
				fprintf(stderr, "Set ruleset from command line: '%s'.\n", arg);
				ruleset = arg;
			} else {
				fprintf(stderr, "Not a valid number or ruleset: '%s'.\n", arg);
				usage();
				return 1;
			}
		}
	}


	srand(time(NULL));


	// Do tests?
	if (mode_enable_tests) {
		if (!run_tests())
			return 1;
	}

	if (mode_output != EM_Auto || !mode_enable_tests) {
		Canvas *world = createCanvas();

		if (!canvasParseRuleset(world, ruleset))
			return 1;

		// Read from stdin
		int w = 0;
		int h = 0;
		if (mode_from_stdin) {
			int ret = readFromStdin(world, &w, &h);
			if (ret > 0)
				return ret;
		}

		if (mode_output == EM_Player) {
			if (steps < 0)
				steps = 0;

			for (int i = 0; i < steps; ++i) {
				canvasStep(world);
				canvasGrow(world);
			}
			return runPlayer(world);
		} else {
			if (steps < 0)
				steps = 1;

			for (int i = 0; i < steps; ++i) {
				canvasStep(world);
				canvasGrow(world);
			}
			chunkPrint(world->center, w, h);
			return 0;
		}
	}
	return 0;
}
