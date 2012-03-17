#include <conio.h>
#include <iostream>

#include "BaseUtils\BaseCore.h"
#include "Scene.h"
#include "BaseUtils\Params.h"

using namespace std;
using namespace Utils;
using namespace nprt;


int main(int argc, char* argv[])
{
	Params params = Params(argc > 1 ? argv[1] : "params.txt");
	
	Scene scene;
	scene.LoadGeometry("data/room.brs");
	scene.LoadAttributes("data/attributes.atr");
	scene.LoadCamera("data/camera.cam");

	scene.RenderToFile("test.png", params.GetInt("width", 320), params.GetInt("height", 240));

	_getch();

	return 0;
}
