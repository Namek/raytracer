#pragma once

#include <iostream>
#include <string>

//////////////////////////////////////////////////////////////////////////
// Wsp�czynniki oznaczaj�:
// kd  - wsp�czynnik rozpraszania powierzchni
// ks  - wsp�czynnik odbicia lustrzanego
// wg  - wsp�czynnik po�yskliwo�ci
// ka  - wsp�czynnik rozpraszania �wiat�a otaczaj�cego
// r   - sk�adowa czerowna
// g   - sk�adowa zielona
// b   - sk�adowa niebieska
// kt  - wsp�czynnik prze�roczysto�ci
// eta - stosunek pr�dko�ci �wiatla w pr�ni do pr�dko�ci �wiat�a w danym o�rodku
//////////////////////////////////////////////////////////////////////////
namespace nprt
{
	class Material
	{
		public:
			std::string name;
			int texture;
			float kdc;
			float ksc;
			float kt;
			float kac;
			float r, g, b;
			float wg, eta;

			Material()
				: kdc(0), ksc(0), kt(0), kac(0), r(0), g(0), b(0), wg(0), eta(1), texture(0)
			{	}

			Material(float _kd, float _ks, float _wg, float _ka, float _r, float _g, float _b, float _kt, float _eta, int _texture)
				: kdc(_kd), ksc(_ks), wg(_wg), kac(_ka), r(_r), g(_g), b(_b), kt(_kt), eta(_eta), texture(_texture)
			{	}
	};
}
