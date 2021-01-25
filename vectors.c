#include "vectors.h"
#include <math.h>
#include <stdlib.h>


struct Vec3* Vec3(float a, float b, float c) {
  struct Vec3* newvec = (struct Vec3*) malloc(sizeof(struct Vec3));
  newvec->x = a;
  newvec->y = b;
  newvec->z = c;
  return newvec;
}

struct Vec3* addVec3(struct Vec3* s, struct Vec3* t) {
  return Vec3(s->x + t->x,
              s->y + t->y,
              s->z + t->z
              );
}

struct Vec3* add3Vec3(struct Vec3* s, struct Vec3* t, struct Vec3* r) {
  return Vec3(s->x + t->x + r->x,
              s->y + t->y + r->y,
              s->z + t->z + r->z
              );
}



struct Vec3* subVec3(struct Vec3* s, struct Vec3* t) {
  return Vec3(s->x - t->x,
              s->y - t->y,
              s->z - t->z
              );
}


float dotVec3(struct Vec3* s, struct Vec3* t) {
  return s->x * t->x + s->y * t->y + s->z * t->z;
}

float lenVec3(struct Vec3* s) {
  return sqrtf(s->x * s->x + s->y * s->y + s->z * s->z);
}

float angleBetweenVec3(struct Vec3* s, struct Vec3* t) {
  return acosf(dotVec3(s, t) / (lenVec3(s) * lenVec3(t)));
}

struct Vec3* crossVec3(struct Vec3* s, struct Vec3* t) {
  float a = s->y * t->z - s->z * t->y;
  float b = s->z * t->x - s->x * t->z;
  float c = s->x * t->y - s->y * t->x;
  return Vec3(a, b, c);
}

struct Vec3* normalize3(struct Vec3* s) {
  float len = lenVec3(s);
  s->x = s->x / len;
  s->y = s->y / len;
  s->z = s->z / len;
  return s;
}

struct Vec3* negate3(struct Vec3* s) {
  return Vec3(-(s->x), -(s->y), -(s->z));
}

struct Vec3* multiplyVec3(struct Vec3* s, float m) {
  return Vec3(s->x * m, s->y * m, s->z * m);
}

struct Ray3* Ray3(struct Vec3* start, struct Vec3* direction) {
  struct Ray3* newray = (struct Ray3*) malloc(sizeof(struct Ray3));
  newray->origin = start;
  newray->dir = direction;
  return newray;
}
