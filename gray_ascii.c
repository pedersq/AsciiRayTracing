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

#define epsilon 0.0005

char float_to_ascii(float x);
void print_img();
void clearScreen();
struct OBJTriangle* closest_intersection(struct Ray3* ray, struct OBJ* object);
struct Ray3* gen_viewing_ray(float i, float j);
int is_point_in_triangle(struct Ray3* ray, struct OBJTriangle* triangle);

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

struct OBJ* objects[10];
int num_objects = 0;

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

void initScene() {
  camera = Vec3(0.0, 0.0, 0.0);
  view_direction = Vec3(0.0, 0.0, 1.0);
  struct OBJ* obj = load_obj("data/Sphere.obj");

  printf("Found %d triangles!\n", obj->num_triangles);

  objects[0] = obj;
  num_objects += 1;
}


int main() {

  initScene();

  return 0;

}





void a() {

  for (int j = 0; j < pixels_y; j++) {
    for (int i = 0; i < pixels_x; i++) {

      // Generate the viewing ray
      struct Ray3* viewing_ray = gen_viewing_ray(i, j);

      for (int i = 1; i <= num_objects; i++) {
        struct OBJTriangle* triangle = closest_intersection(viewing_ray, objects[i]);

        // TODO: stopped working on lecture 7

        // Probably need to calculate the lighting next
        // Then color in the pixel accordingly

      }

    }
  }
}

struct OBJTriangle* closest_intersection(struct Ray3* ray, struct OBJ* object) {

  if (object->num_triangles == 0) {
    return NULL;
  }

  struct OBJTriangle* triangle = object->triangles;

  // min_distance = infinity
  // closest_tri = NULL
  // While triangle aint null:
    // distance to triangle = is_point_in_triangle()
    // if d_to_t is less than min_distance
      // min_distance = d_to_t
      // closest_tri = current triangle

  // return cloestes triangle

}

// Returns -1 if not in triangle,
// Returns distance from start of ray to the end if in the triangle
int is_point_in_triangle(struct Ray3* ray, struct OBJTriangle* triangle) {

  // This function will:
    // Find the intersection of the ray with the plane if exists
    // Check if it's in the triangle (using barycentric coordinates)


  //TODO: find intersection with the plane first

  struct Vec3* point = ray_plane_intersect(ray, triangle);

  float tri_area = area_of_triangle(triangle);
  float alpha = barycentric_alpha(point, triangle->position2, triangle->position3, tri_area);
  float beta = barycentric_beta(point, triangle->position1, triangle->position3, tri_area);
  float gamma = 1 - alpha - beta;

}

struct Vec3* ray_plane_intersect(struct Ray3* ray, struct OBJTriangle* triangle) {

  struct Vec3* normal = plane_normal(triangle);
  float D = -triangle->position1->x * normal->x +
            -triangle->position1->y * normal->y +
            -triangle->position1->z * normal->z;

  // Calculate t for the ray where they intersect
    // Need to check if maybe the ray doesn't land.

  //float t = -((D + ))

}

int point_in_barycentric(float alpha, float beta, float gamma) {
  return alpha <= 1 && alpha >= 0 &&
      beta <= 1 && beta >= 0 &&
      gamma <= 1 && gamma >= 0 &&
      float_equal(alpha + beta + gamma, 1.0);
}

int floats_equal(float f1, float f2) {
  return fabsf(f1 - f2) <= epsilon;
}

float area_of_triangle(struct OBJTriangle* triangle) {
  struct Vec3* AB = subVec3(triangle->position2, triangle->position1);
  struct Vec3* AC = subVec3(triangle->position3, triangle->position1);
  struct Vec3* AB_cross_BC = crossVec3(AB, AC);
  float areaABC = lenVec3(AB_cross_BC) / 2;
  free(AB_cross_BC);
  free(AB);
  free(AC);
  return areaABC;
}

float barycentric_alpha(struct Vec3* point, struct Vec3* B, struct Vec3* C, float areaABC) {
  struct Vec3* PB = subVec3(B, point);
  struct Vec3* PC = subVec3(C, point);
  struct Vec3* cross = crossVec3(B, C);
  float alpha = lenVec3(cross) / (2 * areaABC);
  free(PB);
  free(PC);
  free(cross);
  return alpha;
}

float barycentric_beta(struct Vec3* point, struct Vec3* A, struct Vec3* C, float areaABC) {
  struct Vec3* PC = subVec3(C, point);
  struct Vec3* PA = subVec3(A, point);
  struct Vec3* cross = crossVec3(A, C);
  float beta = lenVec3(cross) / (2 * areaABC);
  free(PC);
  free(PA);
  free(cross);
  return beta;
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
