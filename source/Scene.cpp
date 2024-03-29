#include "Scene.h"
#include "Utils.h"
#include "Material.h"

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene():
		m_Materials({ new Material_SolidColor({1,0,0})})
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for(auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		//todo W1
		//assert(false && "No Implemented Yet!");

		HitRecord currentHitRecord{};

		for (const auto& sphere : m_SphereGeometries)//loop over spheres
		{
			if(GeometryUtils::HitTest_Sphere(sphere, ray, currentHitRecord))
			{
				closestHit = currentHitRecord;
			}
		}

		for(const auto& triangle : m_Triangles)//loop over triangles
		{
			if(GeometryUtils::HitTest_Triangle(triangle,ray, currentHitRecord))
			{
				closestHit = currentHitRecord;
			}
		}

		for (const auto& triangleMesh : m_TriangleMeshGeometries)//loop over triangle meshes
		{
			if (GeometryUtils::HitTest_TriangleMesh(triangleMesh, ray, currentHitRecord))
			{
				closestHit = currentHitRecord;
			}
		}

		for (const auto& plane : m_PlaneGeometries)//loop over planes
		{
			if(GeometryUtils::HitTest_Plane(plane, ray, currentHitRecord))
			{
				if (currentHitRecord.t < closestHit.t)
				{
					closestHit = currentHitRecord;
				}
			}
		}
	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		//todo W3
		//assert(false && "No Implemented Yet!");
		HitRecord currentHitRecord{};

		for (const auto& sphere : m_SphereGeometries)//loop over spheres
		{
			if (GeometryUtils::HitTest_Sphere(sphere, ray))
			{
				return true;
			}
		}
		for (const auto& triangle : m_Triangles)//loop over triangles
		{
			if (GeometryUtils::HitTest_Triangle(triangle, ray))
			{
				return true;
			}
		}
		for (const auto& triangleMesh : m_TriangleMeshGeometries)//loop over triangles meshes
		{
			if (GeometryUtils::HitTest_TriangleMesh(triangleMesh, ray))
			{
				return true;
			}
		}
		for (const auto& plane : m_PlaneGeometries)//loop over planes
		{
			if (GeometryUtils::HitTest_Plane(plane, ray))
			{
				return true;
			}
		}
		return false;
	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W1
	void Scene_W1::Initialize()
	{
				//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Spheres
		AddSphere({ -25.f, 0.f, 100.f }, 50.f, matId_Solid_Red);
		AddSphere({ 25.f, 0.f, 100.f }, 50.f, matId_Solid_Blue);

		//Plane
		AddPlane({ -75.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 75.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, -75.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 75.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 125.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);
	}
#pragma endregion

#pragma region SCENE W2
	void Scene_W2::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;

		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });
		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Plane
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matId_Solid_Green);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matId_Solid_Green);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matId_Solid_Magenta);

		//Spheres

		AddSphere({ -1.75f, 1.f, 0.f }, 0.75f, matId_Solid_Red);
		AddSphere({ 0.f, 1.f, 0.f }, 0.75f, matId_Solid_Blue);
		AddSphere({ 1.75f, 1.f, 0.f }, 0.75f, matId_Solid_Red);
		AddSphere({ -1.75f, 3.f, 0.f }, 0.75f, matId_Solid_Blue);
		AddSphere({ 0.f, 3.f, 0.f }, 0.75f, matId_Solid_Red);
		AddSphere({ 1.75f, 3.f, 0.f }, 0.75f, matId_Solid_Blue);

		//Light
		AddPointLight({ 0.f, 5.f, -5.f }, 70.f, colors::White);
		
	}
#pragma endregion

#pragma region SCENE W3

	void Scene_W3::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, 1.f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .1f));
		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, 1.f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, .6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, .1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));

		//Plane
		AddPlane(Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane(Vector3{ 0.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane(Vector3{ 0.f, 10.f, 0.f }, Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane(Vector3{ 5.f, 0.f, 0.f }, Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.f, 0.f, 0.f }, Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, 0.75f, matCT_GrayRoughMetal);
		AddSphere({ 0.f, 1.f, 0.f }, 0.75f, matCT_GrayMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f }, 0.75f, matCT_GraySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f }, 0.75f, matCT_GrayRoughPlastic);
		AddSphere({ 0.f, 3.f, 0.f }, 0.75f, matCT_GrayMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f }, 0.75f, matCT_GraySmoothPlastic);

		//Light
		AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
		AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light left
		AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f });
	}

#pragma endregion

#pragma region SCENE W3 TestScene

	void Scene_W3_TestScene::Initialize()
	{
		m_Camera.origin = { 0.f, 1.f, -5.f };
		m_Camera.fovAngle = 45.f;

		const auto matLambert_Red = AddMaterial(new Material_Lambert(colors::Red, 1.f));
		const auto matLambertPhong_Blue = AddMaterial(new Material_LambertPhong(colors::Blue, 1.f, 1.f, 60.f));
		const auto matLambert_Yellow = AddMaterial(new Material_Lambert(colors::Yellow, 1.f));
		
		//Spheres
		AddSphere({ -.75f, 1.f, 0.f }, 1.f, matLambert_Red);
		AddSphere({ .75f, 1.f, 0.f }, 1.f, matLambertPhong_Blue);

		//Plane
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_Yellow);

		//Light
		AddPointLight( { 0.f, 5.f, 5.f }, 25.f, colors::White);
		AddPointLight( { 0.f, 5.f, -5.f }, 25.f, colors::White);
	}
#pragma endregion

#pragma region W4_TestScene

	void Scene_W4_TestScene::Initialize()
	{
		m_Camera.origin = { 0.f, 1.f, -5.f };
		m_Camera.fovAngle = 45.f;

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));
		//Plane
		AddPlane(Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane(Vector3{ 0.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane(Vector3{ 0.f, 10.f, 0.f }, Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane(Vector3{ 5.f, 0.f, 0.f }, Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.f, 0.f, 0.f }, Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		////Triangle (temp)
		//auto triangle = Triangle{ {-.75f, .5f, 0.f}, {-.75f, 2.f, 0.f}, {.75f, .5f, 0.f} };
		//triangle.cullMode = TriangleCullMode::NoCulling;
		//triangle.materialIndex = matLambert_White;
		//
		//m_Triangles.emplace_back(triangle);


		////Triangle Mesh (temp)
		//pMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		//pMesh->positions = { {-.75f, -1.f, 0.f}, {-.75f, 1.f, 0.f}, {.75f, 1.f, 1.f}, {.75f, -1.f, 0.f} };
		//pMesh->indices = {
		//	0,1,2, //first triangle
		//	0,2,3  //second triangle
		//};
		//
		//pMesh->CalculateNormals();
		//
		//pMesh->Translate({ 0.f, 1.5f, 0.f });
		//
		//pMesh->UpdateTransforms();

		////Cube Mesh (temp)
		pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		Utils::ParseOBJ("Resources/simple_cube.obj", pMesh->positions, pMesh->normals, pMesh->indices);
		
		pMesh->Scale({ 0.7f, 0.7f, 0.7f });
		pMesh->Translate({ 0.f,1.f,0.f });
		
		pMesh->UpdateTransforms();
		m_TriangleMeshGeometries[0] = *pMesh;

		//Light
		AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
		AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light left
		AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f });
	}

	void Scene_W4_TestScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		pMesh->RotateY(PI * pTimer->GetTotal());
		pMesh->UpdateTransforms();
	}

#pragma endregion

#pragma region W4_ReferenceScene

	void Scene_W4_ReferenceScene::Initialize()
	{
		sceneName = "Reference Scene";
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, 1.f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .1f));
		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, 1.f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, .6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, .1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Plane
		AddPlane(Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane(Vector3{ 0.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane(Vector3{ 0.f, 10.f, 0.f }, Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane(Vector3{ 5.f, 0.f, 0.f }, Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.f, 0.f, 0.f }, Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, 0.75f, matCT_GrayRoughMetal);
		AddSphere({ 0.f, 1.f, 0.f }, 0.75f, matCT_GrayMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f }, 0.75f, matCT_GraySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f }, 0.75f, matCT_GrayRoughPlastic);
		AddSphere({ 0.f, 3.f, 0.f }, 0.75f, matCT_GrayMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f }, 0.75f, matCT_GraySmoothPlastic);

		//Triangles
		const Triangle baseTriangle = { Vector3(-.75f, 1.5f, 0.f), Vector3(.75f, 0.f, 0.f), Vector3(-.75f, 0.f, 0.f) };

		m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		m_Meshes[0]->AppendTriangle(baseTriangle, true);
		m_Meshes[0]->Translate({ -1.75f, 4.5f, 0.f });
		m_Meshes[0]->UpdateTransforms();

		m_Meshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
		m_Meshes[1]->AppendTriangle(baseTriangle, true);
		m_Meshes[1]->Translate({ 0.f, 4.5f, 0.f });
		m_Meshes[1]->UpdateTransforms();

		m_Meshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		m_Meshes[2]->AppendTriangle(baseTriangle, true);
		m_Meshes[2]->Translate({ 1.75f, 4.5f, 0.f });
		m_Meshes[2]->UpdateTransforms();

		//Light
		AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
		AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light left
		AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f });
	}

	void Scene_W4_ReferenceScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		const auto yawAngle = (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2;
		for (const auto pMesh: m_Meshes)
		{
			pMesh->RotateY(yawAngle);
			pMesh->UpdateTransforms();
		}
	}

#pragma endregion

#pragma region W4_BunnyScene

	void Scene_W4_BunnyScene::Initialize()
	{
		m_Camera.origin = { 0.f, 1.f, -5.f };
		m_Camera.fovAngle = 45.f;

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Plane
		AddPlane(Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane(Vector3{ 0.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane(Vector3{ 0.f, 10.f, 0.f }, Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane(Vector3{ 5.f, 0.f, 0.f }, Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.f, 0.f, 0.f }, Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		////Bunny Mesh
		pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		Utils::ParseOBJ("Resources/lowpoly_bunny2.obj", pMesh->positions, pMesh->normals, pMesh->indices);

		//pMesh->Scale({ 2.f, 2.f, 2.f });
		//pMesh->Translate({ 0.f,1.f,-10.f });
		//pMesh->RotateY(180);

		pMesh->UpdateTransforms();
		m_TriangleMeshGeometries[0] = *pMesh;

		//Light
		AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
		AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light left
		AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f });
	}

#pragma endregion
}
