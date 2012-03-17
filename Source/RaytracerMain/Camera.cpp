#include "Camera.h"

using namespace nprt;


Camera::Camera(void)
{
	xResolution = -1;
	yResolution = -1;
}


Camera::~Camera(void)
{
	cameraCenter = Point3d();
	topLeft = Point3d();
	topRight = Point3d();
	bottomLeft = Point3d();
	fovX = 0;
	fovY = 0;
	rotation = 0;
	xResolution = 0;
	yResolution = 0;
}

Camera::Camera(Point3d cameraCenter, Point3d topLeft, Point3d bottomLeft, Point3d topRight, int xResolution, int yResolution)
{
	this->cameraCenter = cameraCenter;
	this->topLeft = topLeft;
	this->bottomLeft = bottomLeft;
	this->topRight = topRight;
	this->xResolution = xResolution;
	this->yResolution = yResolution;
}
