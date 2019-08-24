#include "player.h"
#include "math.h"
#include <SDL2/SDL.h>

struct SPlayerState
{
	bool paused;
	Canvas *world;
	int x;
	int y;

	int last_mouse_x;
	int last_mouse_y;

	SDL_Window *window;
	SDL_Surface *screenSurface;
	SDL_Renderer *renderer;
};
typedef struct SPlayerState PlayerState;

const int PIXEL_SIZE = 10;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool initSDL(PlayerState *state)
{
	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// Create window
	state->window = SDL_CreateWindow("Game of Life",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
		SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!state->window) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// Create surface
	state->screenSurface = SDL_GetWindowSurface(state->window);
	if (!state->screenSurface) {
		printf("Surface could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// Create Renderer
	state->renderer = SDL_CreateRenderer(state->window, 0, SDL_RENDERER_ACCELERATED);
	if (!state->renderer) {
		printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

bool acceptInput(PlayerState *state)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			return false;
		} else if (e.type == SDL_KEYUP) {
			switch (e.key.keysym.sym) {
			case SDLK_SPACE:
				state->paused = !state->paused;
				break;
			case SDLK_s:
				canvasStep(state->world);
				canvasGrow(state->world);
				break;
			case SDLK_r:
				canvasRandomise(state->world);
				break;
			}
		}

		if (e.type == SDL_MOUSEBUTTONDOWN) {
			if (e.button.button != SDL_BUTTON_LEFT) {
				int x, y;
				SDL_GetMouseState(&x, &y);
				state->last_mouse_x = x;
				state->last_mouse_y = y;
			}
		} else if (e.type == SDL_MOUSEBUTTONUP) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				// Do set
				int x, y;
				SDL_GetMouseState(&x, &y);
				x = floor((float)(x + state->x) / (float)PIXEL_SIZE);
				y = floor((float)(y + state->y) / (float)PIXEL_SIZE);
				fprintf(stderr, "User toggled %d, %d\n", x, y);

				canvasSetCell(state->world, x, y, 2);
			} else {
				state->last_mouse_x = -1;
				state->last_mouse_y = -1;
			}
		} else if (e.type == SDL_MOUSEMOTION &&
				state->last_mouse_x > -1 && state->last_mouse_y > -1) {
			int x, y;
			SDL_GetMouseState(&x, &y);

			int dx = x - state->last_mouse_x;
			int dy = y - state->last_mouse_y;
			state->last_mouse_x = x;
			state->last_mouse_y = y;
			state->x -= dx;
			state->y -= dy;
		}
	}
	return true;
}

void renderChunk(PlayerState *state, Chunk *c)
{
	int px = c->x * PIXEL_SIZE * CHUNK_SIZE;
	int py = c->y * PIXEL_SIZE * CHUNK_SIZE;
	if (px > state->x + 700 || px < state->x - 700 ||
			py > state->y + 550 || py < state->y - 550)
		return;

	for (int x = 0; x < CHUNK_SIZE; x++)
	for (int y = 0; y < CHUNK_SIZE; y++) {
		if (c->current[x + y *CHUNK_SIZE]) {
			SDL_SetRenderDrawColor(state->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		} else {
			SDL_SetRenderDrawColor(state->renderer, 0x22, 0x22, 0x22, 0xFF);
		}


		SDL_Rect fillRect = {
			(c->x * CHUNK_SIZE + x) * PIXEL_SIZE - state->x + 1,
			(c->y * CHUNK_SIZE + y) * PIXEL_SIZE - state->y + 1,
			PIXEL_SIZE - 2,
			PIXEL_SIZE - 2};
		SDL_RenderFillRect(state->renderer, &fillRect);
	}
}

void render(PlayerState *state)
{
	SDL_SetRenderDrawColor(state->renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(state->renderer);

	SDL_SetRenderDrawColor(state->renderer, 0xFF, 0xFF, 0xFF, 0xFF);


	for (int i = 0; i < state->world->hash_table_size; ++i) {
		Chunk *c = state->world->hash_table[i];
		while (c) {
			renderChunk(state, c);
			c = c->down;
		}
	}

	SDL_RenderPresent(state->renderer);
}

int runPlayer(Canvas *world)
{
	fprintf(stderr, "Running graphical player!\n");

	PlayerState state;
	state.paused = false;
	state.world = world;
	state.x = 0;
	state.y = 0;
	state.last_mouse_x = -1;
	state.last_mouse_y = -1;

	if (!initSDL(&state))
		return 1;

	bool running = true;
	int timer = SDL_GetTicks();
	float count = 0;
	while (running) {
		int now = SDL_GetTicks();
		float dtime = (float)(now - timer) / 1000.0f;
		timer = now;

		if (state.paused)
			count = 0;
		else {
			count += dtime;
			if (count > 0.1) {
				count = 0;
				canvasStep(world);
				canvasGrow(world);
			}
		}

		running = acceptInput(&state);
		render(&state);
	}

	return 0;
}
