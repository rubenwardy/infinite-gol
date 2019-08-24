#include "canvas.h"
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

Chunk *createChunk(int x, int y)
{
	Chunk *c = malloc(sizeof(Chunk));
	c->x = x;
	c->y = y;

	for (int i = 0; i < 8; ++i)
		c->around[i] = NULL;

	c->count = 0;
	c->current = malloc(sizeof(bool) * CHUNK_SIZE * CHUNK_SIZE);
	c->next    = malloc(sizeof(bool) * CHUNK_SIZE * CHUNK_SIZE);
	c->down    = NULL;
	for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; ++i) {
		c->current[i] = false;
		c->next[i] = false;
	}

	return c;
}

void chunkPrint(Chunk *c, int w, int h)
{
	if (w <= 0 || w > CHUNK_SIZE)
		w = CHUNK_SIZE;
	if (h <= 0 || h > CHUNK_SIZE)
		h = CHUNK_SIZE;

	for (int y = 0; y < w; y++) {
		for (int x = 0; x < w; x++) {
			if (c->current[x + y * CHUNK_SIZE])
				printf("#");
			else
				printf(".");
		}
		printf("\n");
	}
}

#define HASH(W, X, Y) ((X) + 100*(Y)) % (W)->hash_table_size

Chunk *canvasGetChunk(Canvas *w, int cx, int cy)
{
	int hash = HASH(w, cx, cy);
	if (hash < 0)
		hash += w->hash_table_size;

	Chunk *cursor = w->hash_table[hash];
	while (cursor) {
		if (cursor->x == cx && cursor->y == cy)
			return cursor;
		else
			cursor = cursor->down;
	}
	return NULL;
}


void chunkLink(EDIR dir1, EDIR dir2, Chunk *one, Chunk *two)
{
	if (two) {
		one->around[dir1] = two;
		two->around[dir2] = one;
	} //else {
		//fprintf(stderr, "No link possible for (%d, %d)\n", one->x, one->y);
	//}
}

Chunk *canvasCreateChunk(Canvas *w, int cx, int cy)
{
	Chunk *c = createChunk(cx, cy);

	int hash = HASH(w, cx, cy);
	if (hash < 0)
		hash += w->hash_table_size;

	if (w->hash_table[hash]) {
		Chunk *cursor = w->hash_table[hash];
		while (cursor->down)
			cursor = cursor->down;
		cursor->down = c;
	} else
		w->hash_table[hash] = c;

	chunkLink(ED_T,  ED_B,  c, canvasGetChunk(w, cx,     cy - 1));
	chunkLink(ED_TR, ED_BL, c, canvasGetChunk(w, cx + 1, cy - 1));
	chunkLink(ED_R,  ED_L,  c, canvasGetChunk(w, cx + 1, cy));
	chunkLink(ED_BR, ED_TL, c, canvasGetChunk(w, cx + 1, cy + 1));
	chunkLink(ED_B,  ED_T,  c, canvasGetChunk(w, cx,     cy + 1));
	chunkLink(ED_BL, ED_TR, c, canvasGetChunk(w, cx - 1, cy + 1));
	chunkLink(ED_L,  ED_R,  c, canvasGetChunk(w, cx - 1, cy));
	chunkLink(ED_TL, ED_BR, c, canvasGetChunk(w, cx - 1, cy - 1));

	return c;
}

Canvas *createCanvas()
{
	Canvas *world = malloc(sizeof(Canvas));
	world->hash_table_size = 1000;
	world->hash_table = malloc(sizeof(Chunk*) * world->hash_table_size);
	for (int i = 0; i < world->hash_table_size; ++i)
		world->hash_table[i] = NULL;

	world->center = canvasCreateChunk(world, 0, 0);

	for (int i = 0; i < 8; ++i) {
		world->rule_birth[i]   = false;
		world->rule_survive[i] = false;
	}
	// Ruleset will be set by calling canvasParseRuleset().

	return world;
}

typedef enum {
	EPS_None = 0,
	EPS_B,
	EPS_S
} EParseState;

bool canvasParseRuleset(Canvas *world, const char *ruleset)
{
	fprintf(stderr, "Ruleset: '%s'.\n", ruleset);
	for (int i = 0; i < 8; ++i) {
		world->rule_birth[i]   = false;
		world->rule_survive[i] = false;
	}

	EParseState state = EPS_None;
	for (int i = 0; i < strlen(ruleset); ++i) {
		char ch = ruleset[i];
		if (ch == 'B')
			state = EPS_B;
		else if (ch == 'S')
			state = EPS_S;
		else if (isdigit(ch)) {
			if (state == EPS_B)
				world->rule_birth[ch - '0'] = true;
			else if (state == EPS_S)
				world->rule_survive[ch - '0'] = true;
			else {
				fprintf(stderr, "Ruleset parse error: expecting B or S first, "
					"before any numbers!\n");
				return false;
			}
		} else
			state = EPS_None;
	}
	return true;
}

void canvasSetCell(Canvas *w, int x, int y, int cell)
{
	int cx = floor((float)x / (float)CHUNK_SIZE);
	int cy = floor((float)y / (float)CHUNK_SIZE);

	Chunk *c = canvasGetChunk(w, cx, cy);
	if (!c)
		c = canvasCreateChunk(w, cx, cy);

	int inner_x = x % CHUNK_SIZE;
	int inner_y = y % CHUNK_SIZE;
	if (inner_x < 0)
		inner_x = inner_x + CHUNK_SIZE;
	if (inner_y < 0)
		inner_y = inner_y + CHUNK_SIZE;

	bool b4 = c->current[inner_x + inner_y * CHUNK_SIZE];
	if (cell == 2)
		cell = !b4;

	if (cell == 1 && !b4)
		c->count++;
	else if (cell == 0 && b4)
		c->count--;

	c->current[inner_x + inner_y * CHUNK_SIZE] = cell;
}

void chunkCollectNeighbours(Chunk *c, int neighbours[CHUNK_SIZE][CHUNK_SIZE])
{
	for (int inner_y = 0; inner_y < CHUNK_SIZE; ++inner_y)
	for (int inner_x = 0; inner_x < CHUNK_SIZE; ++inner_x) {
		neighbours[inner_y][inner_x] = 0;
	}

	for (int inner_y = 0; inner_y < CHUNK_SIZE; ++inner_y)
	for (int inner_x = 0; inner_x < CHUNK_SIZE; ++inner_x) {
		if (c->current[inner_x + inner_y * CHUNK_SIZE]) {
			bool has_above = (inner_y > 0);
			bool has_left  = (inner_x > 0);
			bool has_below = (inner_y < CHUNK_SIZE - 1);
			bool has_right = (inner_x < CHUNK_SIZE - 1);

			// Left column
			if (has_left) {
				if (has_above)
					neighbours[inner_y - 1][inner_x - 1] += 1;

				neighbours[inner_y][inner_x - 1] += 1;

				if (has_below)
					neighbours[inner_y + 1][inner_x - 1] += 1;
			}

			// Middle column
			{
				if (has_above)
					neighbours[inner_y - 1][inner_x] += 1;

				if (has_below)
					neighbours[inner_y + 1][inner_x] += 1;
			}

			// Right column
			if (has_right) {
				if (has_above)
					neighbours[inner_y - 1][inner_x + 1] += 1;

				neighbours[inner_y][inner_x + 1] += 1;

				if (has_below)
					neighbours[inner_y + 1][inner_x + 1] += 1;
			}
		}
	}

	// Horizontal Scans
	Chunk *other = NULL;
	other = c->around[ED_T];
	if (other) {
		for (int i = 0; i < CHUNK_SIZE; ++i) {
			int x = i;
			int y = CHUNK_SIZE - 1;
			if (other->current[x + y * CHUNK_SIZE]) {
				if (i > 0)
					neighbours[0][i - 1] += 1;

				neighbours[0][i] += 1;

				if (i < CHUNK_SIZE - 1)
					neighbours[0][i + 1] += 1;
			}
		}
	}
	other = c->around[ED_B];
	if (other) {
		for (int i = 0; i < CHUNK_SIZE; ++i) {
			if (other->current[i]) {

				if (i > 0)
					neighbours[CHUNK_SIZE - 1][i - 1] += 1;

				neighbours[CHUNK_SIZE - 1][i] += 1;

				if (i < CHUNK_SIZE - 1)
					neighbours[CHUNK_SIZE - 1][i + 1] += 1;
			}
		}
	}


	// Vertical Scans
	other = c->around[ED_R];
	if (other) {
		for (int i = 0; i < CHUNK_SIZE; ++i) {
			if (other->current[i * CHUNK_SIZE]) {
				if (i > 0)
					neighbours[i - 1][CHUNK_SIZE - 1] += 1;

				neighbours[i][CHUNK_SIZE - 1] += 1;

				if (i < CHUNK_SIZE - 1)
					neighbours[i + 1][CHUNK_SIZE - 1] += 1;
			}
		}
	}
	other = c->around[ED_L];
	if (other) {
		for (int i = 0; i < CHUNK_SIZE; ++i) {
			if (other->current[CHUNK_SIZE - 1 + i * CHUNK_SIZE]) {
				if (i > 0)
					neighbours[i - 1][0] += 1;

				neighbours[i][0] += 1;

				if (i < CHUNK_SIZE - 1)
					neighbours[i + 1][0] += 1;
			}
		}
	}

	// Diagonals
	other = c->around[ED_TL];
	if (other && other->current[CHUNK_SIZE - 1 + (CHUNK_SIZE - 1) * CHUNK_SIZE])
		neighbours[0][0] += 1;

	other = c->around[ED_BL];
	if (other && other->current[CHUNK_SIZE - 1])
		neighbours[CHUNK_SIZE - 1][0] += 1;

	other = c->around[ED_TR];
	if (other && other->current[(CHUNK_SIZE - 1) * CHUNK_SIZE])
		neighbours[0][CHUNK_SIZE - 1] += 1;

	other = c->around[ED_BR];
	if (other && other->current[0])
		neighbours[CHUNK_SIZE - 1][CHUNK_SIZE - 1] += 1;
}

void chunkStep(Canvas *world, Chunk *c)
{
	// As an optimisation, collect neighbours by only looping through cells once
	//fprintf(stderr, " - Collecting neighbours.\n");
	int neighbours[CHUNK_SIZE][CHUNK_SIZE];
	chunkCollectNeighbours(c, neighbours);

	// Print neighbours
	/*fprintf(stderr, "   - Calculating neighbours:\n");
	for (int inner_y = 0; inner_y < CHUNK_SIZE; ++inner_y) {
		fprintf(stderr, "      ");
		for (int inner_x = 0; inner_x < CHUNK_SIZE; ++inner_x) {
			fprintf(stderr, "%d ", neighbours[inner_y][inner_x]);
		}
		fprintf(stderr, "\n");
	}*/

	// Now loop through all cells and update their state
	for (int inner_y = 0; inner_y < CHUNK_SIZE; ++inner_y)
	for (int inner_x = 0; inner_x < CHUNK_SIZE; ++inner_x) {
		bool alive = c->current[inner_x + inner_y * CHUNK_SIZE];
		int cneigh = neighbours[inner_y][inner_x];
		if (alive && !world->rule_survive[cneigh]) {
			c->next[inner_x + inner_y * CHUNK_SIZE] = false;
			c->count--;
		} else if (!alive && world->rule_birth[cneigh]) {
			c->next[inner_x + inner_y * CHUNK_SIZE] = true;
			c->count++;
		} else {
			c->next[inner_x + inner_y * CHUNK_SIZE] = alive;
		}
	}
}

void canvasStep(Canvas *w)
{
	clock_t start = clock();

	for (int i = 0; i < w->hash_table_size; ++i) {
		Chunk *c = w->hash_table[i];
		while (c) {
			chunkStep(w, c);
			c = c->down;
		}
	}

	int count = 0;
	for (int i = 0; i < w->hash_table_size; ++i) {
		Chunk *c = w->hash_table[i];
		while (c) {
			count++;
			bool *tmp = c->current;
			c->current = c->next;
			c->next = tmp;
			c = c->down;
		}
	}


	clock_t end = clock();

	fprintf(stderr, "Stepped %d cells (%d chunks). Took %d ms.\n",
		count * CHUNK_SIZE * CHUNK_SIZE, count,
		(int)floor(1000.0f * ((double) (end - start)) / CLOCKS_PER_SEC));
}

void canvasGrow(Canvas *world)
{
	for (int i = 0; i < world->hash_table_size; ++i) {
		Chunk *c = world->hash_table[i];
		while (c) {
			if (c->count > 0) {
				if (!c->around[ED_T])
					canvasCreateChunk(world, c->x, c->y - 1);
				if (!c->around[ED_B])
					canvasCreateChunk(world, c->x, c->y + 1);

				if (!c->around[ED_L])
					canvasCreateChunk(world, c->x - 1, c->y);
				if (!c->around[ED_R])
					canvasCreateChunk(world, c->x + 1, c->y);

				if (!c->around[ED_TL])
					canvasCreateChunk(world, c->x - 1, c->y - 1);
				if (!c->around[ED_TR])
					canvasCreateChunk(world, c->x + 1, c->y - 1);

				if (!c->around[ED_BL])
					canvasCreateChunk(world, c->x - 1, c->y + 1);
				if (!c->around[ED_BR])
					canvasCreateChunk(world, c->x + 1, c->y + 1);
			}

			c = c->down;
		}
	}
}

void canvasRandomise(Canvas *world)
{
	for (int i = 0; i < world->hash_table_size; ++i) {
		Chunk *c = world->hash_table[i];
		while (c) {
			c->count = 0;
			for (int y = 0; y < CHUNK_SIZE; ++y)
			for (int x = 0; x < CHUNK_SIZE; ++x) {
				if (rand() % 2) {
					c->count++;
					c->current[x + y * CHUNK_SIZE] = true;
				} else{
					c->current[x + y * CHUNK_SIZE] = false;
				}
			}
			c = c->down;
		}
	}
}
