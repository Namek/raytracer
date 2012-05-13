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
	int numScenes = params.GetInt("num_scenes", 0);
	bool newDataFormat = params.GetBool("use_new_format", true);
	unsigned totalTime = 0;
	
	Scene scene;
	scene.SetToneMappingKey(params.GetFloat("tone_mapping_key"));
	scene.SetGamma(params.GetFloat("gamma"));
	scene.SetEnableGamma(params.GetBool("enable_gamma", true));
	scene.SetEnableShadows(params.GetBool("enable_shadows", false));
	scene.SetEnableRefraction(params.GetBool("enable_refraction", false));
	scene.SetEnableReflection(params.GetBool("enable_reflection", false));
	scene.SetEnableToneMapping(params.GetBool("enable_tone_mapping", true));
	scene.SetNumReflections(params.GetInt("num_reflections", 0));
	scene.SetMaxDivideDepth(params.GetInt("max_divide_depth", 0));

	if (!newDataFormat)
	{
		scene.LoadGeometry((std::string("data/") + params.GetString("geometry_name") + ".brs").c_str());
		cout << "Scene geometry loaded successfully." << endl << endl;
	}

	for(int i = 0; i < numScenes; ++i)
	{
		cout << "Loading scene " << (i + 1) << "..." << endl;
		std::string dataFolderFilename = params.GetString(("data_folder" + NumToStr(i)).c_str());

		if (!newDataFormat)
		{
			scene.LoadAttributes(("data/" + dataFolderFilename + "/" + dataFolderFilename + ".atr").c_str());
			cout << "Geometry attributes loaded successfully." << endl;
			scene.LoadCamera(("data/" + dataFolderFilename + "/" + dataFolderFilename + ".cam").c_str());
			cout << "Camera loaded successfully." << endl;
			scene.LoadLights(("data/" + dataFolderFilename + "/" + dataFolderFilename + ".lgt").c_str());
			cout << "Lights loaded successfully." << endl;
		}
		else
		{
			std::string geometryName = params.GetString("geometry_name");
			std::string filename = geometryName + "/" + geometryName + "-" + dataFolderFilename + ".scn";
			scene.LoadScene((std::string("data_new/") + filename).c_str());
		}

		unsigned start_tm = timeGetTime();

		const std::string filename((dataFolderFilename + ".png").c_str());
		cout << "Rendering scene to file " << filename << "..." << endl;
		scene.RenderToFile(filename.c_str(), params.GetInt("width", 0), params.GetInt("height", 0));
	
		totalTime += timeGetTime() - start_tm;
		cout << "Rendering time: " << (timeGetTime() - start_tm) / 1000.0f << " seconds." << endl << endl;
	}

	cout << "Rendered " << numScenes << " scenes in " << totalTime / 1000.0f << " seconds" << endl;
	_getch();

	return 0;
}
