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
	cout << "Scene geometry loaded successfully." << endl;
	scene.LoadAttributes("data/attributes.atr");
	cout << "Geometry attributes loaded successfully." << endl;
	scene.LoadCamera("data/camera.cam");

	const std::string filename("test.png");
	cout << "Rendering scene to file " + filename + "..." << endl;
	scene.RenderToFile(filename.c_str(), params.GetInt("width", 320), params.GetInt("height", 240));

	_getch();

	return 0;
}
