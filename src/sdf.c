#include "main.h"


typedef enum{
  SDF_SPHERE = 31

  ,
  SDF_BOX,
  SDF_OFFSET,
  SDF_OFFSET8,
  SDF_SCALE,
  SDF_UNION,
  SDF_SUBTRACT,
  SDF_NOTHING,
  SDF_NEGATE,
  SDF_POINTER
}sdf_type;

f32 vec3_length(vec3 p){
  return sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
}

u8 read_u8(void ** p){
  u8 * pu = *p;
  *p += 1;
  return pu[0];
}


i8 read_i8(void ** p){
  i8 * pu = *p;
  *p += 1;
  return pu[0];
}


u32 read_u32(void ** p){
  u32 * pu = *p;
  *p += sizeof(u32);
  return pu[0];
}


u64 read_u64(void ** p){
  u64 * pu = *p;
  *p += sizeof(*pu);
  return pu[0];
}

u8 read_f32(void ** p){
  f32 * pu = *p;
  *p += 4;
  return pu[0];
}

 
void write_u8(void ** p, u8 v){
  u8 * p2 = *p;
  
  p2[0] = v;
  *p = *p+1;
}

void write_i8(void ** p, i8 v){
  i8 * p2 = *p;
  
  p2[0] = v;
  *p = *p+1;
}

void write_f32(void ** p, f32 v){
  f32 * p2 = *p;
  p2[0] = v;
  *p = *p + 4;
  
}

void write_u32(void ** p, u32 v){
  u32 * p2 = *p;
  p2[0] = v;
  *p = p2 + 1; 
}

void write_u64(void ** p, u64 v){
  u64 * p2 = *p;
  p2[0] = v;
  *p = p2 + 1; 
}

f32 sdf_distance(void ** sdf, vec3 p){
  sdf_type t = read_u8(sdf);
  switch(t){
  case SDF_SPHERE:
    {
      f32 r = read_f32(sdf);
      return vec3_length(p) - r;
    }
  case SDF_BOX:
    {
      f32 sx = read_f32(sdf);
      f32 sy = read_f32(sdf);
      f32 sz = read_f32(sdf);
      vec3 d = {
        .x = fabsf(p.x) - sx,
        .y = fabsf(p.y) - sy,
        .z = fabsf(p.z) - sz
      };
      f32 outside = vec3_length((vec3){
          .x = fmaxf(d.x, 0.0f),
          .y = fmaxf(d.y, 0.0f),
          .z = fmaxf(d.z, 0.0f)
        });
      f32 inside = fminf(fmaxf(d.x, fmaxf(d.y, d.z)), 0.0f);
      return outside + inside;
    }
  case SDF_UNION:
    {
      u32 count = read_u32(sdf);
      f32 m = 1.0f/0.0f;
      for(u32 i = 0; i < count; i++){
        f32 d = sdf_distance(sdf, p);
        if(d < m)
          m = d;
      }
      return m;
    }
  case SDF_SUBTRACT:
    {
      f32 da = sdf_distance(sdf, p);
      f32 db = sdf_distance(sdf, p);
      return fmaxf(da, -db);
    }
  case SDF_OFFSET:
    {
      f32 x = read_f32(sdf);
      f32 y = read_f32(sdf);
      f32 z = read_f32(sdf);
      return sdf_distance(sdf,
                          (vec3){
                            .x = p.x - x,
                            .y = p.y - y,
                            .z = p.z - z });
    }
      
  case SDF_OFFSET8:
    {
      f32 x = read_i8(sdf);
      f32 y = read_i8(sdf);
      f32 z = read_i8(sdf);
      return sdf_distance(sdf,
                          (vec3){
                            .x = p.x - x,
                            .y = p.y - y,
                            .z = p.z - z });
		
    }
  case SDF_SCALE:
    {
      f32 s = read_f32(sdf);
      vec3 ps = {
        .x = p.x / s,
        .y = p.y / s,
        .z = p.z / s
      };
      return sdf_distance(sdf, ps) * s;
    }
  case SDF_NOTHING:
    return 1.0f/0.0f;
  case SDF_NEGATE:
    return -sdf_distance(sdf, p);
  case SDF_POINTER:
	 
	 void * sdf2 = (void *) read_u64(sdf);
	 return sdf_distance(sdf2,p);
  }
  return 0.0f;
}

typedef struct{
  int expect;
  void * sdf;
}sdf_reader;

sdf_reader sdf_begin_read(void * buffer){
  sdf_reader reader = {.sdf = buffer, .expect = 1};
  return reader;
}

sdf_type sdf_reader_type(sdf_reader * reader){
  void * sdf2 = reader->sdf;
  return read_u8(&sdf2);
}

int sdf_reader_next(sdf_reader * reader){
  
  reader->expect -= 1;
  void **sdf = &reader->sdf;
  sdf_type t = read_u8(sdf);
  switch(t){
  case SDF_SPHERE:
    read_f32(sdf);
    break;
  case SDF_NOTHING:
    break;
  case SDF_NEGATE:
	 reader->expect += 1;
	 break;
  case SDF_OFFSET:
    read_f32(sdf);
    read_f32(sdf);
    read_f32(sdf);
    reader->expect += 1;
    break;
  case SDF_BOX:
    read_f32(sdf);
    read_f32(sdf);
    read_f32(sdf);
    break;
  case SDF_OFFSET8:
    reader->expect += 1;
    read_u8(sdf);
    read_u8(sdf);
    read_u8(sdf);
    break;
  case SDF_SCALE:
    reader->expect += 1;
    read_f32(sdf);
    break;
  case SDF_UNION:
    reader->expect += read_u32(sdf);
    break;
  case SDF_SUBTRACT:
    reader->expect += 2;
    break;
  case SDF_POINTER:
	 read_u64(sdf);
	 break;
  default:
    assert(0);
  }
  return reader->expect;
}

void sdf_reader_sphere(sdf_reader * reader, f32 * size){
  sdf_reader reader2 = *reader;
  assert(read_u8(&reader2.sdf) == SDF_SPHERE);
  *size =read_f32(&reader2.sdf); 
}

void sdf_reader_offset(sdf_reader * reader, f32 * x, f32 * y, f32 * z){
  sdf_reader reader2 = *reader;
  sdf_type t = read_u8(&reader2.sdf);
  assert(t == SDF_OFFSET | t== SDF_OFFSET8);
  if(t == SDF_OFFSET){
	 *x =read_f32(&reader2.sdf);
	 *y =read_f32(&reader2.sdf);
	 *z =read_f32(&reader2.sdf);
  }else{
	 *x =read_i8(&reader2.sdf);
	 *y =read_i8(&reader2.sdf);
	 *z =read_i8(&reader2.sdf);
  }
   
}

void sdf_reader_union(sdf_reader * reader, u32 * c){
  sdf_reader reader2 = *reader;
  sdf_type t = read_u8(&reader2.sdf);
  assert(t == SDF_UNION);
  *c = read_u32(&reader2.sdf);
  printf("got: %i\n", t);
  assert(((u32)*c) < 10);
}

void sdf_optimize_accept(){

}

// SDF Optimizations:
// - subtract -> left side should only contain things that are inside.
void sdf_optimize(void * sdf, void * test_bounds){
  sdf_reader reader2 = sdf_begin_read(sdf);
  switch(sdf_reader_type(&reader2)){
  case SDF_SPHERE:
  case SDF_BOX:
	 sdf_optimize_accept();
	 break;
  case SDF_OFFSET:
  case SDF_OFFSET8:
		{
		  f32 x,y,z;
		  sdf_reader_offset(&reader2, &x, &y, &z);
		  u8 buffer[100];
		  void * p = buffer;
		  write_u8(&p, SDF_OFFSET);
		  write_f32(&p, x);
		  write_f32(&p, y);
		  write_f32(&p, z);
		  write_u8(&p, SDF_POINTER);
		  write_u64(&p, (u64)test_bounds);
		  break;
		}
  default:
	 assert(false);

  }
}

static void sdf_print1(sdf_reader * reader, int indent){
  for(int i = 0; i < indent; i++) printf(" ");
  switch(sdf_reader_type(reader)){
  case SDF_UNION:
	 {
		u32 count = 0;
		sdf_reader_union(reader, &count);
		printf("[Union (%i)]\n", count);
		sdf_reader_next(reader);
		for(u32 i = 0; i < count; i++){
		  sdf_print1(reader, indent + 1);

		}
		break;
	 }
  case SDF_OFFSET8:
  case SDF_OFFSET:
	 {
		f32 x,y,z;
		sdf_reader_offset(reader, &x, &y, &z);
		printf("[Offset x:%f y:%f z:%f]\n", x, y, z);
		sdf_reader_next(reader);
		sdf_print1(reader, indent + 1);
		break;
	 }
  case SDF_SPHERE:
	 {
		f32 radius;
		sdf_reader_sphere(reader, &radius);
		sdf_reader_next(reader);
		printf("[Sphere radius:%f]\n", (double)radius);
		indent -= 1;
		break;
	 }
  default:
	 printf("type: %i %i\n", sdf_reader_type(reader), reader->expect);
	 break;
	 
	 
  }
}

void sdf_print(void * sdf){
  sdf_reader r = sdf_begin_read(sdf);
  sdf_print1(&r, 0);
}

int test_sdf_basic(){
  u8 buffer[200] = {0};
  void * p = buffer;
  write_u8(&p, SDF_UNION);
  write_u32(&p, 2);
  write_u8(&p, SDF_SPHERE);
  write_f32(&p, 10.0f);
  write_u8(&p, SDF_OFFSET8);
  write_i8(&p, -10);
  write_i8(&p, 0);
  write_i8(&p, 0);
  write_u8(&p, SDF_SPHERE);
  write_f32(&p, 1.0f);
  p = buffer;
  f32 dist = sdf_distance(&p, (vec3){.x = 11, .y = 0, .z = 0});
  printf("Structure size: %zu\n", (size_t)(p - (void*)buffer));
  printf("Dist: %f\n", (double)dist);
  if(fabs(dist - 1.0) > 0.001)
    return 1;
  sdf_type types[] = {SDF_UNION, SDF_SPHERE, SDF_OFFSET8, SDF_SPHERE};
  sdf_reader reader = sdf_begin_read(buffer);
  int typei =0;
  do{
	 
	 assert(types[typei++] == sdf_reader_type(&reader));
  }while(sdf_reader_next(&reader));

  sdf_print(buffer);
  
  u8 buffer2[] = {SDF_NOTHING};
  void * buffer2_ptr = buffer2;
  f32 dist2 = sdf_distance(&buffer2_ptr, (vec3){.x = 1, .y = 1, .z = 1});
  assert(isinf(dist2));
  printf("Dist2: %f\n", (double)dist2);
  return 0;
}
void test_sdf_inf(){
  u8 buffer2[] = {SDF_NOTHING};
  void * buffer2_ptr = buffer2;
  f32 dist2 = sdf_distance(&buffer2_ptr, (vec3){.x = 1, .y = 1, .z = 1});
  assert(isinf(dist2));
  return;
}

void test_sdf(){
  TEST(test_sdf_basic);
  TEST(test_sdf_inf);
  
  printf("test passed!\n");
}

