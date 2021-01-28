#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <float.h>
#include "vectors.h"
#include "OBJ.h"

typedef struct Vec3* P3;



#define SCALE_LEN 66
#define pixels_x 50
#define pixels_y 30

#define epsilon 0.0005

char float_to_ascii(float x);
void render_frame();
void clearScreen();
struct OBJTriangle* closest_intersection(struct Ray3* ray, struct OBJ* object, float* minT);
struct Ray3* gen_viewing_ray(float i, float j);
float is_point_in_triangle(struct Ray3* ray, struct OBJTriangle* triangle);
float barycentric_alpha(struct Vec3* point, struct Vec3* B, struct Vec3* C, float areaABC);
float barycentric_beta(struct Vec3* point, struct Vec3* A, struct Vec3* C, float areaABC);
float area_of_triangle(struct OBJTriangle* triangle);
int floats_equal(float f1, float f2);
int point_in_barycentric(float alpha, float beta, float gamma);
struct Vec3* ray_plane_intersect(struct Ray3* ray, struct OBJTriangle* triangle, float* rayT);
float calculate_lighting(struct Ray3* viewing_ray, float t, struct OBJTriangle* triangle);
void process_frame();
void compute_barycentric(struct Vec3* point, struct Vec3* a, struct Vec3* b, struct Vec3* c, float* alpha, float* beta, float* gamma);
float area_of_triangle2(P3 a, P3 b, P3 c);

// The output image / frame
float img[pixels_x][pixels_y];

// Camera Position
struct Vec3* camera;
float cx;
float cy;
float cz;
float cangle;
float radius = 5;

// Distance to viewing window from camera eye
float distance_to_window = 1;

// Camera viewing direction
struct Vec3* view_direction;

// Viewport
float viewport_height = 1;
float viewport_width = 1;

// Light in the scene

struct Vec3 light = {0.0, 1.0, 1.0};
struct Vec3 neg_light = {0.0, -1.0, -1.0};
float intensity = 30.0;

struct OBJ* objects[10];
int num_objects = 0;

char grayscale[] =  "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/()1{}[]?-_+~<>i!lI;:,^`'. ";


void render_frame() {
  //clearScreen();
  for (int j = 0; j < pixels_y; j++) {
    for (int i = 0; i < pixels_x; i++) {
      printf("%c", float_to_ascii(img[i][j]));
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
  camera = Vec3(0.0, 0.0, 5);

  view_direction = normalize3(negate3(camera));
  print_vec3(camera);
  print_vec3(view_direction);
  struct OBJ* obj = load_obj("data/Sphere.obj");



  objects[0] = obj;

  num_objects = 1;

  printf("Scene loaded.\n");
}


int main() {

  initScene();

  //for (int i = 0; i < 32; i++) {

    process_frame();

    render_frame();

    //tickScene();

  //}


  return 0;

}





void process_frame() {

  int pixels_shaded = 0;

  for (int j = 0; j < pixels_y; j++) {
    for (int i = 0; i < pixels_x; i++) {


      // Generate the viewing ray
      //struct Ray3* viewing_ray = gen_viewing_ray((float) i, (float) j);

      struct Ray3* viewing_ray = gen_viewing_ray((float) i, (float) j);



      // Keeps track of the cloeset triangle the ray hit
      // accross all objects in the scene
      struct OBJTriangle* triangle_hit = NULL;
      float min_t = FLT_MAX;

      // Checks every object in the scene for an intersction and saves the closest
      // triangle hit from any object
      for (int k = 0; k < num_objects; k++) {
        float t = FLT_MAX;
        struct OBJTriangle* triangle = closest_intersection(viewing_ray, objects[k], &t);

        if (t < min_t) {
          min_t = t;
          triangle_hit = triangle;
        }
      }

      // Shades the pixel based on the triangle hit
      if (triangle_hit != NULL) {
        pixels_shaded += 1;
        img[i][j] = calculate_lighting(viewing_ray, min_t, triangle_hit);
      } else {
        img[i][j] = 0.0;
      }

    }
  }
}

// Calculates the lighting for a light on the triangle hit by the ray
float calculate_lighting(struct Ray3* viewing_ray, float t, struct OBJTriangle* triangle) {

  // TODO:
  // CURRENTLY ONLY CALCULATES LIGHTING BASED ON THE NORMAL OF THE WHOLE
  // TRIANGLE, BUT IN THE FUTURE IT WOULD BE GOOD TO INTERPOLATE THE NORMAL
  // FROM THE 3 NORMALS STORED ON THE PASSED TRIANGLE

  struct Vec3* tri_normal = plane_normal(triangle);

  float n_dot_l = dotVec3(tri_normal, &light);
  free(tri_normal);


  // Squishes the value between 0 and 1
  //float value = tanhf();

  float max = fmaxf(n_dot_l * intensity, 0.0);

  return max;

}


// Returns the closest triangle that the ray interescts in the object
// Or null if it doesn't hit any
struct OBJTriangle* closest_intersection(struct Ray3* ray, struct OBJ* object, float* minT) {

  if (object->num_triangles == 0) {
    return NULL;
  }

  struct OBJTriangle* curr = object->triangles;

  float min_t = FLT_MAX;
  struct OBJTriangle* closest_tri = NULL;

  int t_hit = 0;



  while (curr != NULL) {
    float distance_to_triangle = is_point_in_triangle(ray, curr);
    t_hit += 1;

    if (distance_to_triangle != -FLT_MAX ) {
      if (min_t > distance_to_triangle) {
        min_t = distance_to_triangle;
        closest_tri = curr;
      }
    }
    curr = curr->next;
  }

  *minT = min_t;
  return closest_tri;

}


// This function will:
// Find the intersection of the ray with the plane if exists
// Check if it's in the triangle (using barycentric coordinates)
// Returns -1.0 if not in triangle,
// Returns distance from start of ray to the end if in the triangle
float is_point_in_triangle(struct Ray3* ray, struct OBJTriangle* triangle) {

  float t;
  struct Vec3* point = ray_plane_intersect(ray, triangle, &t);


  if (point == NULL) {
    return -FLT_MAX;
  }

  float tri_area = area_of_triangle(triangle);

  float alpha;
  float beta;
  float gamma;
  compute_barycentric(point,
                      triangle->position1,
                      triangle->position2,
                      triangle->position3,
                      &alpha,
                      &beta,
                      &gamma
  );


  if (point_in_barycentric(alpha, beta, gamma)) {
    return t;
  } else {
    return -FLT_MAX;
  }

}


void compute_barycentric(struct Vec3* point, struct Vec3* a, struct Vec3* b, struct Vec3* c, float* alpha, float* beta, float* gamma) {

  *alpha = area_of_triangle2(point, c, b) / area_of_triangle2(a, b, c);
  *beta = area_of_triangle2(point, c, a) / area_of_triangle2(a, b, c);
  *gamma = 1.0 - *alpha - *beta;

}

// Saves t of the ray to the passed float pointer
struct Vec3* ray_plane_intersect(struct Ray3* ray, struct OBJTriangle* triangle, float* rayT) {

  struct Vec3* normal = plane_normal(triangle);
  float D = -triangle->position1->x * normal->x +
            -triangle->position1->y * normal->y +
            -triangle->position1->z * normal->z;

  float a0 = ray->origin->x;
  float b0 = ray->origin->y;
  float c0 = ray->origin->z;

  float a = ray->dir->x;
  float b = ray->dir->y;
  float c = ray->dir->z;

  float A = normal->x;
  float B = normal->y;
  float C = normal->z;

  float denominator = (c*C + b*B + a*A);
  if (denominator <= epsilon && denominator >=0) {
    return NULL;
  }

  float numerator = (D + c0*C + b0*B + a0*A);

  float t =  -(numerator / denominator);

  *rayT = t;

  struct Vec3* tS = multiplyVec3(ray->dir, t);
  struct Vec3* point = addVec3(tS, ray->origin);
  free(tS);

  return point;

}

int point_in_barycentric(float alpha, float beta, float gamma) {
  return alpha <= 1 && alpha >= 0 &&
      beta <= 1 && beta >= 0 &&
      gamma <= 1 && gamma >= 0;
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

float area_of_triangle2(P3 a, P3 b, P3 c) {
  P3 AB = subVec3(b, a);
  P3 AC = subVec3(c, a);
  P3 AB_cross_BC = crossVec3(AB, AC);
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

  float u = ((((i + 1.0) - 0.5) / pixels_x) - 0.5) * viewport_width;
  float v = -((((j + 1.0) - 0.5) / pixels_y) - 0.5) * viewport_height;

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
  return grayscale[SCALE_LEN - pos];
}
