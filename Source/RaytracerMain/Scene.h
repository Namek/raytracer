#pragma once

#include <vector>

#include "Geometry\Triangle.h"


namespace nprt
{

class Scene
{
	public:
		Scene();

		void LoadGeometry(const char* filename);

	private:
		std::vector<Triangle> m_Triangles;

		Point3d m_MaxDomain;
		Point3d m_MinDomain;
		Point3d m_DomainSize;
};


}