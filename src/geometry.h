#ifndef __GEOMETRY__H
#define __GEOMETRY__H

#include "common.h"

void load_geometry(const char* filename);

extern float** vertices;
extern int** triangles;
extern int* parts;

extern int verticesCount;
extern int trianglesCount;
extern int partsCount;

#endif