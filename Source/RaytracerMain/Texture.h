#pragma once

#include "Geometry\Vector3d.h"
#include <vector>

namespace nprt
{
	class Texture
	{
	public:
		// Reads PNG image format
		Texture(const char* filename);

		inline char getGrayColor(int x, int y) const;
		inline Vector3d& getPixel(int x, int y);
	
		int width;
		int height;
		std::vector<Vector3d> pixels;
	};
}