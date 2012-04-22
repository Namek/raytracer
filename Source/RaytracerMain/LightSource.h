#pragma once

#include "Geometry\Vector3d.h"


namespace nprt
{
	class LightSource
	{	
		public:
			Vector3d position;
			float flux;
			float r, g, b;
			LightSource(void) {		};
			LightSource(float x, float y, float z, float flux, float r, float g, float b);
			~LightSource(void) {	};

			friend std::ostream& operator<<(std::ostream& os, const LightSource& l) 
			{
				return os << l.position << l.flux << " " << l.r << " " << l.g << " " << l.b << "\n";
			};
	};
}
