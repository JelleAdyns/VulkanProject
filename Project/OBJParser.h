#pragma once
#include <fstream>
#include <vector>
#include "Vertex.h"
#include "glm/glm.hpp"

#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function

static bool ParseOBJ(const std::string& filename, std::vector<Vertex3D>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding = true)
{
	std::ifstream file(filename);
	if (!file)
		return false;

	std::vector<glm::vec3> positions{};
	std::vector<glm::vec3> normals{};
	std::vector<glm::vec2> UVs{};

	vertices.clear();
	indices.clear();

	std::string sCommand;
	// start a while iteration ending when the end of file is reached (ios::eof)
	while (!file.eof())
	{
		//read the first word of the string, use the >> operator (istream::operator>>) 
		file >> sCommand;
		//use conditional statements to process the different commands	
		if (sCommand == "#")
		{
			// Ignore Comment
		}
		else if (sCommand == "v")
		{
			//Vertex
			float x, y, z;
			file >> x >> y >> z;

			positions.emplace_back(x, y, z);
		}
		else if (sCommand == "vt")
		{
			// Vertex TexCoord
			float u, v;
			file >> u >> v;

			UVs.emplace_back(u, 1 - v);
		}
		else if (sCommand == "vn")
		{
			// Vertex Normal
			float x, y, z;
			file >> x >> y >> z;

			normals.emplace_back(x, y, z);
		}
		else if (sCommand == "f")
		{
			//if a face is read:
			//construct the 3 vertices, add them to the vertex array
			//add three indices to the index array
			//add the material index as attibute to the attribute array
			//
			// Faces or triangles
			Vertex3D vertex{};
			size_t iPosition, iTexCoord, iNormal;

			uint32_t tempIndices[3];
			for (size_t iFace = 0; iFace < 3; iFace++)
			{
				// OBJ format uses 1-based arrays
				file >> iPosition;
				vertex.pos = positions[iPosition - 1];


				if ('/' == file.peek())//is next in buffer ==  '/' ?
				{
					file.ignore();//read and ignore one element ('/')

					if ('/' != file.peek())
					{
						// Optional texture coordinate
						file >> iTexCoord;

						vertex.texCoord = UVs[iTexCoord - 1];
						//vertex.texCoord[0] = UVs[iTexCoord - 1][0];
						//vertex.texCoord[1] = UVs[iTexCoord - 1][1];

					}

					if ('/' == file.peek())
					{
						file.ignore();

						// Optional vertex normal
						file >> iNormal;
						vertex.normal = normals[iNormal - 1];
					}
				}

				vertices.push_back(vertex);
				tempIndices[iFace] = uint32_t(vertices.size()) - 1;
				//indices.push_back(uint32_t(vertices.size()) - 1);
			}

			indices.push_back(tempIndices[0]);
			if (flipAxisAndWinding) 
			{
				indices.push_back(tempIndices[2]);
				indices.push_back(tempIndices[1]);
			}
			else
			{
				indices.push_back(tempIndices[1]);
				indices.push_back(tempIndices[2]);
			}
		}
		//read till end of line and ignore all remaining chars
		file.ignore(1000, '\n');
	}

	//Cheap Tangent Calculations
	for (uint32_t i = 0; i < indices.size(); i += 3)
	{

		uint32_t index0 = indices[i];
		uint32_t index1 = indices[size_t(i) + 1];
		uint32_t index2 = indices[size_t(i) + 2];
		
		vertices[index0].tangent = { 0.f,0.f,0.f };
		vertices[index1].tangent = { 0.f,0.f,0.f };
		vertices[index2].tangent = { 0.f,0.f,0.f };

		glm::vec3 delta_pos1 = vertices[index1].pos - vertices[index0].pos;
		glm::vec3 delta_pos2 = vertices[index2].pos - vertices[index0].pos;
		glm::vec2 delta_uv1 = vertices[index1].texCoord - vertices[index0].texCoord;
		glm::vec2 delta_uv2 = vertices[index2].texCoord - vertices[index0].texCoord;
		
		float determinant = (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);
		if (fabs(determinant) < 1e-6f)
		{
			vertices[index0].tangent = {0.f,0.f,0.f};
			vertices[index1].tangent = {0.f,0.f,0.f};
			vertices[index2].tangent = { 0.f,0.f,0.f };
			continue;
		}
		float r = 1.0f / determinant;
		glm::vec3 tangent = (delta_pos1 * delta_uv2.y - delta_pos2 * delta_uv1.y) * r;
		
		vertices[index0].tangent += glm::normalize(tangent);
		vertices[index1].tangent += glm::normalize(tangent);
		vertices[index2].tangent += glm::normalize(tangent);

		/*const glm::vec3& p0{ vertices[index0].pos[0], vertices[index0].pos[1], vertices[index0].pos[2] };
		const glm::vec3& p1{ vertices[index1].pos[0], vertices[index1].pos[1], vertices[index1].pos[2] };
		const glm::vec3& p2{ vertices[index2].pos[0], vertices[index2].pos[1], vertices[index2].pos[2] };
		const glm::vec2& uv0{ vertices[index0].texCoord[0], vertices[index0].texCoord[1] };
		const glm::vec2& uv1{ vertices[index1].texCoord[0], vertices[index1].texCoord[1] };
		const glm::vec2& uv2{ vertices[index2].texCoord[0], vertices[index2].texCoord[1] };
			
		const glm::vec3 edge0 = p1 - p0;
		const glm::vec3 edge1 = p2 - p0;
		const glm::vec3 diffX = glm::vec3(uv1.x - uv0.x, uv2.x - uv0.x, 0);
		const glm::vec3 diffY = glm::vec3(uv1.y - uv0.y, uv2.y - uv0.y, 0);
		float r = 1.f / glm::length(glm::cross(diffX, diffY));
			
		glm::vec3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
		vertices[index0].tangent += tangent;
		vertices[index1].tangent += tangent;
		vertices[index2].tangent += tangent;*/
	}
			
	//Create the Tangents (reject)
	for (auto& v : vertices)
	{
		v.tangent = (v.tangent - v.normal * (glm::dot(v.tangent, v.normal)));
		v.tangent = glm::normalize(v.tangent);
		//v.tangent.z *= -1.f;
		if(flipAxisAndWinding)
		{
			v.pos.z *= -1.f;
			v.normal.z *= -1.f;
			v.tangent.z *= -1.f;
		}

	}

	return true;
}
#pragma warning(pop)