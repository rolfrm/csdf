#include "main.h"


void draw_pixel(int x, int y, unsigned int value){
  if(x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
	 return;
  pixels[x + y * WIDTH] = value;
}

void draw_line (int x1, int x2, int y, unsigned int value){
  if(x2 < x1){
	 int x0 = x1;
	 x1 = x2;
	 x2 = x0;
  }
  if(x1 <0) x1 = 0;
  if(x2 >= WIDTH) x2 = WIDTH - 1;
  if(y < 0 || y >= HEIGHT) return;
  unsigned int * p = &pixels[y * WIDTH];
  for(int i = x1; i < x2; i++)
	 p[i] = value;
}




void draw_cube(int screen_x, int screen_y, int size, unsigned int color, int z_depth) {
    int half_size = size / 2;
    int quarter_size = size / 4;
    int height = size / 3;  // Cube height in screen space
    
    // Center on screen
    int center_x = screen_x + WIDTH / 2;
    int center_y = screen_y + HEIGHT / 2;
    
    // Only draw wireframe for larger cubes, fill smaller ones
    int wireframe_only = (size > 32);
    
    // Depth-based alpha for layering2
    float depth_alpha = 1.0f - (abs(z_depth) / 1000.0f);
    depth_alpha = fmaxf(0.1f, fminf(1.0f, depth_alpha));
    
    // Draw the three visible faces of the cube
    
    // Top face (diamond)
    for(int dy = -quarter_size; dy <= quarter_size; dy++) {
        int y = center_y + dy - height;
        if(y < 0 || y >= HEIGHT) continue;
        
        int width = half_size - abs(dy * 2);
        for(int dx = -width; dx <= width; dx++) {
            int x = center_x + dx;
            if(x < 0 || x >= WIDTH) continue;
            
            if(wireframe_only && abs(dx) != width && abs(dy) != quarter_size) continue;
            
            // Top face is brightest
            unsigned int r = ((color >> 16) & 0xFF) * depth_alpha;
            unsigned int g = ((color >> 8) & 0xFF) * depth_alpha;
            unsigned int b = (color & 0xFF) * depth_alpha;
            
            pixels[y * WIDTH + x] = (r << 16) | (g << 8) | b;
        }
    }

// Left face (parallelogram going down and left)
    for(int h = 0; h < height; h++) {
        int y = center_y - height + h;
        if(y < 0 || y >= HEIGHT) continue;
        
        for(int dx = 0; dx < half_size; dx++) {
			 int x = center_x - half_size + dx - ( h / 2);
            if(x < 0 || x >= WIDTH) continue;
            
            if(wireframe_only && h != 0 && h != height-1 && dx != 0 && dx != half_size-1) continue;
            
            // Left face is darker
            unsigned int r = ((color >> 16) & 0xFF) * 0.6f * depth_alpha;
            unsigned int g = ((color >> 8) & 0xFF) * 0.6f * depth_alpha;
            unsigned int b = (color & 0xFF) * 0.6f * depth_alpha;
            
            pixels[y * WIDTH + x] = (r << 16) | (g << 8) | b;
        }
    }
    
    // Right face (parallelogram going down and right)
    for(int h = 0; h < height; h++) {
        int y = center_y - height + h;
        if(y < 0 || y >= HEIGHT) continue;
        
        for(int dx = 0; dx < half_size; dx++) {
			 int x = center_x + dx + (h / 2);
            if(x < 0 || x >= WIDTH) continue;
            
            if(wireframe_only && h != 0 && h != height-1 && dx != 0 && dx != half_size-1) continue;
            
            // Right face is medium brightness
            unsigned int r = ((color >> 16) & 0xFF) * 0.8f * depth_alpha;
            unsigned int g = ((color >> 8) & 0xFF) * 0.8f * depth_alpha;
            unsigned int b = (color & 0xFF) * 0.8f * depth_alpha;
            
            pixels[y * WIDTH + x] = (r << 16) | (g << 8) | b;
        }
    }
}


void draw_screen(camera * cam, float time){
  float t = time * 5.0;

  for(int i = -5; i < 5; i++){
	 for(int j = -5; j < 5; j++){
		for(int k = 0; k < 1; k++){

		vec3 surf = (vec3){.x = i * 64, .y = k * 64, .z = j * 64};
		
		vec2 sp = iso_project(surf);

		if((i==-3 || i==2 || i == 1 || i == -1 || i == 3) && j == 0 && k == 0){
		  vec2 mid = sp;
		  vec3 surf0 = surf;
		  surf.y = surf.y + 64;
		  //surf.x = surf.x - 64;
		  //surf.z = surf.z - 64;
		  vec2 top = iso_project(surf);
		  surf = surf0;
		  //surf.y = surf.y-64;
		  surf.x = surf.x + 64;
		  surf.z = surf.z + 64;
		  vec2 bot = iso_project(surf);
		  surf = surf0;
		  surf.z = surf.z + 64;

		  vec2 left1 = iso_project(surf);
		  surf = surf0;
		  surf.x = surf.x + 64;
		  surf.y = surf.y + 64;

		  vec2 left2 = iso_project(surf);
		  
		  //printf("%f %f %f %f\n", bot.x, bot.y, left1.x, top.y);
		  for(int i = bot.y, gi = surf.y; i < top.y; i++,gi += 1){
			 if(i < -HEIGHT/2) continue;
			 int w = bot.x - left1.x;
			 
			  if(i < left1.y)
				 w = -((i - bot.y) * (left1.x - bot.x)) / (left1.y - bot.y);
			 if(i > left2.y)
			 	w = w - ((left2.y -i ) * (left1.x - bot.x)) / (top.y - left2.y);
			 vec2 sp = bot;
			 sp.y = i;
			 for(int i2 = -w; i2 < w; i2++){
				unsigned int color = 0x333333FF;
				/*
				if(i < left1.y){

				  if(i2 < 0){
					 int y = surf0.y + i + i2/2;
					 int x = surf0.x ;
					 int z = surf0.z + i2/0.866;

					 if((y % 16) == 0)
						color = 0;//0xFFFFFFFF;
					 else{
						color = 0xFFFF00FF;
					 }
				  }
					 else{
						int y = surf0.y + i2/2;
						int x = surf0.x + i2/0.866;
						int z = surf0.z;
						
						color = 0xFF00ffFF;
					 if((x%16) == 0)
						color = 0;
					 }

				}else if (i < mid.y){
				  if(i2 < 0){
					 int y = surf0.y + i + i2/2;
					 int x = surf0.x ;
					 int z = surf0.z + i2/0.866;

					 if((y % 16) == 0)
						color = 0;//0xFFFFFFFF;
					 else{
						color = 0xFFFF00FF;
					 }
					 
				  }
				  else{
					 int y = surf0.y + i2/2;
					 int x = surf0.x + i2/0.866;
					 int z = surf0.z;
					 
					 color = 0xFF00ffFF;
					 if((x%16) == 0)
						color = 0;
				  }

				  
				}else if (i < left2.y){
				  if(i2 < 0){
					 int y = surf0.y + i + i2/2;
					 int x = surf0.x ;
					 int z = surf0.z + i2/0.866;
					 
					 if((y % 16) == 0)
						color = 0;//0xFFFFFFFF;
					 else{
						color = 0xFFFF00FF;
					 }
					 
					 if(y > (surf0.y + 32)){
						color = 0xFF0000FF;
					 }
				  }
				  else{
					 int y = surf0.y + i2/2;
					 int x = surf0.x + i2/0.866;
					 int z = surf0.z;
					 
					 color = 0xFF00ffFF;
					 if((x%16) == 0)
						color = 0;
				  }
				  }*/
				if(i2 < 0){
				  color = 0xFF0000FF;
				  if(sp.y > (bot.y + 64) - (sp.x + i2 - bot.x) * 0.5)
					 color =0xFFFF0000;
				}
				
				draw_pixel(sp.x + WIDTH/2 + i2, sp.y + HEIGHT/2, color);
				
			 }
			 
			 //draw_line(sp.x + WIDTH/2 -w, sp.x + WIDTH/2 + w, sp.y + HEIGHT/2, 0xFF0000FF);
			 
		  }
		}

		unsigned int color = 0xFFFFFFFFLU;
		if(i == 0)
		  color = 0xFF0000FFLU;
		if(j == 0)
		  color = 0xFFFF00FFLU;
		draw_pixel(sp.x + WIDTH/2, sp.y + HEIGHT/2, color);
		}
	 }
  }
  draw_pixel(10, 10,(((int) t) % 2 == 0 ? 0xFFFF00FF : 0xFF00FFFF));
  draw_pixel(11, 10,(((int) t) % 2 == 0 ? 0xFFFF00FF : 0xFF00FFFF));
  draw_pixel(10, 11,(((int) t) % 2 == 0 ? 0xFFFF00FF : 0xFF00FFFF));
  draw_pixel(11, 11,(((int) t) % 2 == 0 ? 0xFFFF00FF : 0xFF00FFFF));
  
}
