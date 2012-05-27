#include "LightSource.h"


using namespace nprt;


LightSource::LightSource(float x, float y, float z, float r, float g, float b, float power)
{
	position = Vector3d(x, y, z);
	this->r = r;
	this->g = g;
	this->b = b;
	this->power = power;
}
