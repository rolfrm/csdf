#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define WIDTH 300
#define HEIGHT 300
extern unsigned int pixels[WIDTH * HEIGHT];

typedef unsigned char u8;
typedef unsigned char f32;

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
