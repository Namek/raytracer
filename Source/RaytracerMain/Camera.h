#pragma once

#include "Geometry\Vector3d.h"

namespace nprt
{
	class Camera
	{
	public:
		Point3d cameraCenter;
		Point3d lookAt;
		float fovX, fovY;
		float rotation;
		Point3d topLeft;
		Point3d bottomLeft;
		Point3d topRight;
		int xResolution, yResolution;
		Camera(void);
		Camera(const Camera& copy) {
			fovX = copy.fovX;
			fovY = copy.fovY;
			cameraCenter = copy.cameraCenter;
			lookAt = copy.lookAt;
		}
		Camera(Point3d, Point3d, Point3d, Point3d, int, int);
		~Camera(void);
		Point3d getCameraCenter();
		void setCameraCenter(Point3d);
		void setTopLeft(Point3d);
		void setBottomLeft(Point3d);
		void setTopRight(Point3d);
		void setXResolution(int);
		void setYResolution(int);
		friend std::ostream& operator<<(std::ostream& os, const Camera& c) 
		{
			return os << c.cameraCenter << " " << c.topLeft << " " << c.bottomLeft << " " << c.topRight
				<< " " << c.xResolution << " " << c.yResolution << "\n";
		};
	};
}
