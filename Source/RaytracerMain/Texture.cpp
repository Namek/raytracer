#include "Texture.h"
#include "BaseUtils\BaseCore.h"

using namespace nprt;

Texture::Texture(int width, int height, TextureType::TextureTypes type) : m_Width(width), m_Height(height), m_Colors(NULL), m_NumColors(0)
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
	Utils::SafeDeleteArr(m_Colors);
}

double Texture::Ramp(double v, double min, double max) const
{
	double frac = v * (max - min) / max;

	if(frac < max)
	{
		return 0;
	}
		
	return 1;
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
	m_NumColors = 4;
	m_Colors = new Vector3d[m_NumColors];
	m_Colors[0].x = 0.8f;
	m_Colors[0].y = 0.75f;
	m_Colors[0].z = 0.6f;
	
	m_Colors[1].x = 0.5f;
	m_Colors[1].y = 0.4f;
	m_Colors[1].z = 0.3f;

	m_Colors[2].x = 0.4f;
	m_Colors[2].y = 0.2f;
	m_Colors[2].z = 0.1f;

	m_Colors[3].x = 0.2f;
	m_Colors[3].y = 0.1f;
	m_Colors[3].z = 0.0f;

	const double BrickWidth = 45;
	const double BrickHeight = 15;
	const double MortarThickness = 1;

	const double BMWidth = BrickWidth + MortarThickness;
	const double BMHeight = BrickHeight + MortarThickness;
	const double BorderWidth = MortarThickness / BMWidth;
	const double BorderHeight = MortarThickness / BMHeight;
		
	Texture turbulence(m_Width, m_Height, TextureType::Turbulence);

	for(int y = m_Height; y--;) 
	{
		double v = y / BMHeight; // Varies [0,1] over one brick
		int row = static_cast<int>(floor(v));
		v -= floor(v);
			
		double randomness = Noise1D(row) / 14;
		double shift = (row % 2 ? 0.5 + randomness : 0);
		for(int x = m_Width; x--;) 
		{
			double u = x / BMWidth; // Varies [0,1] over one brick
			u += shift;
			u -= floor(u);
				
			double w = Ramp(u, 0, BorderWidth + randomness) - Ramp(u, 1 - BorderWidth + randomness, 1);
			double h = Ramp(v, 0, BorderHeight + randomness) - Ramp(v, 1 - BorderHeight + randomness, 1);
			double B = 0.4f + w * h * turbulence.GetTexel(x, y).x;
			B *= B;

			Vector3d col;
			GetColor(B, col);
			m_Texels[x + y * m_Width] = col;
			//m_Texels[x + y * m_Width] = Vector3d((float)B, (float)B, (float)B);
		} 
	}
}

void Texture::GenerateTurbulence()
{
	double amplitudes[] = {1.6, 0.8, 0.4, 0.7, 0.1};
	for(int i = 0; i < m_Width; ++i)
	{
		for(int j = 0; j < m_Height; ++j)
		{
			const int texel = j * m_Width + i;
			const float color = TurbulentNoise3D(i, j, 0, amplitudes, 5);
			m_Texels[texel].x = color;
			m_Texels[texel].y = color;
			m_Texels[texel].z = color;
		}
	}
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

double Texture::SmoothNoise3D(double x, double y, double z) const
{
	double ret = 0;
	double xv = Noise3D(x, y, z);
	double x1 = Noise3D(x + 1, y, z);
	double x2 = Noise3D(x - 1, y, z);
	double y1 = Noise3D(x, y + 1, z);
	double y2 = Noise3D(x, y - 1, z);

	return (x1 + x2 + y1 + y2 + xv) / 5.0;
}

void Texture::GetColor(double u, Vector3d& in) const
{
	double colorStep = 1.0 / m_NumColors;
	int color = u / colorStep;
	int nextColor = (color + 1) % m_NumColors;

	const Vector3d& currColor = m_Colors[color];
	const Vector3d& destColor = m_Colors[nextColor];
	
	double currU = color * colorStep;
	double destU = nextColor * colorStep;
	double lerp = fabs((u - currU) / (destU - currU));

	in = currColor * (1.0f - lerp) + destColor * lerp;
}

double Texture::TurbulentNoise3D(double x, double y, double z, double* amplitudes, int no_of_octaves) const
{
	double noise = 0.0f;
	double freq = 1.0;
	for (int i = 0; i < no_of_octaves; i++ )
	{   
		noise += fabs( amplitudes[i] * SmoothNoise3D( x/freq, y/freq, z/freq )) / no_of_octaves;
		freq *= 2;
	}
	return noise;
}   
