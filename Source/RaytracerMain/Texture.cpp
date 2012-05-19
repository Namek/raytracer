#include "Texture.h"
#include "BaseUtils\BaseCore.h"

using namespace nprt;

Texture::Texture(int width, int height, TextureType::TextureTypes type) : m_Width(width), m_Height(height)
{
	m_Texels = new Vector3d[m_Width * m_Height];
	memset(m_Texels, 0, sizeof(Vector3d) * m_Width * m_Height);

	switch(type)
	{
		case TextureType::LinearGradient:
			GenerateGradient();
			break;
		case TextureType::Bricks:
			GenerateBricks();
			break;
		case TextureType::Turbulence:
			GenerateTurbulence();
			break;
	}
}

Texture::~Texture()
{
	Utils::SafeDeleteArr(m_Texels);
}

double Texture::Ramp(double v, double min, double max) const
{
	const float Threshold = 0.1f;
	double frac = v * (max - min) / max;

	if(frac < Threshold)
	{
		return 0;
	}
	else if(frac > 1.0f - Threshold)
	{
		return 1.0f;
	}
	else
	{
		return 1;
	}
}

double Texture::Noise1D(int x) const
{
	x = (x<<13) ^ x;
    return ( ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 2147483648.0); 
}

double Texture::Noise3D(int ix, int iy, int iz) const		 
{
   int sz, sy; 

   sy = int(256 * Noise1D( ix ));
   sz = int(256 * Noise1D( iy + sy ));
   return ( Noise1D( iz + sz ) );
}


void Texture::GenerateBricks()
{
	const double BrickWidth = 45;
	const double BrickHeight = 15;
	const double MortarThickness = 1.5;

	const double BMWidth = BrickWidth + MortarThickness;
	const double BMHeight = BrickHeight + MortarThickness;
	const double BorderWidth = MortarThickness / BMWidth;
	const double BorderHeight = MortarThickness / BMHeight;
		
	for(int y = m_Height; y--;) 
	{
		double v = y / BMHeight; // Varies [0,1] over one brick
		int row = static_cast<int>(floor(v));
		v -= floor(v);
			
		double randomness = Noise1D(row) / 3;
		double shift = (row % 2 ? 0.5 + randomness : 0);
		for(int x = m_Width; x--;) 
		{
			double u = x / BMWidth; // Varies [0,1] over one brick
			u += shift;
			u -= floor(u);
				
			double w = Ramp(u, 0, BorderWidth + randomness) - Ramp(u, 1 - BorderWidth + randomness, 1);
			double h = Ramp(v, 0, BorderHeight + randomness) - Ramp(v, 1 - BorderHeight + randomness, 1);
			double B = 1.0f - w * h;
				
			m_Texels[x + y * m_Width] = Vector3d((float)B, (float)B, (float)B);
		} 
	}
}

void Texture::GenerateTurbulence()
{

}

void Texture::GenerateGradient()
{
	for(int i = 0; i < m_Width; ++i)
	{
		for(int j = 0; j < m_Height; ++j)
		{
			const int texel = j * m_Width + i;
			const float color = static_cast<float>(i) / static_cast<float>(m_Width);
			m_Texels[texel].x = color;
			m_Texels[texel].y = color;
			m_Texels[texel].z = color;
		}
	}
}
