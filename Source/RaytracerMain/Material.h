#pragma once

#include <iostream>
#include <string>

//////////////////////////////////////////////////////////////////////////
// Wspó³czynniki oznaczaj¹:
// kd  - wspó³czynnik rozpraszania powierzchni
// ks  - wspó³czynnik odbicia lustrzanego
// wg  - wspó³czynnik po³yskliwoœci
// ka  - wspó³czynnik rozpraszania œwiat³a otaczaj¹cego
// r   - sk³adowa czerowna
// g   - sk³adowa zielona
// b   - sk³adowa niebieska
// kt  - wspó³czynnik przeŸroczystoœci
// eta - stosunek prêdkoœci œwiatla w pró¿ni do prêdkoœci œwiat³a w danym oœrodku
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
