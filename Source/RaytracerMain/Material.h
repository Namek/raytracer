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
