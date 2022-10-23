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
			const Vector3 rayOriginToSphereOrigin{ sphere.origin - ray.origin };
			const float hypothenuseSquared{ rayOriginToSphereOrigin.SqrMagnitude() };
			const float side1{ Vector3::Dot(rayOriginToSphereOrigin, ray.direction) };

			const float distanceToRaySquared{ hypothenuseSquared - side1 * side1 };

			//if the distance to the ray is larger than the radius there will be no results
			//    also if equal because that is the exact border of the circle
			if (distanceToRaySquared >= sphere.radius * sphere.radius) {
				hitRecord.didHit = false;
				return false;
			}

			const float distanceRaypointToIntersect{ sqrt(sphere.radius * sphere.radius - distanceToRaySquared) };
			const float distance{ side1 - distanceRaypointToIntersect };

			if (distance < ray.min || distance > ray.max) {
				hitRecord.didHit = false;
				return false;
			}

			hitRecord.didHit = true;
			if (ignoreHitRecord) {
				return true;
			}
			hitRecord.materialIndex = sphere.materialIndex;
			hitRecord.t = distance;
			hitRecord.origin = ray.origin + distance * ray.direction;
			hitRecord.normal = Vector3(sphere.origin, hitRecord.origin).Normalized();
			return true;
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
			const float distance{ Vector3::Dot(Vector3{ ray.origin, plane.origin }, plane.normal) / Vector3::Dot(ray.direction, plane.normal) };
			if (distance >= ray.min && distance <= ray.max) {
				hitRecord.didHit = true;
				if (ignoreHitRecord) {
					return true;
				}
				hitRecord.t = distance;
				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.normal = plane.normal;
				hitRecord.origin = ray.origin + distance * ray.direction;
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
			//assert(false && "No Implemented Yet!");
			const Vector3 a = triangle.v1 - triangle.v0;
			const Vector3 b = triangle.v2 - triangle.v0;
			const Vector3 normal = Vector3::Cross(a, b);
			const float dotRayNormal{ Vector3::Dot(triangle.normal, ray.direction) };
			if (dotRayNormal == 0.f)
			{
				return false;
			}

			switch (triangle.cullMode)
			{
				case TriangleCullMode::NoCulling:
				{
					break;
				}
				case TriangleCullMode::FrontFaceCulling:
				{
					if (dotRayNormal < 0.f && !ignoreHitRecord)
					{
						return false;
					}
					if(ignoreHitRecord && dotRayNormal > 0.f)
					{
						return false;
					}
					break;
				}
				case TriangleCullMode::BackFaceCulling:
				{
					if (dotRayNormal > 0.f && !ignoreHitRecord)
					{
						return false;
					}
					if (ignoreHitRecord && dotRayNormal < 0.f)
					{
						return false;
					}
					break;
				}
			}

			const Vector3 center = (triangle.v0 + triangle.v1 + triangle.v2) / 3.f;

			const Vector3 l = center - ray.origin;
			const float t = Vector3::Dot(l, triangle.normal) / Vector3::Dot(ray.direction, triangle.normal);

			if(t < ray.min || t > ray.max)
			{
				return false;
			}

			const Vector3 p = ray.origin + t * ray.direction;

			const Vector3 edgeA = triangle.v1 - triangle.v0;
			const Vector3 pointToSideV0 = p - triangle.v0;
			if(Vector3::Dot(normal, Vector3::Cross(edgeA, pointToSideV0)) < 0)
			{
				return false;
			}

			const Vector3 edgeB = triangle.v2 - triangle.v1;
			const Vector3 pointToSideV1 = p - triangle.v1;
			if (Vector3::Dot(normal, Vector3::Cross(edgeB, pointToSideV1)) < 0)
			{
				return false;
			}

			const Vector3 edgeC = triangle.v0 - triangle.v2;
			const Vector3 pointToSideV2 = p - triangle.v2;
			if (Vector3::Dot(normal, Vector3::Cross(edgeC, pointToSideV2)) < 0)
			{
				return false;
			}

			hitRecord.didHit = true;
			hitRecord.normal = triangle.normal;
			hitRecord.materialIndex = triangle.materialIndex;
			hitRecord.t = t;
			hitRecord.origin = p;
			return true;
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
			//assert(false && "No Implemented Yet!");
			for (int triangleNr{}; triangleNr < int(mesh.indices.size() / 3); ++triangleNr) //3 indices indicate 1 triangle
			{
				const Vector3 v0{ mesh.transformedPositions[mesh.indices[triangleNr * 3]] };
				const Vector3 v1{ mesh.transformedPositions[mesh.indices[triangleNr * 3 + 1]] };
				const Vector3 v2{ mesh.transformedPositions[mesh.indices[triangleNr * 3 + 2]] };

				const Triangle triangle{ v0,v1,v2 };

				const Vector3 a = triangle.v1 - triangle.v0;
				const Vector3 b = triangle.v2 - triangle.v0;
				const float dotRayNormal{ Vector3::Dot(mesh.transformedNormals[triangleNr], ray.direction)};
				if (dotRayNormal == 0.f)
				{
					continue;
				}

				switch (mesh.cullMode)
				{
				case TriangleCullMode::NoCulling:
				{
					break;
				}
				case TriangleCullMode::FrontFaceCulling:
				{
					if (dotRayNormal < 0.f && !ignoreHitRecord)
					{
						continue;
					}
					if (ignoreHitRecord && dotRayNormal > 0.f)
					{
						continue;
					}
					break;
				}
				case TriangleCullMode::BackFaceCulling:
				{
					if (dotRayNormal > 0.f && !ignoreHitRecord)
					{
						continue;
					}
					if (ignoreHitRecord && dotRayNormal < 0.f)
					{
						continue;
					}
					break;
				}
				}

				const Vector3 center = (triangle.v0 + triangle.v1 + triangle.v2) / 3.f;

				const Vector3 l = center - ray.origin;
				const float t = Vector3::Dot(l, mesh.transformedNormals[triangleNr]) / Vector3::Dot(ray.direction, mesh.transformedNormals[triangleNr]);

				if (t < ray.min || t > ray.max)
				{
					continue;
				}

				const Vector3 p = ray.origin + t * ray.direction;

				const Vector3 edgeA = triangle.v1 - triangle.v0;
				const Vector3 pointToSideV0 = p - triangle.v0;
				if (Vector3::Dot(mesh.transformedNormals[triangleNr], Vector3::Cross(edgeA, pointToSideV0)) < 0)
				{
					continue;
				}

				const Vector3 edgeB = triangle.v2 - triangle.v1;
				const Vector3 pointToSideV1 = p - triangle.v1;
				if (Vector3::Dot(mesh.transformedNormals[triangleNr], Vector3::Cross(edgeB, pointToSideV1)) < 0)
				{
					continue;
				}

				const Vector3 edgeC = triangle.v0 - triangle.v2;
				const Vector3 pointToSideV2 = p - triangle.v2;
				if (Vector3::Dot(mesh.transformedNormals[triangleNr], Vector3::Cross(edgeC, pointToSideV2)) < 0)
				{
					continue;
				}

				hitRecord.didHit = true;
				hitRecord.normal = mesh.transformedNormals[triangleNr];
				hitRecord.materialIndex = mesh.materialIndex;
				hitRecord.t = t;
				hitRecord.origin = p;
				return true;
			}
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