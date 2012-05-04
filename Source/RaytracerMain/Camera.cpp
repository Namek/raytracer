#include "Camera.h"
#include <cmath>

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

void Camera::initialize(const Vector3d& cameraCenter, const Vector3d& lookAt, float fovX)
{
	this->cameraCenter = cameraCenter;
	this->lookAt = lookAt;
	this->fovX = fovX;
	this->forwardDir = lookAt - cameraCenter;
	this->forwardDir.normalize();
	
	/*Vector3d axisY(0, 1, 0);
	float angle = acosf(axisY.dotProduct(lookDir));*/

	
}

void Camera::setResolution(int width, int height)
{
	const double PI = 3.14159265358979323846;
	const float RAD = PI / 180.0f;

	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	this->fovY = this->fovX / aspectRatio;
	
	// Determine pitch and yaw of look vector
	/*float pitch = atan(sqrt(forwardDir.x*forwardDir.x + forwardDir.y*forwardDir.y) / forwardDir.z);
	float yaw = atan(-forwardDir.x / forwardDir.y);

	this->upDir = Vector3d(0, 1, 0);
	upDir.rotateX(-pitch);
	upDir.rotateY(-yaw);

	this->rightDir = Vector3d(1, 0, 0);
	rightDir.rotateX(pitch);
	rightDir.rotateY(yaw);*/

	Vector3d left(forwardDir);
	//left.rotateY

	// TODO policzyc topLeft, topRight, bottomLeft na podstawie dir i fovX
}
