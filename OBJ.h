struct OBJ {

  struct Vec3** positions;
  struct Vec3** normals;
  struct Triangle** triangles;

  int num_positions;
  int num_normals;
  int num_triangles;


};

// Stores indicies into a obj positions array and normal array
struct Triangle {
  int p1;
  int p2;
  int p3;
  int n1;
  int n2;
  int n3;
};


struct OBJ* load_obj(char* filename);

struct OBJ* OBJ(struct Vec3** positions,
                struct Vec3** normals,
                struct Triangle** triangles,
                int num_positions,
                int num_normals,
                int num_triangles
              );
