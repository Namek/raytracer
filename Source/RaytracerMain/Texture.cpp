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
	}currentX = 0;
}

Texture::~Texture()
{
	Utils::SafeDeleteArr(m_Texels);
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
	m_NumColors.push_back(4);
	m_ColorPalettes.push_back(std::vector<Vector3d>());
	m_ColorPalettes[0].push_back(Vector3d(0.8f, 0.75f, 0.6f));
	m_ColorPalettes[0].push_back(Vector3d(0.5f, 0.4f, 0.3f));
	m_ColorPalettes[0].push_back(Vector3d(0.4f, 0.2f, 0.1f));
	m_ColorPalettes[0].push_back(Vector3d(0.2f, 0.1f, 0.0f));

	m_NumColors.push_back(5);
	m_ColorPalettes.push_back(std::vector<Vector3d>());
	m_ColorPalettes[1].push_back(Vector3d(174 / 255.0f, 137 / 255.0f, 118 / 255.0f));
	m_ColorPalettes[1].push_back(Vector3d(0.8f, 0.75f, 0.6f));
	m_ColorPalettes[1].push_back(Vector3d(150 / 255.0f, 25 / 255.0f, 14 / 255.0f));
	m_ColorPalettes[1].push_back(Vector3d(150 / 255.0f, 22 / 255.0f, 11 / 255.0f));
	m_ColorPalettes[1].push_back(Vector3d(102 / 255.0f, 0, 0));

	const double BrickWidth = 45;
	const double BrickHeight = 15;
	const double MortarThickness = 1;

	const double BMWidth = BrickWidth + MortarThickness;
	const double BMHeight = BrickHeight + MortarThickness;
	const double BorderWidth = MortarThickness / BMWidth;
	const double BorderHeight = MortarThickness / BMHeight;
		
	Texture turbulence(m_Width, m_Height, TextureType::Turbulence);

	bool brickChange = false;
	int prevRow = -1;
	for(int y = m_Height; y--;) 
	{
		double v = y / BMHeight; // Varies [0,1] over one brick

		int row = static_cast<int>(floor(v));
		v -= floor(v);

		int palette = 0;
		double randomness = Noise1D(row) / 10;
		double shift = (row % 2 ? 0.5 + randomness : 0);
		
		for(int x = m_Width; x--;) 
		{
			double u = x / BMWidth; // Varies [0,1] over one brick

			u += shift;

			int column = static_cast<int>(floor(u));
			if (column % 2)
			{
				palette = (Noise3D(column, row, column + row) > 0.65f ? 1 : 0);
			}
			else palette = 0;

			u -= floor(u);

			double w = Ramp(u, 0, BorderWidth + randomness) - Ramp(u, 1 - BorderWidth + randomness, 1);
			double h = Ramp(v, 0, BorderHeight + randomness) - Ramp(v, 1 - BorderHeight + randomness, 1);
			double B = 0.4f + w * h * turbulence.GetTexel(x, y).x;
			B *= B;

			Vector3d col;
			GetColor(palette, B, col);
			m_Texels[x + y * m_Width] = col;
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
			const float color = static_cast<float>(TurbulentNoise3D(i, j, 0, amplitudes, 5));
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
	double xv = Noise3D(x		, y		, z);
	double x1 = Noise3D(x + 1	, y		, z);
	double x2 = Noise3D(x - 1	, y		, z);
	double y1 = Noise3D(x		, y + 1	, z);
	double y2 = Noise3D(x		, y - 1	, z);

	return (x1 + x2 + y1 + y2 + xv) / 5.0;
}

void Texture::GetColor(int palette, float u, Vector3d& in) const
{
	float colorStep = 1.0f / m_NumColors[palette];
	int color = u / colorStep;
	int nextColor = (color + 1) % m_NumColors[palette];

	const Vector3d& currColor = m_ColorPalettes[palette][color];
	const Vector3d& destColor = m_ColorPalettes[palette][nextColor];
	
	float currU = color * colorStep;
	float destU = nextColor * colorStep;
	float lerp = fabs((u - currU) / (destU - currU));

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
