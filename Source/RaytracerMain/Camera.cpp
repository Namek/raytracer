#include "Camera.h"
#include <cmath>

using namespace nprt;


Camera::Camera(void)
{
	xResolution = -1;
	yResolution = -1;
}

Camera::Camera(const Vector3d& cameraCenter, const Vector3d& topLeft, const Vector3d& bottomLeft, const Vector3d& topRight, int xResolution, int yResolution)
{
	initialize(cameraCenter, topLeft, bottomLeft, topRight, xResolution, yResolution);
}

void Camera::initialize(const Vector3d& cameraCenter, const Vector3d& topLeft, const Vector3d& bottomLeft, const Vector3d& topRight, int xResolution, int yResolution)
{
	this->cameraCenter = cameraCenter;
	this->topLeft = topLeft;
	this->bottomLeft = bottomLeft;
	this->topRight = topRight;
	this->xResolution = xResolution;
	this->yResolution = yResolution;

	Vector3d U = topRight - topLeft;
	Vector3d V = bottomLeft - topLeft;
	Vector3d ul = topLeft;
	Vector3d P_ij = ul + U * 0.5f + V * 0.5f;
	this->forwardDir = P_ij - cameraCenter;
	this->forwardDir.normalize();
}

void Camera::initialize(const Vector3d& cameraCenter, const Vector3d& lookAt, float fovHorizontal)
{	
	const double PI = 3.14159265358979323846;
	const double RAD = PI / 180.0f;

	this->cameraCenter = cameraCenter;
	this->lookAt = lookAt;
	this->fovHorizontal = static_cast<float>(fovHorizontal * RAD);
	this->forwardDir = lookAt - cameraCenter;
	this->forwardDir.normalize();
	
	// Assume aspect ratio 4:3
	this->fovVertical = this->fovHorizontal / 1;//(4.0/3.0f);
}

void Camera::setResolution(int /*width*/, int /*height*/)
{
	////float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	this->fovVertical = this->fovHorizontal / 1;//aspectRatio;
	
	Vector3d bottomLeftDir(forwardDir);
	bottomLeftDir.rotateX(fovVertical/2);
	bottomLeftDir.rotateY(-fovHorizontal/2);
	this->bottomLeft = cameraCenter + bottomLeftDir;

	Vector3d topLeftDir(forwardDir);
	topLeftDir.rotateX(-fovVertical/2);
	topLeftDir.rotateY(-fovHorizontal/2);
	this->topLeft = cameraCenter + topLeftDir;

	Vector3d topRightDir(forwardDir);
	topRightDir.rotateX(-fovVertical/2);
	topRightDir.rotateY(fovHorizontal/2);
	this->topRight = cameraCenter + topRightDir;
}
