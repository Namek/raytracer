#include "Texture.h"
#include "BaseUtils\BaseCore.h"
#include "Geometry\Triangle.h"

using namespace nprt;

const double Texture::BrickWidth = 180;
const double Texture::BrickHeight = 60;
const double Texture::MortarThickness = 16;

const double Texture::BMWidth = BrickWidth + MortarThickness;
const double Texture::BMHeight = BrickHeight + MortarThickness;
const double Texture::BorderWidth = MortarThickness / BMWidth;
const double Texture::BorderHeight = MortarThickness / BMHeight;

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
		case TextureType::Ramp:
			GenerateRamp();
			break;
	}
	currentX = 0;
}

Texture::~Texture()
{
	Utils::SafeDeleteArr(m_Texels);
}

double Texture::Ramp(double v, double min, double max, double range) const
{	
	double frac = v * (max - min) / max;	

	if(frac > 1.0f - range
		|| frac < range)
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
	m_ColorPalettes[1].push_back(Vector3d(0.8f, 0.75f, 0.6f));
	m_ColorPalettes[1].push_back(Vector3d(174 / 255.0f, 137 / 255.0f, 118 / 255.0f));	
	m_ColorPalettes[1].push_back(Vector3d(150 / 255.0f, 25 / 255.0f, 14 / 255.0f));
	m_ColorPalettes[1].push_back(Vector3d(150 / 255.0f, 22 / 255.0f, 11 / 255.0f));
	m_ColorPalettes[1].push_back(Vector3d(102 / 255.0f, 0, 0));

	int numBricksHorz = (int)(m_Width / BMWidth);
	int numBricksVert = (int)(m_Height / BMHeight);
	Texture turbulence(m_Width, m_Height, TextureType::Turbulence);
		
	for(int y = 0; y < numBricksVert; ++y)
	{
		for(int x = 0; x < numBricksHorz; ++x)
		{
			DrawBrick((int)(x * BMWidth), (int)(y * BMHeight), turbulence);
		}
	}
}

void Texture::FillWithMortar(int x, int y, const Texture& turbulence)
{
	int maxY = static_cast<int>(y + BMHeight < m_Height ? y + BMHeight : m_Height);
	int maxX = static_cast<int>(x + BMWidth < m_Width ? x + BMWidth : m_Width);

	for(int row = y; row < maxY; ++row)
	{	
		float v = row / (float)BMHeight;
		int palette = 0;
		int brickRow = static_cast<int>(floor(v));
		double randomness = Noise1D(brickRow) / 10;
		double shift = (brickRow % 2 ? 0.5f + randomness : 0);		
		v -= floor(v);

		for(int col = x; col < maxX; ++col)
		{
			double u = col / (float)BMWidth;			

			u += shift;
			int column = static_cast<int>(floor(u));

			if (column % 2)
			{
				palette = (Noise3D(column, brickRow, column + brickRow) > 0.65f ? 1 : 0);
			}
			else 
			{
				palette = 0;
			}

			u -= floor(u);			

			double w = Ramp(u, 0, BorderWidth + randomness, BorderWidth);
			double h = Ramp(v, 0, BorderHeight + randomness, BorderHeight);

			if (w > std::numeric_limits<double>::epsilon()
				&&  h > std::numeric_limits<double>::epsilon())
			{
				w = h = 0;
			}

			double B = 0.4f + w * h * turbulence.GetTexel(col, row).x;
			B *= B;
			B = Utils::Clamp(B, 0.0, 0.999);			

			Vector3d color;
			GetColor(palette, B, color);
			m_Texels[col + row * m_Width] = color * (0.5f + turbulence.GetTexel(col, row).x);
		}
	}
}

void Texture::DrawBrick(int x, int y, const Texture& turbulence)
{
	FillWithMortar(x, y, turbulence);
	
	int maxY = static_cast<int>(y + BMHeight < m_Height ? y + BMHeight : m_Height);
	int maxX = static_cast<int>(x + BMWidth < m_Width ? x + BMWidth : m_Width);

	// Untransformed
	Vector3d p1(BorderWidth, BorderHeight, 0);
	Vector3d p2(1 - BorderWidth, BorderHeight, 0);
	Vector3d p3(1 - BorderWidth, 1 - BorderHeight, 0);
	Vector3d p4(0, 1 - BorderHeight, 0);

	// Transformed
	float angle = 10 - 20 * Noise3D(x, y, x + y);
	Vector3d p1t = rotateVec(p1, angle);
	Vector3d p2t = rotateVec(p2, angle);
	Vector3d p3t = rotateVec(p3, angle);
	Vector3d p4t = rotateVec(p4, angle);

	Triangle t1(p1t, p2t, p3t, 0);
	Triangle t2(p1t, p3t, p4t, 0);

	for(int row = y; row < maxY; ++row)
	{	
		float v = row / (float)BMHeight;
		int palette = 0;
		int brickRow = static_cast<int>(floor(v));
		double randomness = Noise1D(brickRow) / 10;
		double shift = (brickRow % 2 ? 0.5f + randomness : 0);		
		v -= floor(v);

		for(int col = x; col < maxX; ++col)
		{
			double u = col / (float)BMWidth;			

			u += shift;
			int column = static_cast<int>(floor(u));

			if (column % 2)
			{
				palette = (Noise3D(column, brickRow, column + brickRow) > 0.65f ? 1 : 0);
			}
			else 
			{
				palette = 0;
			}

			u -= floor(u);			

			// Check if the point is inside the diamond
			Vector3d point(u, v, 0);
			float uOut, vOut;
			bool pointInDiamond = false;
			t1.getUV(point, uOut, vOut);
			pointInDiamond |= (uOut + vOut < 1.0f && uOut > 0.0f && vOut > 0.0f);
			t2.getUV(point, uOut, vOut);
			pointInDiamond |= (uOut + vOut < 1.0f && uOut > 0.0f && vOut > 0.0f);

			point = rotateVec(point, -angle);

			if (pointInDiamond)
			{
				double w = Ramp(point.x, 0, BorderWidth + randomness, BorderWidth);
				double h = Ramp(point.y, 0, BorderHeight + randomness, BorderHeight);
				double B = 0.3f + w * h * turbulence.GetTexel(col, row).x;
				B *= B;
				B = Utils::Clamp(B, 0.0, 0.999);

				Vector3d color;
				GetColor(palette, B, color);
				m_Texels[col + row * m_Width] = color * (0.5f + turbulence.GetTexel(col, row).x);
			}
		}
	}
}

void Texture::GenerateTurbulence()
{
	double amplitudes[] = {1.6, 1.2, 0.8, 0.4, 0.7, 0.1};
	for(int i = 0; i < m_Width; ++i)
	{
		for(int j = 0; j < m_Height; ++j)
		{
			const int texel = j * m_Width + i;
			const float color = static_cast<float>(TurbulentNoise3D(i, j, 0, amplitudes, 6));
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
	double xv = Noise3D((int)x		, (int)y		, (int)z);
	double x1 = Noise3D((int)x + 1	, (int)y		, (int)z);
	double x2 = Noise3D((int)x - 1	, (int)y		, (int)z);
	double y1 = Noise3D((int)x		, (int)y + 1	, (int)z);
	double y2 = Noise3D((int)x		, (int)y - 1	, (int)z);

	return (x1 + x2 + y1 + y2 + xv) / 5.0;
}

void Texture::GetColor(int palette, double u, Vector3d& in) const
{
	double colorStep = 1.0f / m_NumColors[palette];
	int color = (int)(u / colorStep);

	if (color != 0)
	{
		int nextColor = (color + 1) % m_NumColors[palette];

		const Vector3d& currColor = m_ColorPalettes[palette][color];
		const Vector3d& destColor = m_ColorPalettes[palette][nextColor];
	
		double currU = color * colorStep;
		double destU = nextColor * colorStep;
		double lerp = fabs((u - currU) / (destU - currU));

		in = currColor * static_cast<float>(1.0f - lerp) + destColor * static_cast<float>(lerp);
	}
	else
	{
		in = m_ColorPalettes[palette][color];
	}
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

void Texture::GenerateRamp()
{
	for(int i = 0; i < m_Width; ++i)
	{
		for(int j = 0; j < m_Height; ++j)
		{
			const int texel = j * m_Width + i;			
			float ramp = (float)Ramp(static_cast<float>(i) / static_cast<float>(m_Width), 0.0f, 1.0f, 0.1f);
			m_Texels[texel].x = ramp;
			m_Texels[texel].y = ramp;
			m_Texels[texel].z = ramp;
		}
	}
}

Vector3d Texture::rotateVec(Vector3d vec, float angle) const
{
	Vector3d out(0, 0, 0);
	angle *= 3.14 / 180.0f;
	out.x = (vec.x - 0.5f) * cosf(angle) - (vec.y - 0.5f) * sinf(angle);
	out.y = (vec.x - 0.5f) * sinf(angle) + (vec.y - 0.5f) * cosf(angle);

	out.x += 0.5f;
	out.y += 0.5f;
	return out;
}
