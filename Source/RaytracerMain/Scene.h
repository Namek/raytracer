#pragma once

#include <vector>
#include <FreeImage.h>

#include "Material.h"
#include "Camera.h"
#include "Geometry\Octree.h"

namespace nprt
{

class Scene
{
	public:
		Scene();

		void LoadGeometry(const char* filename);
		void LoadAttributes(const char* filePath);
		void LoadCamera(const char* filename);
		void BuildOctree();
		void RenderToFile(const char* filename, int width, int height) const;

	private:
		std::vector<Triangle> m_Triangles;//deprecated: Octree will build it's own list of triangles (they will be cut)
		std::vector<Material> m_Materials;

		Octree m_Octree;
		Camera m_Camera;
};


}