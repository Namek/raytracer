#include <conio.h>
#include <iostream>

#include "BaseUtils\BaseCore.h"
#include "Scene.h"

using namespace std;
using namespace Utils;
using namespace nprt;


int main(int, char* )
{
	Scene sc;
	sc.LoadGeometry("data/room.brs");
	

	_getch();

	return 0;
}
