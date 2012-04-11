#include <conio.h>
#include <iostream>
#include <Windows.h>

#include "BaseUtils\BaseCore.h"
#include "Scene.h"
#include "BaseUtils\Params.h"

using namespace std;
using namespace Utils;
using namespace nprt;

int main(int argc, char* argv[])
{
	Params params(argc > 1 ? argv[1] : "params.txt");
	int numScenes = params.GetInt("num_scenes");
	unsigned totalTime = 0;

	Scene scene;
	scene.LoadGeometry((std::string("data/") + params.GetString("geometry_name") + ".brs").c_str());
	cout << "Scene geometry loaded successfully." << endl << endl;

	for(int i = 0; i < numScenes; ++i)
	{
		cout << "Loading scene " << (i + 1) << "..." << endl;
		std::string dataFolderFilename = params.GetString(("data_folder" + NumToStr(i)).c_str());

		scene.LoadAttributes(("data/" + dataFolderFilename + "/" + dataFolderFilename + ".atr").c_str());
		cout << "Geometry attributes loaded successfully." << endl;
		scene.LoadCamera(("data/" + dataFolderFilename + "/" + dataFolderFilename + ".cam").c_str());
		cout << "Camera loaded successfully." << endl;
		scene.LoadLights(("data/" + dataFolderFilename + "/" + dataFolderFilename + ".lgt").c_str());
		cout << "Lights loaded successfully." << endl;

		unsigned start_tm = timeGetTime();

		const std::string filename((dataFolderFilename + ".png").c_str());
		cout << "Rendering scene to file " << filename << "..." << endl;
		scene.RenderToFile(filename.c_str(), params.GetInt("width", 320), params.GetInt("height", 240));
	
		totalTime += timeGetTime() - start_tm;
		cout << "Rendering time: " << (timeGetTime() - start_tm) / 1000.0f << " seconds." << endl << endl;
	}

	cout << "Rendered " << numScenes << " scenes in " << totalTime / 1000.0f << " seconds" << endl;
	_getch();

	return 0;
}
