#pragma once

#include <vector>
#include <FreeImage.h>

#include "Material.h"
#include "LightSource.h"
#include "Camera.h"
#include "Geometry\Octree.h"

namespace nprt
{

class Scene
{
	public:
		Scene();

		void LoadScene(const char* filename);
		void LoadGeometry(const char* filename);
		void LoadAttributes(const char* filePath);
		void LoadCamera(const char* filename);
		void LoadLights(const char* filename);
		void BuildOctree();
		void RenderToFile(const char* filename, int width, int height) const;

		inline void SetEnableToneMapping(bool enable) {m_EnableToneMapping = enable; }
		inline void SetToneMappingKey(float key) {m_ToneMappingKey = key; }
		inline void SetGamma(float gamma) {m_Gamma = gamma; }
		inline void SetEnableGamma(bool enable) {m_EnableGamma = enable; }
		inline void SetEnableShadows(bool enable) {m_EnableShadows = enable; }

	private:
		void PerformToneMapping(Vector3d* pixels, const int numPixels) const;
		void PerformGammaCorrection(Vector3d* pixels, const int numPixels) const;
		void CalculateColor(const Vector3d& rayDirection, const Vector3d& observerPos, Vector3d& in_color) const;

		std::vector<Triangle> m_Triangles;	//deprecated: Octree will build it's own list of triangles (they will be cut)
		std::vector<Triangle> m_ShadowTriangles;
		std::vector<Material> m_Materials;
		std::vector<LightSource> m_Lights;

		Octree m_Octree;
		mutable Camera m_Camera;

		float m_ToneMappingKey;
		float m_Gamma;
		bool m_EnableGamma;
		bool m_EnableShadows;
		bool m_EnableToneMapping;
};


}