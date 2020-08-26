#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "vector.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_TITLE "Ballpit"

// a low coefficient of restitution causes a lot of bugs: the engine is 
// not meant to handle deformations etc. The balls 'slide' into one another
// with a very low e
//
// one more thing is that the ticrate needs to be quite high, otherwise 
// bugs galore. This is not good as I am using muscle power to compensate for
// a poor algorithm. I'll need to think of a better collision algorithm, but
// that is not a high priority now. 
// 
// One solution is to run as fast as the system will allow, using a variable
// tic rate and render the circles independently (only when their position
// changes) rather than rendering them all at once.
// This needs more thought. A lot more thought.

#define E 1 // Coefficient of Restitution

typedef struct {
	Vector2 *pos;
	Vector2 *v;
	float r;
	float m;
} Ball;

Ball* new_ball(float x, float y, float vx, float vy, float r, float m) {
	Vector2 *pos = vec2_new(x, y);
	Vector2 *v = vec2_new(vx, vy);
	Ball *b = malloc(sizeof(Ball));
	b->pos = pos;
	b->v = v;
	b->r = r;
	b->m = m;
	return b;
}

void destroy_ball(Ball *b) {
	vec2_destroy(b->pos);
	vec2_destroy(b->v);
	free(b);
}

void recompute_post_collision(Ball *b1, Ball *b2) {
	// first, find the direction of common normal: from b1_pos to b2_pos
	Vector2 *b2pos = vec2_dup(b2->pos);
	vec2_subtract(b2pos, b1->pos);
	Vector2 *cn = vec2_unit_vector(b2pos);
	// then calculate component of velocity along CN
	float ucn1 = vec2_dot(cn, b1->v);
	float ucn2 = vec2_dot(cn, b2->v);
	// then find final velocities along CN using coefft. of resititution formula
	float m1 = b1->m;
	float m2 = b2->m;
	float vcn1 = ((m1-E*m2)/(m1+m2))*ucn1 + ((1+E)*m2/(m1+m2))*ucn2;
	float vcn2 = ((1+E)*m1/(m1+m2))*ucn1 + ((m2-E*m1)/(m1+m2))*ucn2;
	// change in velocity along CN for balls 1 and 2
	float dv1 = vcn1-ucn1;
	float dv2 = vcn2-ucn2;
	// express dv1 and dv2 as vectors and add to v
	vec2_scale(cn, dv1);
	vec2_add(b1->v, cn);
	vec2_destroy(cn);
	cn = vec2_unit_vector(b2pos);
	vec2_scale(cn, dv2);
	vec2_add(b2->v, cn);
	// finally, clean up and destory b2pos and cn
	vec2_destroy(b2pos);
	vec2_destroy(cn);
}

void handle_collisions(const int nb, Ball **b) {
	// first handle ball-ball collisions
	for (int i=0; i<nb; i++) {
		for (int j=i+1; j<nb; j++) {
			// check if b_i, b_j are in contact
			if (vec2_distance_between(b[i]->pos, b[j]->pos) <= b[i]->r+b[j]->r ) {
				// collision has occured; compute new velocities along common
				// normal of collision (mu=0, so no impulse along common tangent
				recompute_post_collision(b[i], b[j]);
			}
		}
	}
	// then handle ball-wall collisions
	// TODO incorporate E in ball-wall collisions as well.
	for (int i=0; i<nb; i++) {
		if (b[i]->pos->x <= b[i]->r || b[i]->pos->x >= SCREEN_WIDTH-b[i]->r) {
			b[i]->v->x *= -1; // reverse velocity along x-axis
		}
		if (b[i]->pos->y <= b[i]->r || b[i]->pos->y >= SCREEN_HEIGHT-b[i]->r) {
			b[i]->v->y *= -1;
		}
	}
}

void update_positions(const int nb, Ball **b, const Vector2 *g, const float tic_ms) {
	for (int i=0; i<nb; i++) {
		// add effects of gravity to velocity
		Vector2 *dv = vec2_dup(g);
		vec2_scale(dv, tic_ms*0.001);
		vec2_add(b[i]->v, dv);
		// add velocity*tic_ms*0.001 + g_dir*tic_ms*10^-6*0.5 to position
		Vector2 *dx = vec2_dup(b[i]->v);
		vec2_scale(dx, tic_ms*0.001);
		vec2_scale(dv, tic_ms*0.001*0.5);
		vec2_add(b[i]->pos, dx);
		vec2_add(b[i]->pos, dv);
		vec2_destroy(dx);
		vec2_destroy(dv);
	}
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

int iroundf(float f) {
	// can be optimized as return (int)(f+0.5)
	int i = (int)f; // floor(f)
	if (f-i >= 0.5f) return i+1;
	else return i;
}

void draw_ball(SDL_Renderer *renderer, Ball *b) {
	int x0 = iroundf(b->pos->x);
	int y0 = iroundf(b->pos->y);
	float r = b->r;

	float x=0, y=r;
	sym_render_pts(renderer, x0, y0, x, iroundf(y));
	while (x <= y) {
		x++;
		y = sqrt(r*r-x*x);
		sym_render_pts(renderer, x0, y0, x, iroundf(y));
	}
}

void draw_balls(SDL_Renderer *renderer, int nballs, Ball **balls) {
	for (int i=0; i<nballs; i++) {
		draw_ball(renderer, balls[i]);
	}
}

void clear_balls(SDL_Renderer *renderer, int nballs, Ball **balls) {
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
	draw_balls(renderer, nballs, balls);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
}

void init_graphics(SDL_Window *w, SDL_Renderer *renderer, int nb, Ball **b) {
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
	SDL_Rect border = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_RenderFillRect(renderer, &border);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(renderer, &border);
	draw_balls(renderer, nb, b);
	SDL_RenderPresent(renderer);
}

int main_loop() {
	Ball *b1 = new_ball(30, 30, 200, 0, 20, 15);
	Ball *b2 = new_ball(80, 50, -50, -200, 10, 5);
	Ball *b3 = new_ball(480, 200, 60, -150, 40, 30);
	Ball *b4 = new_ball(250, 300, 0, 0, 30, 20);
	Ball *balls[] = {b1, b2, b3, b4};
	int nballs = 4;
	Vector2 *g = vec2_new(0, 200);

	SDL_Event e;
	bool quit = false;
	bool startAnim = false;
	const int ticrate = 1;

	SDL_Window *w = SDL_CreateWindow(SCREEN_TITLE, 
									 SDL_WINDOWPOS_CENTERED, 
									 SDL_WINDOWPOS_CENTERED, 
									 SCREEN_WIDTH, 
									 SCREEN_HEIGHT, 
									 SDL_WINDOW_OPENGL);
	if (w == NULL) {
		printf("Could not create window: %s\n", SDL_GetError());
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
	init_graphics(w, renderer, nballs, balls);

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
		SDL_Delay(ticrate);
		clear_balls(renderer, nballs, balls);
		handle_collisions(nballs, balls);
		update_positions(nballs, balls, g, ticrate);
		draw_balls(renderer, nballs, balls);
		SDL_RenderPresent(renderer);
	}
	
	for (int i=0; i<nballs; i++) {
		destroy_ball(balls[i]);
	}

	SDL_DestroyWindow(w);
	SDL_Quit();
	return 0;
}

int main() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("Error initializing SDL: %s\n", SDL_GetError());
	}
	else {
		main_loop();
	}
	return 0;
}
