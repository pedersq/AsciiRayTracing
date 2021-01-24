struct Vec3 {
  float x;
  float y;
  float z;
};

struct Ray3 {
  struct Vec3* origin;
  struct Vec3* dir;
};

struct Ray3* Ray3(struct Vec3* start, struct Vec3* direction);

struct Vec3* Vec3(float a, float b, float c);

struct Vec3* addVec3(struct Vec3* s, struct Vec3* t);

struct Vec3* add3Vec3(struct Vec3* s, struct Vec3* t, struct Vec3* r);

// Subtracts t from s
struct Vec3* subVec3(struct Vec3* s, struct Vec3* t);

float dotVec3(struct Vec3* s, struct Vec3* t);

float lenVec3(struct Vec3* s);

float angleBetweenVec3(struct Vec3* s, struct Vec3* t);

struct Vec3* crossVec3(struct Vec3* s, struct Vec3* t);

// Returns itself
struct Vec3* normalize3(struct Vec3* s);

struct Vec3* negate3(struct Vec3* s);

struct Vec3* multiplyVec3(struct Vec3* s, float m);
