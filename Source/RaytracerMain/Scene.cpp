#include "Scene.h"
#include "BaseUtils\BaseCore.h"

using namespace nprt;
using namespace std;


Scene::Scene() : m_Triangles(), 
	m_MaxDomain(-numeric_limits<float>::infinity(), -numeric_limits<float>::infinity(), -numeric_limits<float>::infinity()),
	m_MinDomain(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity()),
	m_DomainSize(0, 0, 0)
{ }


void Scene::LoadGeometry(const char* filename)
{
	int vertSize = NULL;
	ifstream file;
	string line, token;
	stringstream lineStream;
	vector<Point3d> vertices;

	m_MaxDomain.x = m_MaxDomain.y = m_MaxDomain.z = -numeric_limits<float>::infinity();
	m_MinDomain.x = m_MinDomain.y = m_MinDomain.z = numeric_limits<float>::infinity();
	
	file.open(filename);

	if (file.is_open()) 
	{	
		while (!file.eof()) 
		{			
			getline(file, line);
			lineStream.clear();
			lineStream.str(line);
			
			while (lineStream >> token) 
			{				
				if (token == "vertices" && !lineStream.eof()) 
				{
					lineStream >> vertSize;
					vertices = vector<Point3d>(vertSize);
					
					for (int i=0; i<vertSize; i++) 
					{
						getline(file, line);
						lineStream.clear();
						lineStream.str(line);
						
						float t1, t2, t3;
						lineStream >> t1;
						lineStream >> t2;
						lineStream >> t3;

						m_MinDomain.x = t1 < m_MinDomain.x ? t1 : m_MinDomain.x;
						m_MinDomain.y = t2 < m_MinDomain.y ? t2 : m_MinDomain.y;
						m_MinDomain.z = t3 < m_MinDomain.z ? t3 : m_MinDomain.z;

						m_MaxDomain.x = t1 > m_MaxDomain.x ? t1 : m_MaxDomain.x;
						m_MaxDomain.y = t2 > m_MaxDomain.y ? t2 : m_MaxDomain.y;
						m_MaxDomain.z = t3 > m_MaxDomain.z ? t3 : m_MaxDomain.z;
						
						vertices[i] = Point3d(t1, t2, t3);
					}
				}
				
				int trianSize = 0;
				if (token == "triangles" && !lineStream.eof()) 
				{					
					lineStream >> trianSize;
					m_Triangles.reserve(trianSize);
					
					for (int i=0; i<trianSize; i++) 
					{
						getline(file, line);
						lineStream.clear();
						lineStream.str(line);
						int v1, v2, v3;
						lineStream >> v1;
						lineStream >> v2;
						lineStream >> v3;
						
						m_Triangles.push_back(Triangle(vertices[v1], vertices[v2], vertices[v3], i));
					}
				}

				if (token == "parts" && !lineStream.eof()) 
				{
					int i=0;
					int ind;
					const int numTriangles = m_Triangles.size();

					while (i < numTriangles) 
					{
						getline(file, line);
						lineStream.clear();
						lineStream.str(line);

						while (lineStream >> ind) 
						{
							m_Triangles[i].setMaterialIndex(ind);
							i++;
						}
					}
				}
			}			
		}
	}
	file.close();

	m_DomainSize.x = m_MaxDomain.x - m_MinDomain.x;
	m_DomainSize.y = m_MaxDomain.y - m_MinDomain.y;
	m_DomainSize.z = m_MaxDomain.z - m_MinDomain.z;
}

void Scene::LoadAttributes(const char* filePath) 
{
	ifstream file;
	string line, token;
	stringstream lineStream;

	float kd, ks, wg, ka, r, g, b, kt, eta, kr;
	kd = ks = wg = ka = r = g = b = kt = eta = kr = 0.0f;

	file.open(filePath);

	if (file.is_open())
	{
		getline(file, line);
		lineStream.clear();
		lineStream.str(line);

		int attributeNumber = 0;
		lineStream >> attributeNumber;			
		getline(file, line);
		while(!file.eof())
		{
			for (int i=0; i<attributeNumber; i++) 
			{
				getline(file, line);
				getline(file, line);
				getline(file, line);
				lineStream.clear();
				lineStream.str(line);
				while(lineStream>>token) 
				{
					if (token=="kd")		
					{
						lineStream >> kd;
					}
					else if (token=="ks")		
					{
						lineStream >> ks;
					}
					else if (token=="gs")		
					{
						lineStream >> wg;
					}
					else if (token=="color")	
					{
						lineStream >> r;
						lineStream >> g;
						lineStream >> b;
					}
					else if (token=="kts")	
					{
						lineStream >> kt;
					}
					else if (token=="eta")	
					{
						lineStream >> eta;
					}
					else if (token=="kf")		
					{
						lineStream >> kr;
					}
					else if (token=="ka")		
					{
						lineStream >> ka;
					}
					else if (token=="enddef") 
					{
						Material m(kd, ks, wg, ka, r / 255.0f, g / 255.0f, b / 255.0f, kt, eta, kr);
						m.kdcG = kd;
						m.kdcB = kd;
						m.kscG = ks;
						m.kscB = ks;
						m.krcG = kt;
						m.krcB = kt;
						m.kacR = m.kacG = m.kacB = 0.0f;
						m_Materials.push_back(m);

						break;
					}
					getline(file, line);
					lineStream.clear();
					lineStream.str(line);
				}
				getline(file, line);
			}
			break;
		}
	}
	file.close();
}

void Scene::RenderToFile(const char* filename, int width, int height)
{
	FIBITMAP* dib = FreeImage_Allocate(width, height, 24);
	RGBQUAD color = {0};
	const int numTriangles = m_Triangles.size();
	
	Vector3d rayDir;
	const Vector3d observerPos(m_Camera.cameraCenter.x, m_Camera.cameraCenter.y, m_Camera.cameraCenter.z);

	for (int x = 0; x < width; x++)
	{
		cout << "Row: " << (x + 1) << "/" << width << "\n";
		for (int y = 0; y < height; y++)
		{
			// The background color is black
			color.rgbRed = color.rgbGreen = color.rgbBlue = 0;

			// Calculate the ray direction based on the magic equations from the lecture
			Point3d U = m_Camera.topRight - m_Camera.topLeft;
			Point3d V = m_Camera.bottomLeft - m_Camera.topLeft;
			Point3d ul = m_Camera.topLeft;
			Point3d P_ij = ul + U * ((float)x / (m_Camera.xResolution - 1)) + V * ((float)y / (m_Camera.yResolution - 1));

			rayDir.x = (P_ij.x - observerPos.x);
			rayDir.y = (P_ij.y - observerPos.y);
			rayDir.z = (P_ij.z - observerPos.z);
			rayDir.normalize();

			// Select the driangle that has the smallest intersection distance
			float minDist = numeric_limits<float>::max();
			int idx = -1;
			for(int t = 0; t < numTriangles; ++t)
			{
				float intersectionDist = m_Triangles[t].intersection(observerPos, rayDir);
				if(intersectionDist > -1.0f && intersectionDist < minDist)
				{
					idx = t;
					minDist = intersectionDist;
				}
			}

			// Get the colour only if a triangle has been hit
			if(idx != -1)
			{
				color.rgbRed = static_cast<BYTE>(255 * m_Materials[m_Triangles[idx].materialIndex].r);
				color.rgbGreen = static_cast<BYTE>(255 * m_Materials[m_Triangles[idx].materialIndex].g);
				color.rgbBlue = static_cast<BYTE>(255 * m_Materials[m_Triangles[idx].materialIndex].b);
			}

			// Debug output
			//cout << (int)color.rgbRed << ", " << (int)color.rgbGreen << ", " << (int)color.rgbBlue << "\n";
			//cout << "hit: " << x << ", " << y << " (" << idx << ")\n";
			//cin.get();

			FreeImage_SetPixelColor(dib, x, y, &color);
		}
	}

	FreeImage_Save(FIF_PNG, dib, filename, PNG_Z_BEST_SPEED);
	FreeImage_Unload(dib);
}

void Scene::LoadCamera(const char* filePath) 
{
	ifstream file;
	string line, token;
	stringstream lineStream;
	Point3d cameraCenter, topLeft, bottomLeft, topRight;
	int xRes=-1, yRes=-1;

	file.open(filePath);

	if (file.is_open()) 
	{
		while(!file.eof())
		{
			getline(file, line);
			lineStream.clear();
			lineStream.str(line);
			while (lineStream >> token)
			{

				if (token == "viewpoint")
				{
					float x, y, z;
					lineStream >> x;
					lineStream >> y;
					lineStream >> z;
					cameraCenter = Point3d(x, y, z);
				}

				if (token == "screen")
				{
					getline(file, line);
					lineStream.clear();
					lineStream.str(line);
					float x, y, z;
					lineStream >> x;
					lineStream >> y;
					lineStream >> z;
					topLeft = Point3d(x, y, z);

					getline(file, line);
					lineStream.clear();
					lineStream.str(line);
					lineStream >> x;
					lineStream >> y;
					lineStream >> z;
					topRight = Point3d(x, y, z);

					getline(file, line);
					lineStream.clear();
					lineStream.str(line);
					lineStream >> x;
					lineStream >> y;
					lineStream >> z;
					bottomLeft = Point3d(x, y, z);
				}

				if (token == "resolution")
				{
					lineStream >> xRes;
					lineStream >> yRes;
				}
			}
		}
	}
	file.close();
	m_Camera = Camera(cameraCenter, topLeft, bottomLeft, topRight, xRes, yRes);
}
