#ifndef _EXAMPLE_H
#define _EXAMPLE_H

#include <stdint.h>

typedef struct {
  uint32_t width, height;
  uint8_t *pixel;
} Frame;

typedef struct {
  float x, y, z;
} Vec3f;
typedef struct {
  float x, y;
} Vec2f;

typedef struct {
  Vec3f light_pos,
        light_color,
        diffuse_color;
} Scene_CTX;

float vec3f_dot(Vec3f a, Vec3f b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}
Vec3f vec3f_sub(Vec3f a, Vec3f b) {
  return (Vec3f){a.x - b.x, a.y - b.y, a.z - b.z};
}
Vec3f vec3f_add(Vec3f a, Vec3f b) {
  return (Vec3f){a.x + b.x, a.y + b.y, a.z + b.z};
}
Vec3f vec3f_scale(Vec3f a, float t) {
  return (Vec3f){a.x * t, a.y  * t, a.z * t};
}
float vec3f_length(Vec3f a) {
  return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}
Vec3f vec3f_normalize(Vec3f a) {
  float len = vec3f_length(a);
  return (Vec3f){a.x / len, a.y / len, a.z / len};
}

Vec2f vec2f_scale(Vec2f a, float t) {
  return (Vec2f){a.x * t, a.y * t};
}
Vec2f vec2f_sub(Vec2f a, Vec2f b) {
  return (Vec2f){a.x - b.x, a.y - b.y};
}

char get_base64(uint8_t p) {
  if (p <= 25) {
    return (p + 65);
  }
  else if (p <= 51) {
    return (p + 71);
  }
  else if (p <= 61) {
    return (p - 4);
  }
  else if (p == 62) return '+';

  return '/';
}

void generate_frame(Frame*);
void draw_to_screen(Frame*);

#endif // _EXAMPLE_H
