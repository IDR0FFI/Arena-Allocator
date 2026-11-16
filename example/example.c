#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include "example.h"
#include "../arena/arena.h"

bool intersect(Vec3f rp, Vec3f rd, Vec3f s, float r) {
  float t = vec3f_dot(vec3f_sub(s, rp), rd);
  Vec3f p = vec3f_scale(vec3f_add(rp, rd), t);

  float y = vec3f_length(vec3f_sub(p, s));

  return t >= 0.0f && y < r;
}

double elapsed_time = 0;
Scene_CTX *sc;

void generate_frame(Frame* f) {
  Vec3f LIGHT_POS     = sc->light_pos;
  Vec3f LIGHT_COLOR   = sc->light_color;
  Vec3f DIFFUSE_COLOR = sc->diffuse_color;

  Vec3f LIGHT_POS_NEG = {-LIGHT_POS.x, -LIGHT_POS.y, -LIGHT_POS.z};

  Vec2f  RESOLUTION = {f->width, f->height};

  for (size_t i=0; i<f->height; i++) {
    for (size_t j=0; j<f->width; j++) {
      Vec2f fragCoord = {j, i};
      Vec2f uv = vec2f_scale(vec2f_sub(fragCoord, vec2f_scale(RESOLUTION, 0.5f)), 1 / RESOLUTION.y);
      Vec3f color = {0.3f, 0.3f, 0.3f};

      Vec3f rp = {.0f, .0f, .0f};
      Vec3f rd = vec3f_normalize((Vec3f){uv.x, uv.y, 1.0f});

      Vec3f s = {cosf(elapsed_time) * 2.0f, 0.0f, sin(elapsed_time) + 5.0f};
      float r = 1.0f;

      float t = vec3f_dot(vec3f_sub(s, rp), rd);
      Vec3f p = vec3f_add(rp, vec3f_scale(rd, t));

      float y = vec3f_length(vec3f_sub(p, s));

      if (t >= 0.0f && y < r) {
        float x = sqrtf(r * r - y * y);
        float t1 = t - x;

        Vec3f ip = vec3f_add(rp, vec3f_scale(rd, t1));
        Vec3f id = vec3f_normalize(vec3f_sub(s, ip));

        if (intersect(ip, LIGHT_POS, s, r)) {
          color = DIFFUSE_COLOR;
        }
        else {
          float lightAngle = fmax(0.0f, vec3f_dot(id, vec3f_normalize(LIGHT_POS_NEG)));
          color = vec3f_add(DIFFUSE_COLOR, vec3f_scale(LIGHT_COLOR, lightAngle));
        }

      }
      f->pixel[i * f->width * 3 + j * 3 + 0] = (uint8_t)(color.x * 255);
      f->pixel[i * f->width * 3 + j * 3 + 1] = (uint8_t)(color.y * 255);
      f->pixel[i * f->width * 3 + j * 3 + 2] = (uint8_t)(color.z * 255);
    }
  }
}

void draw_to_screen(Frame* f) {
  char payload[f->width * f->height * 4];

  int index = 0;
  uint8_t *p = f->pixel;

  for (size_t i=0; i<f->width * f->height * 3; i+=3) {
    uint32_t b = 0;
    b = p[i] << 16 | p[i+1] << 8 | p[i+2];
    uint8_t p1 = (b >> 18) & 0x3f,
            p2 = (b >> 12) & 0x3f,
            p3 = (b >>  6) & 0x3f,
            p4 = (b >>  0) & 0x3f;

    char c1 = get_base64(p1),
         c2 = get_base64(p2),
         c3 = get_base64(p3),
         c4 = get_base64(p4);

    payload[index++] = c1;
    payload[index++] = c2;
    payload[index++] = c3;
    payload[index++] = c4;
  }
  
  printf("\e_Ga=T,f=24,v=%d,s=%d,r=50;%.*s\e\\", f->width, f->height, (int) sizeof(payload), payload);
}

int main() {
  Arena *arena = arena_alloc();

  Frame *screen_buffer = arena_push_struct(arena, Frame);
  screen_buffer->width  = 128;
  screen_buffer->height = 128;
  screen_buffer->pixel  = arena_push_arrayZero(arena, uint8_t, screen_buffer->width * screen_buffer->height * 3);

  sc = arena_push_struct(arena, Scene_CTX);
  sc->light_pos     = (Vec3f){0.7071f, 0.7071f, 0.0f};
  sc->light_color   = (Vec3f){0.4f, 0.4f, 0.0f};
  sc->diffuse_color = (Vec3f){0.3f, 0.1f, 0.1f};

  // enabling certain terminal mode
  printf("\e[?1049h"); // new screen buffer

  // Disabling echo
  struct termios oldt, newt;
  {
    int oldflags = fcntl(STDIN_FILENO, F_GETFL);

    if (tcgetattr(STDIN_FILENO, &oldt) == -1) {
      perror("tcgetattr");
      return 1;
    }

    newt = oldt;
    newt.c_lflag &= !(ECHO | ICANON);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1 ||
      fcntl(STDIN_FILENO, F_SETFL, oldflags | O_NONBLOCK) == -1) {
      perror("set terminal mode");
      tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
      return 1;
    }
  }

  bool is_Running = true;
  char c;
  clock_t start = 0, end;

  while (is_Running) {
    printf("\e[0;0H");  // back to home position

    read(STDIN_FILENO, &c, 1);
    if (c == 'q') break;

    // generate_frame
    generate_frame(screen_buffer);
    // draw_to_screen
    draw_to_screen(screen_buffer);

    end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    // Setting fixed frameRate
    if (time_taken < 0.016f) {
      struct timespec ts;
      ts.tv_sec = 0.016f - time_taken;
      ts.tv_nsec = (0.016f - time_taken) * 1000000;
      nanosleep(&ts, &ts);
    }

    start = end;
    elapsed_time = (double) end / CLOCKS_PER_SEC;
  }

  // reverting back to original state
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  printf("\e[?1049l");

  arena_release(arena);

  return 0;
}
