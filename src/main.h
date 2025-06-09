#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>


#define WIDTH 300
#define HEIGHT 300
extern unsigned int pixels[WIDTH * HEIGHT];

typedef size_t u64;
typedef unsigned char u8;
typedef signed char i8;
typedef unsigned int u32;
typedef float f32;

typedef struct{
  float x, y;
}vec2;

typedef struct {
  float x, y, z;
}vec3;

vec2 iso_project(vec3 pos);
void draw_pixel(int x, int y, unsigned int value);
void draw_line (int x1, int x2, int y, unsigned int value);

typedef struct{
  int x, y, z;
}camera;
void draw_screen(camera * cam, float time);


// testing
extern int test_level;
#define TEST(name) { \
	 for(int i = 0; i < test_level; ++i) printf("  "); \
    printf("Testing %s...\n", #name);						\
    fflush(stdout); \
	 test_level++;	  \
    name(); \
	 test_level--;														\
		for(int i = 0; i < test_level; ++i) printf("  ");	\
    printf("✔ %s\n", #name);									\
}
