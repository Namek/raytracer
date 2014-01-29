#pragma once

#include "Geometry\Vector3d.h"

namespace nprt
{
	class Camera
	{
	public:
		Vector3d cameraCenter;
		Vector3d lookAt;
		float fovHorizontal, fovVertical;
		float rotation;
		Vector3d topLeft;
		Vector3d bottomLeft;
		Vector3d topRight;
		Vector3d upDir, rightDir, forwardDir;
		int xResolution, yResolution;
		Camera(void);
		Camera(const Camera& copy) 
		{
			fovHorizontal = copy.fovHorizontal;
			fovVertical = copy.fovVertical;
			cameraCenter = copy.cameraCenter;
			lookAt = copy.lookAt;
		}
		Camera(const Vector3d&, const Vector3d&, const Vector3d&, const Vector3d&, int, int);

		void initialize(const Vector3d& cameraCenter, const Vector3d& topLeft, const Vector3d& bottomLeft, const Vector3d& topRight, int xResolution, int yResolution);
		void initialize(const Vector3d& cameraCenter, const Vector3d& lookAt, float fovHorizontal);
		void setResolution(int width, int height);

		friend std::ostream& operator<<(std::ostream& os, const Camera& c) 
		{
			return os << c.cameraCenter << " " << c.topLeft << " " << c.bottomLeft << " " << c.topRight
				<< " " << c.xResolution << " " << c.yResolution << "\n";
		};
	};
}
