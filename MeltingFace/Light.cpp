#include "Light.h"

namespace MF
{
	Light::Light()
	{
	}
	
	Light::Light(const Light& other)
	{
	}
	
	Light::~Light()
	{
	}

	void Light::SetDiffuseColor(float r, float g, float b, float a)
	{
		m_diffuseColor = XMFLOAT4(r, g, b, a);
	}
	
	void Light::SetDirection(float x, float y, float z)
	{
		m_direction = XMFLOAT3(x, y, z);
	}

	XMFLOAT4 Light::GetDiffuseColor()
	{
		return m_diffuseColor;
	}
	
	XMFLOAT3 Light::GetDirection()
	{
		return m_direction;
	}
}

