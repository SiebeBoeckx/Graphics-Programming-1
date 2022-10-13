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

	const float ar{ float(m_Width) / float(m_Height) };
	const float fov{ tanf((camera.fovAngle*TO_RADIANS) / 2) };

	const Matrix cameraToWorld = camera.CalculateCameraToWorld();

	for (int px{}; px < m_Width; ++px)
	{
		const float rayX{ (((2 * (px + 0.5f)) / m_Width) - 1) * ar * fov };

		for (int py{}; py < m_Height; ++py)
		{
			const float rayY{ (1 - ((2 * (py + 0.5f)) / m_Height )) * fov };

			const Vector3 origin{ 0,0,0 };
			const Vector3 rayDirection = cameraToWorld.TransformVector(rayX, rayY, 1).Normalized();

			const Ray viewRay{ camera.origin, rayDirection };

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

			if(px == 320 && py == 288)
			{
				int k{};
			}


			for (const Light& pLight : lights)
			{
				pScene->GetClosestHit(viewRay, closestHit);
				if (closestHit.didHit)
				{
					const float cosineLaw{ Vector3::Dot(closestHit.normal, LightUtils::GetDirectionToLight(pLight, closestHit.origin).Normalized()) };

					if (cosineLaw < 0)
					{
						continue;
					}

					//shadows(hard)
					const Vector3 offsetOrigin = closestHit.normal * 0.001f;

					Vector3 lightDir = LightUtils::GetDirectionToLight(pLight, closestHit.origin); //offset not needed
					const float lightrayMagnitude{ lightDir.Normalize() };
					const Ray lightRay{ closestHit.origin + offsetOrigin,lightDir,0.0001f,lightrayMagnitude };
					if (pScene->DoesHit(lightRay))
					{
						continue;
					}

					const ColorRGB irradiance{ LightUtils::GetRadiance(pLight, closestHit.origin) };
					const ColorRGB BRDF{ materials[closestHit.materialIndex]->Shade(closestHit, lightDir, -rayDirection) };
					switch (m_CurrentLightingMode)
					{
					case LightingMode::Combined:
						finalColor += irradiance * BRDF * cosineLaw;
						break;
					case LightingMode::ObservedArea:
						finalColor += {cosineLaw, cosineLaw, cosineLaw};
						break;
					case LightingMode::Radiance:
						finalColor += irradiance;
						break;
					case LightingMode::BRDF:
						finalColor += BRDF;
						break;
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
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void Renderer::CycleLightingMode()
{
	switch(m_CurrentLightingMode)
	{
	case LightingMode::ObservedArea:
		m_CurrentLightingMode = LightingMode::Radiance;
		break;
	case LightingMode::Radiance:
		m_CurrentLightingMode = LightingMode::BRDF;
		break;
	case LightingMode::BRDF:
		m_CurrentLightingMode = LightingMode::Combined;
		break;
	case LightingMode::Combined:
		m_CurrentLightingMode = LightingMode::ObservedArea;
		break;
	default:
		break;
	}
}