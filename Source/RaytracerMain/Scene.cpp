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

					while (i<trianSize) 
					{
						getline(file, line);
						lineStream.clear();
						lineStream.str(line);

						while (lineStream >> ind) 
						{
							m_Triangles[i].setAtributeIndex(ind);
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
