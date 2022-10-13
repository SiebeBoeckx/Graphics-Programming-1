#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
			//assert(false && "No Implemented Yet!");

			
			const Vector3 tc{ sphere.origin - ray.origin }; //vector between ray origin and sphere center
			const float tcl2{ tc.SqrMagnitude() }; //length of tc squared
			const float dp{ Vector3::Dot(tc, ray.direction) }; //distance between ray origin and P (P is the intersection point of the line through the sphere origin, perpendicular onto the ray)
			const float od2{ tcl2 - (dp * dp) }; //squared length of CP (center of sphere to point described above)
			
			if (od2 <= sphere.radius * sphere.radius) // if distance between center of sphere and P is smaller than radius --> intersection(s)
			{
				const float tca{ sqrtf((sphere.radius * sphere.radius) - od2) };
				const float t0{ dp - tca };
			
				if (t0 < ray.min || t0 > ray.max)
				{
					hitRecord.didHit = false;
					return false;
				}

				hitRecord.didHit = true;
				if(ignoreHitRecord)

				{
					return true;
				}
			
				const Vector3 intersect{ ray.origin + ray.direction * t0 };
				
				hitRecord.t = t0;
				hitRecord.materialIndex = sphere.materialIndex;
				hitRecord.origin = intersect;
				hitRecord.normal = (intersect - sphere.origin).Normalized();
				return true;
			}
			hitRecord.didHit = false;
			return false;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
			//assert(false && "No Implemented Yet!");

			const float t{ Vector3::Dot((plane.origin - ray.origin), plane.normal) / Vector3::Dot(ray.direction, plane.normal)}; //distance between ray origin and plane intersect

			if (t > ray.min && t < ray.max)
			{
				const Vector3 p{ ray.origin + t * ray.direction };

				hitRecord.didHit = true;

				if(ignoreHitRecord)
				{
					return true;
				}

				hitRecord.t = t;
				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.origin = p;
				hitRecord.normal = plane.normal;
				return true;
			}
			hitRecord.didHit = false;
			return false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
		
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			//todo W3
			//assert(false && "No Implemented Yet!");
			return light.origin - origin;
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3
			//assert(false && "No Implemented Yet!");
			if(light.type == LightType::Point)
			{
				const ColorRGB irradiance = light.color * (light.intensity / (light.origin - target).SqrMagnitude());
				return irradiance;
			}
			else if(light.type == LightType::Directional)
			{
				const ColorRGB irradiance = light.color * light.intensity;
				return irradiance;
			}
			return {};
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

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
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}