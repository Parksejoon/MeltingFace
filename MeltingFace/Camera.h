#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace MF
{
	class Camera
	{
	public:
		Camera();
		~Camera();

		void SetPosition(float, float, float);
		void SetRotation(float, float, float);

		XMFLOAT3 GetPosition();
		XMFLOAT3 GetRotation();

		void Render();
		void GetViewMatrix(XMMATRIX&);

	private:
		float m_positionX;
		float m_positionY;
		float m_positionZ;
		float m_rotationX;
		float m_rotationY;
		float m_rotationZ;
		XMMATRIX m_viewMatrix;
	};
}