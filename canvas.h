#ifndef CANVAS_H_INCLUDED
#define CANVAS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
	ED_T = 0,
	ED_TR,
	ED_R,
	ED_BR,
	ED_B,
	ED_BL,
	ED_L,
	ED_TL
} EDIR;

#define CHUNK_SIZE 8
struct SChunk
{
	int count;
	bool *current;
	bool *next;
	int x;
	int y;

	struct SChunk *down;
	struct SChunk *around[8];
};
typedef struct SChunk Chunk;

struct SCanvas
{
	Chunk *center;

	Chunk **hash_table;
	int hash_table_size;

	bool rule_birth[8];
	bool rule_survive[8];
};
typedef struct SCanvas Canvas;

Chunk *createChunk(int x, int y);
void chunkPrint(Chunk *c, int w, int h);
void chunkLink(EDIR dir1, EDIR dir2, Chunk *one, Chunk *two);

Chunk *canvasGetChunk(Canvas *w, int cx, int cy);
Chunk *canvasCreateChunk(Canvas *w, int cx, int cy);
Canvas *createCanvas();
bool canvasParseRuleset(Canvas *world, const char *ruleset);
void canvasSetCell(Canvas *w, int x, int y, int cell);

void chunkCollectNeighbours(Chunk *c, int neighbours[CHUNK_SIZE][CHUNK_SIZE]);
void canvasStep(Canvas *w);
void canvasGrow(Canvas *world);
void canvasRandomise(Canvas *world);

#endif
