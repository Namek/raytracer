#include <cstdio>
#include <iostream>
#include <fstream>
#include "geometry.h"


void main(int argc, char *argv[])
{
	// Instead of using argv table, read params from defined file OR from params.txt file.
	std::string paramsFileName = "params.txt";
	if (argc > 0)
		paramsFileName = argv[0];

	std::ifstream paramsFile(paramsFileName);
	char params[1024];

	if (paramsFile.is_open())
	{
		memset(params, 0, sizeof(params));
		paramsFile.getline(params, sizeof(params));
		paramsFile.close();
	}


	// TODO interpret 'params' var

	// --

	// Load geometry
	load_geometry("data/room.BRS");


	// TODO raytracer :)
		// use global variables defined in geometry.h
	// --


	// Wait for input
	getchar();
}