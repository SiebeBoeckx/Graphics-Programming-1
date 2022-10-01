//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	float ar{ float(m_Width) / float(m_Height) };
	float fov{ tanf(	(camera.fovAngle*TO_RADIANS) / 2) };

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			float rayX{ (((2 * (px + 0.5f)) / m_Width) - 1) * ar * fov };
			float rayY{ 1 - ((2 * (py + 0.5f)) / m_Height ) * fov };

			Vector3 origin{ 0,0,0 };
			Vector3 rayDirection = rayX * camera.right + rayY * camera.up + camera.forward;

			const Matrix cameraToWorld = camera.CalculateCameraToWorld();
			Vector3 transformedRay = { cameraToWorld.TransformVector(rayDirection) };
			
			//Vector3 rayDirection = { rayX,rayY,1 };
			transformedRay.Normalize();

			Ray viewRay{ camera.origin, transformedRay };

			//float gradient = px / static_cast<float>(m_Width);
			//gradient += py / static_cast<float>(m_Width);
			//gradient /= 2.0f;

			//ColorRGB finalColor{ rayDirection.x, rayDirection.y, rayDirection.z };
			ColorRGB finalColor{};

			HitRecord closestHit{};
			//
			//Sphere testSphere{ Vector3{0.f, 0.f, 100.f}, 50.f, 0 };
			//
			//GeometryUtils::HitTest_Sphere(testSphere, viewRay, closestHit);
			//

			//if(px == 320 && py == 288)
			//{
			//	int k{};
			//}

			pScene->GetClosestHit(viewRay, closestHit);
			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();
			}

			//shadows(hard)
			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();

				for (auto& pLight : lights)
				{
					Vector3 lightDir = LightUtils::GetDirectionToLight(pLight, closestHit.origin + (closestHit.normal * 0.001f));
					const float lightrayMagnitude{ lightDir.Magnitude() };
					Ray lightRay{ closestHit.origin + (closestHit.normal * 0.001f),lightDir.Normalized(),0.0001f,lightrayMagnitude };
					if (pScene->DoesHit(lightRay))
					{
						finalColor *= 0.5f;
					}
				}
			}


			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	};

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
