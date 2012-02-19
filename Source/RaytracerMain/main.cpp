#include <conio.h>
#include <iostream>

#include "BaseUtils\BaseCore.h"
#include "Scene.h"

using namespace std;
using namespace Utils;
using namespace nprt;


int main(int argc, char* argv[])
{	
	char params[1024];
	std::string paramsFileName = "params.txt";
	std::ifstream paramsFile(paramsFileName);

	if (argc > 0)
		paramsFileName = argv[0];

	if (paramsFile.is_open())
	{
		memset(params, 0, sizeof(params));
		paramsFile.getline(params, sizeof(params));
		paramsFile.close();
	}

	// TODO Interpret parameters

	Scene sc;
	sc.LoadGeometry("data/room.brs");

	_getch();

	return 0;
}
