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

		inline const int GetWidth() const { return m_Width; }
		inline const int GetHeight() const { return m_Height; }
		inline const Vector3d& GetTexel(int x, int y) const { return m_Texels[x + y * m_Width];}
		inline const Vector3d& GetTexel(float u, float v) const 
		{ 
			return GetTexel(static_cast<int>((u - floor(u)) * m_Width), 
							static_cast<int>((v - floor(v)) * m_Height));
		}

		~Texture();

	private:
		Vector3d* m_Colors;
		Vector3d* m_Texels;
		int m_Width;
		int m_Height;
		int m_NumColors;

		double Ramp(double v, double min, double max) const;
		double Noise1D(int arg) const;
		double Noise3D(int ix, int iy, int iz) const;
		double SmoothNoise3D(double x, double y, double z) const;
		void GetColor(double u, Vector3d& in) const;
		double TurbulentNoise3D(double x, double y, double z, double* amplitudes, int no_of_octaves) const;

		void GenerateBricks();
		void GenerateTurbulence();
		void GenerateGradient();
};

}