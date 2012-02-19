#pragma once

#include <vector>
#include <FreeImage.h>

#include "Geometry\Triangle.h"


namespace nprt
{

class Scene
{
	public:
		Scene();

		void LoadGeometry(const char* filename);
		void RenderToFile(const char* filename, int width, int height);

	private:
		std::vector<Triangle> m_Triangles;

		Point3d m_MaxDomain;
		Point3d m_MinDomain;
		Point3d m_DomainSize;
};


}