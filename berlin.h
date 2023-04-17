typedef struct {
    int num;
    float x;
    float y;
} Point;

float     distance           (Point p1, Point p2);

void      initMarque         (int* marque, int n);

void      escaladeSimple     (float* matriceCout, int* marque, int n);

void      escaladeComplete   (float* matriceCout, int* marque, int n);


