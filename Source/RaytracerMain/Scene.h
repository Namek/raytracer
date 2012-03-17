#pragma once

#include <vector>
#include <FreeImage.h>

#include "Geometry\Triangle.h"
#include "Material.h"
#include "Camera.h"

namespace nprt
{

class Scene
{
	public:
		Scene();

		void LoadGeometry(const char* filename);
		void LoadAttributes(const char* filePath);
		void LoadCamera(const char* filename);
		void RenderToFile(const char* filename, int width, int height);

	private:
		std::vector<Triangle> m_Triangles;
		std::vector<Material> m_Materials;

		Point3d m_MaxDomain;
		Point3d m_MinDomain;
		Point3d m_DomainSize;
		Camera m_Camera;
};


}