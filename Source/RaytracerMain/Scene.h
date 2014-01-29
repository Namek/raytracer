#pragma once

#include <vector>
#include <FreeImage.h>

#include "Material.h"
#include "LightSource.h"
#include "Camera.h"
#include "Geometry\Octree.h"
#include "Texture.h"

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
		void SetMaxDivideDepth(int maxDivideDepth) { m_Octree.setMaxDivideDepth(maxDivideDepth); }
		void RenderToFile(const char* filename, int width, int height) const;
		void RenderToFile(const char* filename) const;

		inline void SetEnableToneMapping(bool enable) {m_EnableToneMapping = enable; }
		inline void SetToneMappingKey(float key) {m_ToneMappingKey = key; }
		inline void SetGamma(float gamma) {m_Gamma = gamma; }
		inline void SetEnableGamma(bool enable) {m_EnableGamma = enable; }
		inline void SetEnableShadows(bool enable) {m_EnableShadows = enable; }
		inline void SetEnableRefraction(bool enable) {m_EnableRefraction = enable; }
		inline void SetEnableReflection(bool enable) {m_EnableReflection = enable; }
		inline void SetNumReflections(int reflections) {m_NumReflections = reflections; }

	private:
		void PerformToneMapping(Vector3d* pixels, const int numPixels) const;
		void PerformGammaCorrection(Vector3d* pixels, const int numPixels) const;
		void CalculateColor(const Vector3d& rayDirection, const Vector3d& observerPos, int numReflections, Vector3d& in_color) const;
		void CalculateRefractionComponent(nprt::Vector3d& in_Color, const nprt::Vector3d& intersectionPt, 
										const nprt::Vector3d& observerDir, const nprt::Triangle& hitTriangle, 
										const nprt::Material& material, int numReflections) const;
		void CalculateReflectionComponent(nprt::Vector3d& in_color, const nprt::Vector3d& intersectionPt, const nprt::Material& material,
										const nprt::Vector3d& reflectedRay, int numReflections) const;
		void ApplyMaterialColor(const Material& mat, Vector3d& in_color) const;
		void ApplyTexture(const Triangle& hitTriangle, const Vector3d& hitPoint, Vector3d& in_color) const;
		bool ManualRaytrace(const Vector3d& rayPos, const Vector3d& rayDir, std::pair<Triangle, AlignedVector3d>& out) const;

		std::vector<Triangle> m_Triangles;	//deprecated: Octree will build it's own list of triangles (they will be cut)
		std::vector<Triangle> m_ShadowTriangles;
		std::vector<Material> m_Materials;
		std::vector<LightSource> m_Lights;

		Octree m_Octree;
		mutable Texture m_WallTexture;
		mutable Camera m_Camera;

		int m_NumReflections;
		float m_ToneMappingKey;
		float m_Gamma;
		bool m_EnableGamma;
		bool m_EnableShadows;
		bool m_EnableToneMapping;
		bool m_EnableRefraction;
		bool m_EnableReflection;
};


}