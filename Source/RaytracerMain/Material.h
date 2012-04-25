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
// kr  - ?
//////////////////////////////////////////////////////////////////////////
namespace nprt
{
	class Material
	{
		public:
			std::string name;
			float kdcR, kdcG, kdcB;
			float kscR, kscG, kscB;
			float krcR, krcG, krcB;
			float kacR, kacG, kacB;
			float r, g, b;
			float wg, eta;
			float kr;
			int G, N;

			Material() {	}
			Material(float _kd, float _ks, float _wg, float _ka, float _r, float _g, float _b, float _kt, float _eta, float _kr) 
				: kdcR(_kd), kscR(_ks), wg(_wg), kdcG(_ka), r(_r), g(_g), b(_b), krcR(_kt), eta(_eta), kr(_kr)
			{
			}
	};
}
