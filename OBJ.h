struct OBJ {

  struct OBJVec3* positions;
  struct OBJVec3* normals;
  struct OBJTriangle* triangles;

  int num_positions;
  int num_normals;
  int num_triangles;


};

// Stores indicies into a obj positions array and normal array
struct OBJTriangle {
  int p1;
  int p2;
  int p3;
  int n1;
  int n2;
  int n3;
  struct Vec3* position1;
  struct Vec3* position2;
  struct Vec3* position3;
  struct Vec3* normal1;
  struct Vec3* normal2;
  struct Vec3* normal3;
  struct OBJTriangle* next;
};

struct OBJVec3 {
  struct Vec3* vec;
  struct OBJVec3* next;
};

void print_vec3(struct Vec3* vec);


void addTriangle(struct OBJTriangle* addition, struct OBJ* object);
void addPosition(struct OBJVec3* addition, struct OBJ* object);
void addNormal(struct OBJVec3* addition, struct OBJ* object);

struct Vec3* plane_normal(struct OBJTriangle* triangle);

struct OBJVec3* OBJVec3(struct Vec3* input);

struct OBJTriangle* OBJTriangle(int i1, int i2, int i3, int i4, int i5, int i6);

struct OBJ* load_obj(char* filename);

struct OBJ* OBJ(struct OBJVec3* positions,
                struct OBJVec3* normals,
                struct OBJTriangle* triangles,
                int count_positions,
                int count_normals,
                int count_triangles
              );
