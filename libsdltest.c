#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
	int x;
	int y;
	int r;
} Circle;

void draw(SDL_Surface *surf, SDL_Renderer *renderer);
void render_circle(SDL_Renderer *renderer, Circle *circ);
void clear_circle(SDL_Renderer *renderer, Circle *circ);

int main() {
	bool quit = false;
	SDL_Event e;
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { 
        printf("error initializing SDL: %s\n", SDL_GetError()); 
    } 
	else {
		const int h = 480, w = 640;
	    SDL_Window* win = SDL_CreateWindow("Bouncing Ball", 
    	                                   SDL_WINDOWPOS_CENTERED, 
        	                               SDL_WINDOWPOS_CENTERED, 
            	                           w, h, 0);
		SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
		draw(SDL_GetWindowSurface(win), renderer);
		SDL_UpdateWindowSurface(win);
		printf("Updated window surface\n");
		SDL_RenderPresent(renderer);

		int r = 30;
		Circle ball = {r+2, r+2, r};
		int xdir = 2;
		int ydir = 2;

		bool startAnim = false;
		while (!quit) {
			while (SDL_PollEvent(&e) != 0 || !startAnim) {
				if (e.type == SDL_QUIT) {
					quit = true;
					break;
				}
				else if (e.type == SDL_KEYDOWN) {
					startAnim = true;
				}
			}
			SDL_Delay(20);
			clear_circle(renderer, &ball);
			ball.x += xdir;
			ball.y += ydir;
			if (ball.x >= w-r-2) xdir *= -1;
			if (ball.x <= r+1) xdir *= -1;
			if (ball.y >= h-r-2) ydir *= -1;
			if (ball.y <= r+1) ydir *= -1;
			render_circle(renderer, &ball);
			SDL_RenderPresent(renderer);
		}
		SDL_Quit();
	}
    return 0; 
}

void draw(SDL_Surface *surf, SDL_Renderer *renderer) {
	SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 0x00, 0x00, 0x00));
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
	SDL_Rect border = {0, 0, 640, 480};
	SDL_RenderDrawRect(renderer, &border);
}

void sym_render_pts(SDL_Renderer *renderer, int x, int y, int dx, int dy) {
	SDL_RenderDrawPoint(renderer, x+dx, y+dy);
	SDL_RenderDrawPoint(renderer, x-dx, y+dy);
	SDL_RenderDrawPoint(renderer, x+dx, y-dy);
	SDL_RenderDrawPoint(renderer, x-dx, y-dy);
	SDL_RenderDrawPoint(renderer, x+dy, y+dx);
	SDL_RenderDrawPoint(renderer, x-dy, y+dx);
	SDL_RenderDrawPoint(renderer, x+dy, y-dx);
	SDL_RenderDrawPoint(renderer, x-dy, y-dx);
}

void render_circle(SDL_Renderer *renderer, Circle *circ) {
	// Bresenham circle algorithm
	int dx=0, dy=circ->r-1;
	while (dy >= dx) {
		sym_render_pts(renderer, circ->x, circ->y, dx, dy);
		dx++;
		dy = sqrt(circ->r*circ->r - dx*dx);
	}
}

void clear_circle(SDL_Renderer *renderer, Circle *circ) {
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
	render_circle(renderer, circ);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
}
