#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace MF
{
	class Light
	{
	public:
		Light();
		Light(const Light&);
		~Light();

		void SetDiffuseColor(float, float, float, float);
		void SetDirection(float, float, float);

		XMFLOAT4 GetDiffuseColor();
		XMFLOAT3 GetDirection();

	private:
		XMFLOAT4 m_diffuseColor;
		XMFLOAT3 m_direction;
	};
}
