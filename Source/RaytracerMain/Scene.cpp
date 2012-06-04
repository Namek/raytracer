#include "Scene.h"
#include "BaseUtils\BaseCore.h"
#include <Windows.h>
#include <omp.h>
#include <cmath>
#include <numeric>

using namespace nprt;
using namespace std;


Scene::Scene() : m_Triangles(), m_ToneMappingKey(0.0f), m_WallTexture(512, 512, TextureType::Bricks)
{ }

void Scene::LoadScene(const char* filename)
{
	m_Triangles.clear();
	m_Materials.clear();
	m_Lights.clear();

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
				if (token == "points_count" && !lineStream.eof())
				{
					int verticesCount;
					lineStream >> verticesCount;
					vertices = vector<Point3d>(verticesCount);

					for (int i = 0; i < verticesCount; ++i)
					{
						getline(file, line);
						lineStream.clear();
						lineStream.str(line);
						
						float t1, t2, t3;
						lineStream >> t3;
						lineStream >> t2;
						lineStream >> t1;

						minDomain.x = t1 < minDomain.x ? t1 : minDomain.x;
						minDomain.y = t2 < minDomain.y ? t2 : minDomain.y;
						minDomain.z = t3 < minDomain.z ? t3 : minDomain.z;

						maxDomain.x = t1 > maxDomain.x ? t1 : maxDomain.x;
						maxDomain.y = t2 > maxDomain.y ? t2 : maxDomain.y;
						maxDomain.z = t3 > maxDomain.z ? t3 : maxDomain.z;
						
						vertices[i] = Point3d(t1, t2, t3);
					}
				}
				else if (token == "triangles_count" && !lineStream.eof())
				{
					int trianglesCount;
					lineStream >> trianglesCount;
					m_Triangles.reserve(trianglesCount);
					
					for (int i = 0; i < trianglesCount; ++i) 
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
				else if (token == "parts_count" && !lineStream.eof())
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
				else if (token == "materials_count" && !lineStream.eof())
				{
					int materialsCount;
					lineStream >> materialsCount;
					m_Materials.reserve(materialsCount);

					int i = 0;
					Material material;

					while (i < materialsCount)
					{
						getline(file, line);
						lineStream.clear();
						lineStream.str(line);
						lineStream >> token;

						if (token == "mat_name")
						{
							if (i > 0)
							{
								m_Materials.push_back(material);
							}

							material.texture = 0;
							lineStream >> material.name;
							i++;
						}
						else if (token == "rgb")
						{
							lineStream >> material.r;
							lineStream >> material.g;
							lineStream >> material.b;
						}
						else if (token == "kdCr")
							lineStream >> material.kdc;
						else if (token == "kdCg")
							lineStream >> material.kdc;
						else if (token == "kdCb")
							lineStream >> material.kdc;
						else if (token == "ksCr")
							lineStream >> material.ksc;
						else if (token == "ksCg")
							lineStream >> material.ksc;
						else if (token == "ksCb")
							lineStream >> material.ksc;
						else if (token == "kaCr")
							lineStream >> material.kac;
						else if (token == "kaCg")
							lineStream >> material.kac;
						else if (token == "kaCb")
							lineStream >> material.kac;
						else if(token == "texture")
							lineStream >> material.texture;
						else if(token == "kt")
							lineStream >> material.kt;
						else if(token == "eta")
							lineStream >> material.eta;
					}
					m_Materials.push_back(material);
				}
				else if (token == "texcoord_count" && !lineStream.eof())
				{
					int texcoordCount;
					lineStream >> texcoordCount;
					
					getline(file, line);
					lineStream.clear();
					lineStream.str(line);

					for (int i = 0; i < texcoordCount; ++i)
					{
						int triangle;
						float u1, v1, u2, v2, u3, v3;
						lineStream >> triangle;
						
						lineStream >> u1;
						lineStream >> v1;
						lineStream >> u2;
						lineStream >> v2;
						lineStream >> u3;
						lineStream >> v3;

						m_Triangles[triangle].SetTexcoords(u1, v1, u2, v2, u3, v3);

						getline(file, line);
						lineStream.clear();
						lineStream.str(line);
					}
				}
				else if (token == "lights_count" && !lineStream.eof())
				{
					int lightsCount;
					lineStream >> lightsCount;

					// Move the reader to the first light name
					while (token != "light_name")
					{
						getline(file, line);
						lineStream.clear();
						lineStream.str(line);
						lineStream >> token;
					}

					for (int i = 0; i < lightsCount; ++i)
					{
						LightSource lightSource;
						
						getline(file, line);
						lineStream.clear();
						lineStream.str(line);
						lineStream >> token;

						// Read light data
						while (token != "gonio_count")
						{
							getline(file, line);
							lineStream.clear();
							lineStream.str(line);
							lineStream >> token;

							if (token == "rgb" && !lineStream.eof())
							{
								lineStream >> lightSource.r;
								lineStream >> lightSource.g;
								lineStream >> lightSource.b;
							}
							else if (token == "pos" && !lineStream.eof())
							{
								lineStream >> lightSource.position.z;
								lineStream >> lightSource.position.y;
								lineStream >> lightSource.position.x;
							}
							else if (token == "power" && !lineStream.eof())
							{
								lineStream >> lightSource.power;
							}
						}

						m_Lights.push_back(lightSource);
					}
				}				
				else if (token == "cam_name" && !lineStream.eof())
				{
					Vector3d pos, lookAt;
					float fov = 0, rotation = 0;
					int resX = 0, resY = 0;
					
					bool usingOldFormat = false;
					Vector3d topLeft, topRight, bottomLeft;
					float x, y, z;

					int params = 0;
					while (params < 9)
					{
						getline(file, line);
						lineStream.clear();
						lineStream.str(line);

						lineStream >> token;
						{
							params++;

							if (token == "pos")
							{
								lineStream >> pos.z;
								lineStream >> pos.y;
								lineStream >> pos.x;
							}
							else if (token == "lookAt")
							{
								lineStream >> lookAt.z;
								lineStream >> lookAt.y;
								lineStream >> lookAt.x;
							}
							else if (token == "resolution")
							{
								lineStream >> resX;
								lineStream >> resY;
							}
							else if (token == "rotation")
							{
								lineStream >> rotation;
								break;
							}
							else if (token == "fov")
								lineStream >> fov;
							
							// old format camera
							else if (token == "viewpoint")
							{
								usingOldFormat = true;

								lineStream >> x;
								lineStream >> y;
								lineStream >> z;
								pos = Point3d(x, y, z);
							}

							else if (token == "screen_topLeft")
							{
								lineStream >> x;
								lineStream >> y;
								lineStream >> z;
								topLeft = Point3d(x, y, z);
							}
							else if (token == "screen_topRight")
							{
								lineStream >> x;
								lineStream >> y;
								lineStream >> z;
								topRight = Point3d(x, y, z);
							}
							else if (token == "screen_bottomLeft")
							{							
								lineStream >> x;
								lineStream >> y;
								lineStream >> z;
								bottomLeft = Point3d(x, y, z);
							}
							else
								params--;
						}
					}

					if (usingOldFormat)
					{
						m_Camera.initialize(pos, topLeft, bottomLeft, topRight, resX, resY);
					}
					else
					{
						m_Camera.initialize(pos, lookAt, fov);
						m_Camera.setResolution(resX, resY);
					}
				}
			}
		}
	}
	file.close();

	for (int i = 0, n = m_Triangles.size(); i < n; ++i)
	{
		Triangle& triangle = m_Triangles[i];

		triangle.hasDisplacement = (triangle.texture != 0);
		triangle.texture = &m_WallTexture;
		Vector3d triangleCenter = triangle.p1 + (triangle.p2 - triangle.p1) * 0.5f + (triangle.p3 - triangle.p1) * 0.5f;

		if (triangle.norm.dotProduct(triangleCenter - m_Camera.cameraCenter) > 0)
		{
			m_Triangles[i] = Triangle(triangle.p3, triangle.p2, triangle.p1, triangle.ind);
		}
	}

	m_Octree.buildTree(m_Triangles, minDomain, maxDomain);
}

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

						while (i < numTriangles && lineStream >> ind)
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
			int tex = 0;
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
					else if (token=="ka")		
					{
						lineStream >> ka;
					}
					else if(token=="texture")
					{
						lineStream >> tex;
					}
					else if (token=="enddef") 
					{
						Material m(kd, ks, wg, ka, r / 255.0f, g / 255.0f, b / 255.0f, kt, eta, tex);
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

void Scene::RenderToFile(const char* filename) const
{
	RenderToFile(filename, m_Camera.xResolution, m_Camera.yResolution);
}

void Scene::RenderToFile(const char* filename, int width, int height) const
{
	FIBITMAP* dib = FreeImage_Allocate(width, height, 24);
	const int numPixels = width * height;
	Vector3d* pixels = new Vector3d[numPixels];
	
	const Point3d observerPos(m_Camera.cameraCenter);
	Point3d U = m_Camera.topRight - m_Camera.topLeft;
	Point3d V = m_Camera.bottomLeft - m_Camera.topLeft;

	volatile long rowsDoneCount = 0;

#ifndef _DEBUG
	#pragma omp parallel for
#endif
	for (int y = 0; y < height; y++)
	{	
		cout << "Completed: " << (100 * (static_cast<int>(rowsDoneCount) + 1) / height) << "%\t\t\t\t\t\r";

		Vector3d rayDirection;
		Vector3d floatColor(0, 0, 0);

		for (int x = 0; x < width; x++)
		{m_WallTexture.currentX = x;
			// Calculate the ray direction based on the magic equations from the lecture
			Point3d P_ij = m_Camera.topLeft + U * (static_cast<float>(x) / (width - 1)) + V * (static_cast<float>(y) / (height - 1));
			rayDirection = P_ij - observerPos;
			rayDirection.normalize();
			CalculateColor(rayDirection, observerPos, m_NumReflections, floatColor);

			pixels[y * width + x].x = floatColor.x;
			pixels[y * width + x].y = floatColor.y;
			pixels[y * width + x].z = floatColor.z;

			floatColor.x = 0;
			floatColor.y = 0;
			floatColor.z = 0;
		}

		_InterlockedIncrement(&rowsDoneCount);
	}


	// Do some tone mapping
	if(m_EnableToneMapping)
	{
		PerformToneMapping(pixels, numPixels);
	}

	if(m_EnableGamma)
	{
		PerformGammaCorrection(pixels, numPixels);
	}

	//Determine the maximum colour value	
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

void Scene::CalculateColor(const Vector3d& rayDirection, const Vector3d& observerPos, int numReflections, Vector3d& in_color) const
{
	const int numLights = m_Lights.size();

	// Trace the ray in Octree
	vector<pair<Triangle, Point3d>> intersectedTriangles;
	pair<Triangle, Point3d> intersectedTriangle;
	
	if(m_Octree.castRayForTriangle(observerPos, rayDirection, intersectedTriangle))
	{
		const Triangle& hitTriangle = intersectedTriangle.first;
		const Vector3d intersectionPt = intersectedTriangle.second;
		const Vector3d observerDir = -rayDirection;
		const Material& material = m_Materials[hitTriangle.materialIndex];
		Vector3d reflectedRay = hitTriangle.norm * 2 * observerDir.dotProduct(hitTriangle.norm) - observerDir;
		reflectedRay.normalize();

		// Wireframe mode
		//float u, v;
		//hitTriangle.getUV(intersectedTriangle.second, u, v);
		//if(u * v > 0.01f)
		//	return;

		/*if(hitTriangle.materialIndex != 32)
			return;*/

		if(material.texture == 0)
		{
			// Apply the material
			ApplyMaterialColor(material, in_color);
		}
		else
		{
			// Apply a texture
			ApplyTexture(hitTriangle, intersectedTriangle.second, in_color);
		}

		// Mirror reflection component
		CalculateReflectionComponent(in_color, intersectionPt, material, reflectedRay, numReflections - 1);
		 
		// Refraction component
		CalculateRefractionComponent(in_color, intersectionPt, observerDir, hitTriangle, material, numReflections - 1);

		float lightIntensity = 0.0f;
		for(int lgt = 0; lgt < numLights; ++lgt)
		{
			const LightSource& light = m_Lights[lgt];
			Vector3d lgtDir = (light.position - intersectionPt);
			float attenuation = lgtDir.length() / m_Octree.getDomainSize().length();
			lgtDir.normalize();			

			// Perform a shadow cast from the intersection point
			bool shadow = false;
			if(m_EnableShadows)
			{
				pair<Triangle, Point3d> t;
				shadow = m_Octree.castRayForTriangle(intersectionPt + lgtDir * 0.002f, lgtDir, t);

				if(shadow)
				{
					//in_color *= 0.95f;
					continue;
				}
			}

			// If there are no intersections, add the light
			// Calculate the diffuse component
			float dot = hitTriangle.norm.dotProduct(lgtDir);			
			dot *= dot;

			// Calculate the specular component
			float specular = lgtDir.dotProduct(reflectedRay);
			
			// Apply the specular and diffuse components
			lightIntensity += (dot * material.kdc + specular * material.ksc) * attenuation * light.power;
		}

		in_color += Vector3d(1.0f, 1.0, 1.0f) * lightIntensity;
	}
}

void Scene::CalculateReflectionComponent(nprt::Vector3d& in_color, const nprt::Vector3d& intersectionPt, const nprt::Material& material,
										const nprt::Vector3d& reflectedRay, int numReflections) const 
{
	if(m_EnableReflection
		&& material.ksc > 0 
		&& numReflections > 0)
	{
		Vector3d in_refl_color(0, 0, 0);
		CalculateColor(reflectedRay, intersectionPt + reflectedRay * 0.01f, numReflections - 1, in_refl_color);
		
		// Scaling of the reflection intensity
		float div = static_cast<float>((m_NumReflections) ? m_NumReflections - 1 : 0);
		float intensity = (m_NumReflections - numReflections) / div;
		in_color += in_refl_color * material.ksc * intensity;
	}
}

void Scene::CalculateRefractionComponent(nprt::Vector3d& in_color, const nprt::Vector3d& intersectionPt, 
										const nprt::Vector3d& observerDir, const nprt::Triangle& hitTriangle, 
										const nprt::Material& material, int numReflections) const 
{
	if(m_EnableRefraction  
			&& material.kt > 0
			&& numReflections > 0)
		{
			Vector3d in_refr_color(0, 0, 0);
			Vector3d horz_vec = (observerDir - hitTriangle.norm * observerDir.dotProduct(hitTriangle.norm)) * (-material.eta);

			float ndo = hitTriangle.norm.dotProduct(observerDir);
			float vert_vec_scale = -sqrt(1 - material.eta * material.eta * (1 - ndo * ndo));
			Vector3d vert_vec = hitTriangle.norm * vert_vec_scale;
			Vector3d refractedRay = horz_vec + vert_vec;
			refractedRay.normalize();

			CalculateColor(refractedRay, intersectionPt + refractedRay * 0.01f, numReflections - 1, in_refr_color);
			in_color += in_refr_color * material.kt;
		}
}

void Scene::PerformGammaCorrection(Vector3d* pixels, const int numPixels) const
{
	for(int p = 0; p < numPixels; ++p)
	{
		pixels[p].x = powf(pixels[p].x, m_Gamma);
		pixels[p].y = powf(pixels[p].y, m_Gamma);
		pixels[p].z = powf(pixels[p].z, m_Gamma);

		pixels[p].x /= (1.0f + pixels[p].x);
		pixels[p].y /= (1.0f + pixels[p].y);
		pixels[p].z /= (1.0f + pixels[p].z);
	}
}

void Scene::PerformToneMapping(Vector3d* pixels, const int numPixels) const
{
	// Logarithmic luminance scaling
	// Calculate the low average luminance
	const float rgbToLum[] = {0.27f, 0.67f, 0.06f};
	const float delta = 0.01f;
	float lw = 0.0f;
	float lsum = 0.0f;
	float pixelLum = 0.0f;
	for(int p = 0; p < numPixels; ++p)
	{
		pixels[p].x = Utils::Clamp(pixels[p].x, 0.0f, std::numeric_limits<float>::max());
		pixels[p].y = Utils::Clamp(pixels[p].y, 0.0f, std::numeric_limits<float>::max());
		pixels[p].z = Utils::Clamp(pixels[p].z, 0.0f, std::numeric_limits<float>::max());

		pixelLum = rgbToLum[0] * pixels[p].x 
				 + rgbToLum[1] * pixels[p].y 
				 + rgbToLum[2] * pixels[p].z;

		lsum += log(delta + pixelLum);
	}
	
	lw = 1.0f / numPixels * lsum;

	//// Apply the luminance scaling operator
	float* luminance = new float[numPixels];	
	for(int p = 0; p < numPixels; ++p)
	{
		pixelLum = rgbToLum[0] * pixels[p].x 
				 + rgbToLum[1] * pixels[p].y 
				 + rgbToLum[2] * pixels[p].z;
		luminance[p] = m_ToneMappingKey * pixelLum / lw;
		luminance[p] /= (1.0f + luminance[p]);
		
		pixels[p] *= luminance[p];

		pixels[p].x /= (1.0f + pixels[p].x);
		pixels[p].y /= (1.0f + pixels[p].y);
		pixels[p].z /= (1.0f + pixels[p].z);
	}
	Utils::SafeDeleteArr(luminance);
}

void Scene::LoadLights(const char* filename)
{
	ifstream file;
	string line, token;
	stringstream lineStream;
	float x, y, z, r, g, b, flux;
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

						m_Lights.push_back(LightSource(x, y, z, r, g, b, flux));
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

void Scene::ApplyMaterialColor(const Material& material, Vector3d& in_color) const
{
	in_color.x = material.r * (1.0f - material.kt);
	in_color.y = material.g * (1.0f - material.kt);
	in_color.z = material.b * (1.0f - material.kt);
}

void Scene::ApplyTexture(const Triangle& hitTriangle, const Vector3d& hitPoint, Vector3d& in_color) const
{
	Vector3d barycentric;
	hitTriangle.getUV(hitPoint, barycentric.x, barycentric.y);
	barycentric.z = 1.0f - barycentric.x - barycentric.y;
	
	float u = barycentric.x * hitTriangle.u1 + barycentric.y * hitTriangle.u2 + barycentric.z * hitTriangle.u3;
	float v = barycentric.x * hitTriangle.v1 + barycentric.y * hitTriangle.v2 + barycentric.z * hitTriangle.v3;
	in_color.x = m_WallTexture.GetTexel(u, v).x;
	in_color.y = m_WallTexture.GetTexel(u, v).y;
	in_color.z = m_WallTexture.GetTexel(u, v).z;

	hitTriangle.texture = &m_WallTexture;
	hitTriangle.hasDisplacement = true;
}
