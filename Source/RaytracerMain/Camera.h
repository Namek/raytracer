#pragma once

#include "Geometry\Vector3d.h"

namespace nprt
{
	class Camera
	{
	public:
		Point3d cameraCenter;
		Point3d lookAt;
		float fovHorizontal, fovVertical;
		float rotation;
		Point3d topLeft;
		Point3d bottomLeft;
		Point3d topRight;
		Vector3d upDir, rightDir, forwardDir;
		int xResolution, yResolution;
		Camera(void);
		Camera(const Camera& copy) {
			fovHorizontal = copy.fovHorizontal;
			fovVertical = copy.fovVertical;
			cameraCenter = copy.cameraCenter;
			lookAt = copy.lookAt;
		}
		Camera(Point3d, Point3d, Point3d, Point3d, int, int);

		void initialize(Point3d cameraCenter, Point3d topLeft, Point3d bottomLeft, Point3d topRight, int xResolution, int yResolution);
		void initialize(const Vector3d& cameraCenter, const Vector3d& lookAt, float fovHorizontal);
		void setResolution(int width, int height);

		friend std::ostream& operator<<(std::ostream& os, const Camera& c) 
		{
			return os << c.cameraCenter << " " << c.topLeft << " " << c.bottomLeft << " " << c.topRight
				<< " " << c.xResolution << " " << c.yResolution << "\n";
		};
	};
}
