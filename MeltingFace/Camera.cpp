#include "Camera.h"

namespace MF
{
	Camera::Camera()
	{
		m_positionX = 0;
		m_positionY = 0;
		m_positionZ = 0;

		m_rotationX = 0;
		m_rotationY = 0;
		m_rotationZ = 0;
	}

	Camera::~Camera()
	{
	}

	void Camera::SetPosition(float x, float y, float z)
	{
		m_positionX = x;
		m_positionY = y;
		m_positionZ = z;
	}

	void Camera::SetRotation(float x, float y, float z)
	{
		m_rotationX = x;
		m_rotationY = y;
		m_rotationZ = z;
	}

	XMFLOAT3 Camera::GetPosition()
	{
		return XMFLOAT3(m_positionX, m_positionY, m_positionY);
	}

	XMFLOAT3 Camera::GetRotation()
	{
		return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
	}

	void Camera::Render()
	{
		XMVECTOR up = { 0.0f, 1.0f, 0.0f };
		XMVECTOR position = { m_positionX, m_positionY, m_positionZ };
		XMVECTOR lookAt = { 0.0f, 0.0f, 1.0f };

		float pitch = XMConvertToRadians(m_rotationX);
		float yaw = XMConvertToRadians(m_rotationY);
		float roll = XMConvertToRadians(m_rotationZ);

		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

		lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
		up = XMVector3TransformCoord(up, rotationMatrix);

		lookAt = position + lookAt;

		m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);
	}

	void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
	{
		viewMatrix = m_viewMatrix;
	}
}
