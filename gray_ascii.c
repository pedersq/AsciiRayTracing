#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "vectors.h"
#include "OBJ.h"

#define SCALE_LEN 66
#define pixels_x 150
#define pixels_y 40

char float_to_ascii(float x);
void print_img();
void clearScreen();
struct Ray3* gen_viewing_ray(float i, float j);

// The output image / frame
float img[pixels_y][pixels_x];

// Camera Position
struct Vec3* camera;

// Distance to viewing window from camera eye
float distance_to_window = 1;

// Camera viewing direction
struct Vec3* view_direction;

// Viewport
int viewport_height = 1;
int viewport_width = 1;

char grayscale[] =  "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/()1{}[]?-_+~<>i!lI;:,^`'. ";


void render_frame() {
  clearScreen();
  for (int j = 0; j < pixels_y; j++) {
    for (int i = 0; i < pixels_x; i++) {
      printf("%c", float_to_ascii(img[j][i]));
    }
    printf("\n");
  }
}

void clearScreen()
{
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
}

void initCamera() {
  camera = Vec3(0.0, 0.0, 0.0);
  view_direction = Vec3(0.0, 0.0, 1.0);
}


int main() {

  char a[] = "Sphere.obj";

  struct OBJ* obj = load_obj(a);

  return 0;

}





void a() {

  for (int j = 0; j < pixels_y; j++) {
    for (int i = 0; i < pixels_x; i++) {

      // Generate the viewing ray
      struct Ray3* viewing_ray = gen_viewing_ray(i, j);

    }
  }
}

struct Ray3* gen_viewing_ray(float i, float j) {
  // Convert (i, j) to (u, v)
  float u = ((i + 0.5) / 2 + 0.5) * viewport_width;
  float v = ((j + 0.5) / 2 + 0.5) * viewport_height;

  // TODO: this might be wrong
  struct Vec3* d = Vec3(-u, -v, -distance_to_window);

  // Create basis
  struct Vec3* w_vec = normalize3(negate3(view_direction));

  struct Vec3* up = Vec3(0.0, 1.0, 0.0);
  struct Vec3* u_vec = normalize3(crossVec3(up, w_vec));

  struct Vec3* v_vec = crossVec3(w_vec, u_vec);

  struct Vec3* p1 = multiplyVec3(u_vec, u);
  struct Vec3* p2 = multiplyVec3(v_vec, v);
  struct Vec3* p3 = multiplyVec3(w_vec, -distance_to_window);

  struct Vec3* ray_direction = add3Vec3(p1, p2, p3);

  free(d);
  free(w_vec);
  free(up);
  free(u_vec);
  free(v_vec);
  free(p1);
  free(p2);
  free(p3);
  struct Ray3* ray = Ray3(camera, ray_direction);
  return ray;
}

char float_to_ascii(float x) {
  if (x > 1.0) {
    x = 1.0;
  }

  int pos = (int) (x * SCALE_LEN);
  return grayscale[pos];
}
