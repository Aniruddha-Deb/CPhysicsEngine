#include <stdio.h>

#include "vector.h"

int main() {
	Vector2 *vec1 = vec2_new(2,3);
	Vector2 *vec2 = vec2_new(4,5);
	Vector2 *vec3 = vec2_new(6,7);

	printf("%s\n", vec2_to_str(vec1));
	vec2_add(vec1, vec2);
	printf("%s\n", vec2_to_str(vec1));
	vec2_subtract(vec1, vec3);
	printf("%s\n", vec2_to_str(vec1));

	vec2_destroy(vec1);
	vec2_destroy(vec2);
	vec2_destroy(vec3);
}
