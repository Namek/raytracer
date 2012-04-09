#include "LightSource.h"


using namespace nprt;


LightSource::LightSource(float x, float y, float z, float flux, float r, float g, float b)
{
	position = Vector3d(x, y, z);
	this->flux = flux;
	this->r = r;
	this->g = g;
	this->b = b;
}
