#include "minwindow.h"
#include "main.h"

unsigned int pixels[WIDTH * HEIGHT];
int test_level;
void usleep(int time);
// Draw a 3D cube in isometric projection

vec2 iso_project(vec3 pos){
  return (vec2){
	 .x= (pos.x - pos.z) * 0.866f,
	 .y= -((pos.x + pos.z) * 0.5 - pos.y) 
  };
}

void test_sdf();

int main(int argc, char ** argv){
  int test = 0;
  for(int i = 0; i < argc; i++){
	 if(strcmp(argv[i], "--test") == 0){
		test = 1;
	 }
  }

  if(test){
	 TEST(test_sdf);
	 return 0;
  }

  camera cam = {0};
  
  MiniWindow * win = mw_create_window(WIDTH,HEIGHT, pixels, "hello");
  unsigned int value = 0xFFFF;
  float time = 0.0;
  while(1){
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
