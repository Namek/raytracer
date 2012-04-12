#include "Scene.h"
#include "BaseUtils\BaseCore.h"

using namespace nprt;
using namespace std;


Scene::Scene() : m_Triangles()
{ }


void Scene::LoadGeometry(const char* filename)
{
	int vertSize = NULL;
	ifstream file;
	string line, token;
	stringstream lineStream;
	vector<Point3d> vertices;
	Point3d minDomain, maxDomain;

	maxDomain.x = maxDomain.y = maxDomain.z = -numeric_limits<float>::infinity();
	minDomain.x = minDomain.y = minDomain.z = numeric_limits<float>::infinity();
	
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

						minDomain.x = t1 < minDomain.x ? t1 : minDomain.x;
						minDomain.y = t2 < minDomain.y ? t2 : minDomain.y;
						minDomain.z = t3 < minDomain.z ? t3 : minDomain.z;

						maxDomain.x = t1 > maxDomain.x ? t1 : maxDomain.x;
						maxDomain.y = t2 > maxDomain.y ? t2 : maxDomain.y;
						maxDomain.z = t3 > maxDomain.z ? t3 : maxDomain.z;
						
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
	
	m_Octree.buildTree(m_Triangles, minDomain, maxDomain);
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
						m.kacR = m.kacG = m.kacB = ka;
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

void Scene::RenderToFile(const char* filename, int width, int height) const
{
	FIBITMAP* dib = FreeImage_Allocate(width, height, 24);	
	Vector3d floatColor(0, 0, 0);
	const int numTriangles = m_Triangles.size();
	const int numLights = m_Lights.size();
	Vector3d* pixels = new Vector3d[width * height];
	
	Vector3d rayDir;
	const Point3d observerPos(m_Camera.cameraCenter.x, m_Camera.cameraCenter.y, m_Camera.cameraCenter.z);
	m_Octree.setObserverPoint(observerPos);

	for (int y = 0; y < height; y++)
	{
		cout << "Completed: " << (100 * (y + 1) / height) << "%\r";
		for (int x = 0; x < width; x++)
		{
			// Calculate the ray direction based on the magic equations from the lecture
			Point3d U = m_Camera.topRight - m_Camera.topLeft;
			Point3d V = m_Camera.bottomLeft - m_Camera.topLeft;
			Point3d ul = m_Camera.topLeft;
			Point3d P_ij = ul + U * (static_cast<float>(x) / (width - 1)) + V * (static_cast<float>(y) / (height - 1));

			rayDir = P_ij - observerPos;
			rayDir.normalize();

			// Trace the ray in Octree
			Triangle triangle;
			m_Octree.castRayForTriangle(rayDir, triangle);

			///////////////////////////////////////////////////////
			// Deprecated: octree will be used 
			// Select the triangle that has the smallest intersection distance
			///////////////////////////////////////////////////////
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
			///////////////////////////////////////////////////////
			// End Deprecated
			///////////////////////////////////////////////////////

			// Get the colour only if a triangle has been hit
			if(idx != -1)
			{
				const Vector3d intersectionPt = observerPos + rayDir * minDist;
				const Triangle& hitTriangle = m_Triangles[idx];
				const Material& material = m_Materials[hitTriangle.materialIndex];
				floatColor.x = 1.0f * material.r;
				floatColor.y = 1.0f * material.g;
				floatColor.z = 1.0f * material.b;

				for(int lgt = 0; lgt < numLights; ++lgt)
				{
					const LightSource& light = m_Lights[lgt];
					Vector3d lgtDir = (intersectionPt - light.position);
					lgtDir.normalize();

					float intensityDiffuse = hitTriangle.norm.dotProduct(lgtDir);

					// Value clamping is being done after the rendering
					float valRed = material.kdcR * intensityDiffuse * light.r;
					floatColor.x += valRed;

					float valGreen = material.kdcG * intensityDiffuse * light.g;
					floatColor.y += valGreen;

					float valBlue = material.kdcB * intensityDiffuse * light.b;
					floatColor.z += valBlue;
				}
			}

			pixels[y * width + x].x = floatColor.x;
			pixels[y * width + x].y = floatColor.y;
			pixels[y * width + x].z = floatColor.z;
		}
	}

	//Determine the maximum colour value
	const int numPixels = width * height;
	RGBQUAD color = {0};

	// Normalize the colours
	for(int p = 0; p < numPixels; ++p)
	{
		pixels[p].x = Utils::Clamp(pixels[p].x, 0.0f, 1.0f);
		pixels[p].y = Utils::Clamp(pixels[p].y, 0.0f, 1.0f);
		pixels[p].z = Utils::Clamp(pixels[p].z, 0.0f, 1.0f);

		color.rgbRed = static_cast<BYTE>(255 * pixels[p].x);
		color.rgbGreen = static_cast<BYTE>(255 * pixels[p].y);
		color.rgbBlue = static_cast<BYTE>(255 * pixels[p].z);
		FreeImage_SetPixelColor(dib, p % width, height - p / width, &color);
	}

	Utils::SafeDeleteArr(pixels);

	FreeImage_Save(FIF_PNG, dib, filename, PNG_Z_BEST_SPEED);
	FreeImage_Unload(dib);
}

void Scene::LoadLights(const char* filename)
{
	ifstream file;
	string line, token;
	stringstream lineStream;
	float x, y, z, flux, r, g, b;
	int lightCount = 0;

	file.open(filename);

	if (file.is_open()) 
	{
		while(!file.eof()) 
		{
			getline(file, line);
			lineStream.clear();
			lineStream.str(line);
			while(lineStream >> token) 
			{
				if (token == "lights")
				{
					getline(file, line);
					lineStream.clear();
					lineStream.str(line);

					lineStream >> lightCount;
				}

				if (token == "Position")
				{
					for (int i = 0; i < lightCount; i++)
					{
						getline(file, line);
						lineStream.clear();
						lineStream.str(line);

						lineStream >> x;
						lineStream >> y;
						lineStream >> z;
						lineStream >> flux;
						lineStream >> r;
						lineStream >> g;
						lineStream >> b;

						m_Lights.push_back(LightSource(x, y, z, flux, r, g, b));
					}
				}
			}
		}
	}
	file.close();
}

void Scene::LoadCamera(const char* filePath) 
{
	ifstream file;
	string line, token;
	stringstream lineStream;
	Point3d cameraCenter, topLeft, bottomLeft, topRight;
	int xRes = -1, yRes = -1;

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
