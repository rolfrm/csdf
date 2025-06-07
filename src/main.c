#include "minwindow.h"
#include "main.h"

unsigned int pixels[WIDTH * HEIGHT];

void usleep(int time);
// Draw a 3D cube in isometric projection

vec2 iso_project(vec3 pos){
  return (vec2){
	 .x= (pos.x - pos.z) * 0.866f,
	 .y= -((pos.x + pos.z) * 0.5 - pos.y) 
  };
}

typedef enum{
  SDF_SPHERE = 1,
  SDF_OFFSET = 2
}sdf_type;

f32 vec3_length(vec3 p){
  return sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
}

u8 read_u8(void ** p){
  u8 * pu = *p;
  *p += 1;
  return pu[0];
}

u8 read_f32(void ** p){
  f32 * pu = *p;
  *p += 4;
  return pu[0];
}

//#define vec3.length vec3_length

f32 sdf_distance(u8 * sdf, vec3 p){
  sdf_type t = read_u8(&sdf);
  switch(t){
  case SDF_SPHERE:
	 {
		f32 r = read_f32(&sdf);
		return vec3_length(p) - r;
	 }

  }
  return 0.0f;
}

int main(int argc, char ** argv){
  int test = 0;
  for(int i = 0; i < argc; i++){
	 if(strcmp(argv[i], "--test") == 0){
		test = 1;
	 }
  }

  if(test){

	 
	 return 0;
  }

  camera cam = {0};
  
  MiniWindow * win = mw_create_window(WIDTH,HEIGHT, pixels, "hello");
  int i = 0;
  unsigned int value = 0xFFFF;
  float time = 0.0;
  while(1){
	 i++;
	 time += 0.016;
	 value = value  + 131432312;
	 memset(pixels, 0xFF, sizeof(pixels));
	 draw_screen(&cam, time);
	 //draw_cube(0, i / 10000, 50, 0xFFFFFFFFLU, 0);
	 mw_redraw(win);
	 mw_process_events(win);
	 usleep(16000);
	 
  }
  
  return 0;
}
