#ifndef COMMON_LIB
#define COMMON_LIB

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned char u8;
typedef int i32;
typedef long int i64;

typedef float f32;

char *read_entire_file(char *path, size_t *s) {
  printf("[INFO] Reading file %s\n", path);
  FILE *fp = fopen(path, "r");

  if (fp == NULL) {
    printf("[WARNING] Could not open file\n");
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  printf("[INFO] File size: %lld bytes\n", size);

  char *buf = (char *)malloc(size + 1);
  if (buf == NULL) {
    printf("[WARNING] Failed to allocate memory for reading file\n");
    fclose(fp);
    return NULL;
  }

  size_t read_size = fread(buf, 1, size, fp);
  buf[read_size] = '\0';
  if (read_size != size) {
    printf("[WARNING] Could not read the entire file %s => size: %lld, read: "
           "%lld\n",
           path, size, read_size);
  }

  fclose(fp);
  *s = read_size;

  printf("[INFO] Successfully read file\n");
  return buf;
}

// typedef struct {
//   byte r;
//   byte g;
//   byte b;
// } Pixel;

// void write_bitmap(Pixel *pixels) {
//   struct bitmap_file_header {
//     byte bitmap_type
//   }
// }

// Math stuff
typedef struct {
  f32 x;
  f32 y;
} Vector2;

typedef struct {
  Vector2 start;
  Vector2 end;
} Line;

f32 min_f32(f32 a, f32 b) { return a > b ? b : a; }
f32 max_f32(f32 a, f32 b) { return a > b ? a : b; }

Vector2 vec2_add_scalar(Vector2 v, f32 s) {
  return (Vector2){
      .x = v.x + s,
      .y = v.y + s,
  };
}

Vector2 vec2_add(Vector2 v1, Vector2 v2) {
  return (Vector2){
      .x = v1.x + v2.x,
      .y = v1.y + v2.y,
  };
}

Vector2 vec2_sub(Vector2 v1, Vector2 v2) {
  return (Vector2){
      .x = v1.x - v2.x,
      .y = v1.y - v2.y,
  };
}

Vector2 vec2_scale(Vector2 v, f32 s) {
  return (Vector2){.x = v.x * s, .y = v.y * s};
}

f32 vec2_length(Vector2 v) { return sqrtf(v.x * v.x + v.y * v.y); }

Vector2 vec2_normalize(Vector2 v) {
  f32 il = 1 / vec2_length(v);
  return (Vector2){.x = v.x * il, .y = v.y * il};
}

bool line_intersect(Vector2 l1p1, Vector2 l1p2, Vector2 l2p1, Vector2 l2p2,
                    Vector2 *intersection) {
  f32 a1 = l1p2.y - l1p1.y;
  f32 b1 = l1p1.x - l1p2.x;
  f32 a2 = l2p2.y - l2p1.y;
  f32 b2 = l2p1.x - l2p2.x;

  f32 c1 = a1 * l1p1.x + b1 * l1p1.y;
  f32 c2 = a2 * l2p1.x + b2 * l2p1.y;
  f32 det = a1 * b2 - a2 * b1;

  if (det == 0) {
    return false;
  }

  f32 x = (b2 * c1 - b1 * c2) / det;
  f32 y = (a1 * c2 - a2 * c1) / det;

  f32 min_l1x = min_f32(l1p1.x, l1p2.x);
  f32 max_l1x = max_f32(l1p1.x, l1p2.x);
  f32 min_l1y = min_f32(l1p1.y, l1p2.y);
  f32 max_l1y = max_f32(l1p1.y, l1p2.y);

  f32 min_l2x = min_f32(l2p1.x, l2p2.x);
  f32 max_l2x = max_f32(l2p1.x, l2p2.x);
  f32 min_l2y = min_f32(l2p1.y, l2p2.y);
  f32 max_l2y = max_f32(l2p1.y, l2p2.y);

  bool l1 = min_l1x <= x && max_l1x >= x && min_l1y <= y && max_l1y >= y;
  bool l2 = min_l2x <= x && max_l2x >= x && min_l2y <= y && max_l2y >= y;

  intersection->x = x;
  intersection->y = y;
  return l1 && l2;
}

#endif