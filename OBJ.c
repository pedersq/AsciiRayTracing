#include "OBJ.h"
#include "vectors.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void remove_slashes(char* line, int len);
void populate_triangles(struct OBJ* obj);
struct Vec3* get_position(int idx, struct OBJ* obj);
struct Vec3* get_normal(int idx, struct OBJ* obj);

// filename must be null terminated
struct OBJ* load_obj(char* filename) {

  printf("loading %s\n", filename);

  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    printf("Couldn't open file at: %s\n", filename);
  }


  struct OBJ* obj = OBJ(NULL, NULL, NULL, 0, 0, 0);

  int flag = 0;
  char type[2];
  char triangle_point[30];
  float f1;
  float f2;
  float f3;
  int i1, i2, i3;
  int t1, t2, t3;
  int n1, n2, n3;
  flag = fscanf(f, "%s", type);
  while (flag != EOF) {
    // Texture
    if (type[0] == 'v' && type[1] == 't') {
      flag = fscanf(f, "%d %d", &i1, &i2);
    }

    // Normal
    if (type[0] == 'v' && type[1] == 'n') {
      flag = fscanf(f, "%f %f %f", &f1, &f2, &f3);
      struct Vec3* normal = Vec3(f1, f2, f3);
      struct OBJVec3* objvec3 = OBJVec3(normal);
      addNormal(objvec3, obj);
    }

    // Position
    if (type[0] == 'v' && type[1] == '\0') {
      flag = fscanf(f, "%f %f %f", &f1, &f2, &f3);
      struct Vec3* position = Vec3(f1, f2, f3);
      struct OBJVec3* objvec3 = OBJVec3(position);
      addPosition(objvec3, obj);
    }

    // Face
    if (type[0] == 'f') {
      flag = fscanf(f, "%s", triangle_point);
      remove_slashes(triangle_point, 30);
      sscanf(triangle_point, "%d %d %d", &i1, &t1, &n1);

      flag = fscanf(f, "%s", triangle_point);
      remove_slashes(triangle_point, 30);
      sscanf(triangle_point, "%d %d %d", &i2, &t2, &n2);

      flag = fscanf(f, "%s", triangle_point);
      remove_slashes(triangle_point, 30);
      sscanf(triangle_point, "%d %d %d", &i3, &t3, &n3);

      struct OBJTriangle* tri = OBJTriangle(i1, i2, i3, n1, n2, n3);
      addTriangle(tri, obj);
    }


    type[0] = '\0'; type[1] = '\0';
    flag = fscanf(f, "%s", type);
  }

  populate_triangles(obj);
  return obj;

}

void remove_slashes(char* line, int len) {
  for (int i = 0; i < len; i++) {
    if (line[i] == '/') {
      line[i] = ' ';
    }
  }
}

void populate_triangles(struct OBJ* obj) {
  struct OBJTriangle* triangle = obj->triangles;
  while (triangle != NULL) {
    triangle->position1 = get_position(triangle->p1, obj);
    triangle->position2 = get_position(triangle->p2, obj);
    triangle->position3 = get_position(triangle->p3, obj);
    triangle->normal1 = get_normal(triangle->n1, obj);
    triangle->normal2 = get_normal(triangle->n2, obj);
    triangle->normal3 = get_normal(triangle->n3, obj);
    triangle = triangle->next;
  }
}

struct Vec3* get_position(int idx, struct OBJ* obj) {
    struct OBJVec3* curr = obj->positions;
    for (int i = 1; i <= idx; i++) {
      if (i == idx) {
        return curr->vec;
      }
      curr = curr->next;
    }
    printf("Couldn't find position idx\n");
    exit(EXIT_FAILURE);
    return NULL;
}

struct Vec3* get_normal(int idx, struct OBJ* obj) {
    struct OBJVec3* curr = obj->normals;
    for (int i = 1; i <= idx; i++) {
      if (i == idx) {
        return curr->vec;
      }
      curr = curr->next;
    }
    printf("Couldn't find position idx\n");
    exit(EXIT_FAILURE);
    return NULL;
}


struct OBJ* OBJ(struct OBJVec3* pos,
                struct OBJVec3* norms,
                struct OBJTriangle* tris,
                int count_positions,
                int count_normals,
                int count_triangles
              ) {
  struct OBJ* newobj = (struct OBJ*) malloc(sizeof(struct OBJ));
  newobj->positions = pos;
  newobj->normals = norms;
  newobj->triangles = tris;
  newobj->num_positions = count_positions;
  newobj->num_normals = count_normals;
  newobj->num_triangles = count_triangles;
  return newobj;
}

struct OBJTriangle* OBJTriangle(int i1, int i2, int i3, int i4, int i5, int i6) {
  struct OBJTriangle* tri = (struct OBJTriangle*) malloc(sizeof(struct OBJTriangle));
  tri->p1 = i1;
  tri->p2 = i2;
  tri->p3 = i3;
  tri->n1 = i4;
  tri->n2 = i5;
  tri->n3 = i6;
  tri->position1 = NULL;
  tri->position2 = NULL;
  tri->position3 = NULL;
  tri->normal1 = NULL;
  tri->normal2 = NULL;
  tri->normal3 = NULL;
  tri->next = NULL;
  return tri;
}

struct Vec3* plane_normal(struct OBJTriangle* triangle) {
  struct Vec3* AB = subVec3(triangle->position2, triangle->position1);
  struct Vec3* AC = subVec3(triangle->position3, triangle->position1);
  free(AB);
  free(AC);
  struct Vec3* normal = crossVec3(AB, AC);
}


struct OBJVec3* OBJVec3(struct Vec3* input) {
  struct OBJVec3* objvec = (struct OBJVec3*) malloc(sizeof(struct OBJVec3));
  objvec->vec = input;
  objvec->next = NULL;
}

void addTriangle(struct OBJTriangle* addition, struct OBJ* object) {
  if (object->triangles == NULL) {
    object->triangles = addition;
  } else {
    struct OBJTriangle* curr = object->triangles;
    while (curr->next != NULL) {
      curr = curr->next;
    }
    curr->next = addition;
  }

  object->num_triangles += 1;
}

void addPosition(struct OBJVec3* addition, struct OBJ* object) {
  if (object->positions == NULL) {
    object->positions = addition;
  } else {
    struct OBJVec3* curr = object->positions;
    while (curr->next != NULL) {
      curr = curr->next;
    }
    curr->next = addition;
  }

  object->num_positions += 1;
}

void addNormal(struct OBJVec3* addition, struct OBJ* object) {
  if (object->normals == NULL) {
    object->normals = addition;
  } else {
    struct OBJVec3* curr = object->normals;
    while (curr->next != NULL) {
      curr = curr->next;
    }
    curr->next = addition;
  }

  object->num_normals += 1;
}
