#include <iostream>
#include <fstream>
#include <sstream>
#include "geometry.h"

// Global data
float** vertices;
int** triangles;
int* parts;

int verticesCount;
int trianglesCount;
int partsCount;// may be needed. NOT!!

// When loading
enum CurrentDataType {
	Vertices = 1,
	Triangles = 2,
	Parts = 3
};

CurrentDataType currentDataType;
int currentVertexIndex = 0, currentTriangleIndex = 0, currentPartIndex = 0;


// Private functions
void interpret_line(std::string& line);


////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////

void load_geometry(const char* filename)
{
	std::ifstream file(filename);

	char line[255];
	if (!file.is_open()) std::cout << "File open error!" << std::endl;
	while (!file.eof())
	{
		file.getline(line, sizeof(line));
		interpret_line(trim(std::string(line)));
	}
	file.close();

	std::cout << std::endl << "Results:" << std::endl << verticesCount << " vertices have been read." << std::endl;
	std::cout << trianglesCount << " triangles have been read." << std::endl;
	std::cout << partsCount << " parts have been read." << std::endl;
}

void interpret_line(std::string& line)
{
	if (strcmp(line.substr(0, 8).c_str(), "vertices") == 0)
	{
		sscanf(line.c_str(), "vertices %d", &verticesCount);
		std::cout << verticesCount << " vertices are being read." << std::endl;
		vertices = new float*[verticesCount];
		currentDataType = CurrentDataType::Vertices;
	}
	else if (strcmp(line.substr(0, 9).c_str(), "triangles") == 0)
	{
		sscanf(line.c_str(), "triangles %d", &trianglesCount);
		std::cout << trianglesCount << " triangles are being read." << std::endl;
		triangles = new int*[trianglesCount];
		currentDataType = CurrentDataType::Triangles;
	}
	else if (strcmp(line.substr(0, 5).c_str(), "parts") == 0)
	{
		sscanf(line.c_str(), "parts %d", &partsCount);
		std::cout << partsCount << " parts are being read for every trangle." << std::endl;
		parts = new int[trianglesCount];
		currentDataType = CurrentDataType::Parts;
	}

	else if (currentDataType == CurrentDataType::Vertices && currentVertexIndex < verticesCount)
	{
		vertices[currentVertexIndex] = new float[3];
		sscanf(line.c_str(), "%f %f %f", &vertices[currentVertexIndex][0], &vertices[currentVertexIndex][1], &vertices[currentVertexIndex][2]);
		currentVertexIndex ++;
	}
	else if (currentDataType == CurrentDataType::Triangles && currentTriangleIndex < trianglesCount)
	{
		triangles[currentTriangleIndex] = new int[3];
		sscanf(line.c_str(), "%d %d %d", &triangles[currentTriangleIndex][0], &triangles[currentTriangleIndex][1], &triangles[currentTriangleIndex][2]);
		currentTriangleIndex ++;
	}
	else if (currentDataType == CurrentDataType::Parts && currentPartIndex < trianglesCount)
	{
		std::istringstream s(trim(line));
		char token[10];

		while (s.getline(token, sizeof(token), ' '))
		{
			int index = atoi(token);
			parts[currentPartIndex++] = index;
		}
	}
}