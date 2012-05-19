#include "Texture.h"
#include <FreeImage.h>

using namespace nprt;

Texture::Texture(const char* filename)
{
	FIBITMAP* dib = FreeImage_Load(FREE_IMAGE_FORMAT::FIF_PNG, filename);
	RGBQUAD rgb;
	this->width = FreeImage_GetWidth(dib);
	this->height = FreeImage_GetHeight(dib);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			FreeImage_GetPixelColor(dib, x, y, &rgb);
			this->pixels.push_back(Vector3d(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue));
		}
	}
}

char Texture::getGrayColor(int x, int y) const
{
	return static_cast<char>(pixels[y*height+x].x*255);
}

Vector3d& Texture::getPixel(int x, int y)
{
	return pixels[y*height+x];
}