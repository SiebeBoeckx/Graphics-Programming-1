#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

#include <iostream>

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			//todo: W2
			//assert(false && "Not Implemented Yet");

			Vector3 rC{ Vector3::Cross(Vector3::UnitY,forward).Normalized() };
			Vector3 uC{ Vector3::Cross(forward,rC).Normalized() };

			Matrix cameraToWorld
			{
				Vector4{rC, 0},
				Vector4{uC, 0},
				Vector4{forward, 0},
				Vector4{rC, 1}
			};

			right = rC;
			up = uC;


			return cameraToWorld;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();
			const float movementSpeed = 5.f * pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);


			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			//todo: W2
			//assert(false && "Not Implemented Yet");

			//movement
			if(pKeyboardState[SDL_SCANCODE_W])
			{
				this->origin += forward * movementSpeed;
			}
			else if (pKeyboardState[SDL_SCANCODE_S])
			{
				this->origin -= forward * movementSpeed;
			}
			else if (pKeyboardState[SDL_SCANCODE_A])
			{
				this->origin -= right * movementSpeed;
			}
			else if (pKeyboardState[SDL_SCANCODE_D])
			{
				this->origin += right * movementSpeed;
			}

			//rotation
			Matrix finalRotationMat{};
			const float rotationSpeed{ 10.f * pTimer->GetElapsed() };

			if((mouseState & SDL_BUTTON_RMASK) != 0)
			{
				if(mouseX != 0)
				{
					int k = 1;
				}

				totalYaw += mouseX * rotationSpeed;
				totalPitch -= mouseY * rotationSpeed;

				finalRotationMat = Matrix::CreateRotation(totalPitch, totalYaw, 0);

				forward = finalRotationMat.TransformVector(Vector3::UnitZ);
				forward.Normalize();
			}
		}
	};
}
