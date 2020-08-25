#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
	float x;
	float y;
} Vector2;

typedef struct {
	float x;
	float y;
	float z;
} Vector3;

Vector2* vec2_new(float x, float y) {
	Vector2 *vec = malloc(sizeof(Vector2));
	vec->x = x;
	vec->y = y;
	return vec;
}

Vector3* vec3_new(float x, float y, float z) {
	Vector3 *vec = malloc(sizeof(Vector3));
	vec->x = x;
	vec->y = y;
	vec->z = z;
	return vec;
}

// TODO varargs here to destory multiple at once.
void vec2_destroy(Vector2 *vec) {
	free(vec);
}

Vector2* vec2_dup(const Vector2 *vec) {
	Vector2 *dup = vec2_new(vec->x, vec->y);
	return dup;
}

void vec2_add(Vector2 *vec1, const Vector2 *vec2) {
	vec1->x += vec2->x;
	vec1->y += vec2->y;
}

void vec2_subtract(Vector2 *vec1, const Vector2 *vec2) {
	vec1->x -= vec2->x;
	vec1->y -= vec2->y;
}

float vec2_magnitude(const Vector2 *vec) {
	float x = vec->x;
	float y = vec->y;
	return sqrt(x*x+y*y);
}

float vec3_magnitude(const Vector3 *vec) {
	float x = vec->x;
	float y = vec->y;
	float z = vec->z;
	return sqrt(x*x + y*y + z*z);
}

float vec2_angle(const Vector2 *vec) {
	return atan2(vec->y, vec->x);
}

float vec2_angle_between(const Vector2 *vec1, const Vector2 *vec2) {
	return vec2_angle(vec1) - vec2_angle(vec2);
}

float vec2_distance_between(const Vector2 *vec1, const Vector2 *vec2) {
	Vector2 *vec1_dup = vec2_dup(vec1);
	vec2_subtract(vec1_dup, vec2);
	float dist = vec2_magnitude(vec1_dup);
	vec2_destroy(vec1_dup);
	return dist;
}

Vector3* vec2_to_vec3(const Vector2 *vec) {
	return vec3_new(vec->x, vec->y, 0);
}

float vec2_cross_magnitude(const Vector2 *vec1, const Vector2 *vec2) {
	return (vec1->x*vec2->y - vec2->x*vec1->y);
}

Vector3* vec2_cross(const Vector2 *vec1, const Vector2 *vec2) {
	Vector3 *vec3 = vec3_new(0, 0, vec2_cross_magnitude(vec1, vec2));
	return vec3;
}

float vec2_dot(const Vector2 *vec1, const Vector2 *vec2) {
	return vec1->x*vec2->x + vec1->y*vec2->y;
}

void vec2_scale(Vector2 *vec, float scalar) {
	vec->x *= scalar;
	vec->y *= scalar;
}

Vector2* vec2_unit_vector(const Vector2 *vec) {
	float m = vec2_magnitude(vec);
	Vector2 *uv = vec2_new(vec->x/m, vec->y/m);
	return uv;
}

char* vec2_to_str(const Vector2 *vec) {
	char* str = malloc(100*sizeof(char));
	snprintf(str, 100, "%.2fi + %.2fj", vec->x, vec->y);
	return str;
}
// TODO vec3 ops
#endif
