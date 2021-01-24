#include "OBJ.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// filename must be null terminated
struct OBJ* load_obj(char* filename) {

  FILE* f = fopen(filename, "r");

  struct OBJ* obj = OBJ(NULL, NULL, NULL, 0, 0, 0);

  int flag = 0;
  char type[2];
  char triangle_point[30];
  float f1;
  float f2;
  float f3;
  int i1;
  int i2;
  int i3;
  flag = fscanf(f, "%s", type);
  while (flag != EOF) {
    // Texture
    if (type[0] == 'v' && type[1] == 't') {
      flag = fscanf(f, "%d %d", &i1, &i2);
    }

    // Normal
    if (type[0] == 'v' && type[1] == 'n') {
      flag = fscanf(f, "%f %f %f", &f1, &f2, &f3);
      obj->num_normals += 1;
    }

    // Position
    if (type[0] == 'v' && type[1] == '\0') {
      flag = fscanf(f, "%f %f %f", &f1, &f2, &f3);
      obj->num_normals += 1;
    }

    // Face
    if (type[0] == 'f') {
      obj->num_triangles += 1;
      flag = fscanf(f, "%s", triangle_point);
      flag = fscanf(f, "%s", triangle_point);
      flag = fscanf(f, "%s", triangle_point);
    }

    type[0] = '\0'; type[1] = '\0';
    flag = fscanf(f, "%s", type);
  }

  return obj;

}

struct OBJ* OBJ(struct Vec3** positions,
                struct Vec3** normals,
                struct Triangle** triangles,
                int num_positions,
                int num_normals,
                int num_triangles
              ) {
  struct OBJ* newobj = (struct OBJ*) malloc(sizeof(OBJ));
  newobj->positions = positions;
  newobj->normals = normals;
  newobj->triangles = triangles;
  newobj->num_positions = num_positions;
  newobj->num_normals = num_normals;
  newobj->num_triangles = num_triangles;
  return newobj;
}
