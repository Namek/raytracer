#pragma once

#include "Geometry\Vector3d.h"

namespace nprt
{

namespace TextureType
{
	enum TextureTypes
	{
		LinearGradient = 0,
		Bricks,
		Turbulence
	};
}

class Texture
{
	public:
		Texture(int width, int height, TextureType::TextureTypes texTypes);

		inline const Vector3d& GetTexel(int x, int y) const { return m_Texels[x + y * m_Width];}
		inline const Vector3d& GetTexel(float u, float v) const { return GetTexel(u * m_Width, v * m_Height);}

		~Texture();

	private:
		Vector3d* m_Texels;
		int m_Width;
		int m_Height;

		double Ramp(double v, double min, double max) const;
		double Noise1D(int arg) const;
		double Noise3D(int ix, int iy, int iz) const;

		void GenerateBricks();
		void GenerateTurbulence();
		void GenerateGradient();
};

}