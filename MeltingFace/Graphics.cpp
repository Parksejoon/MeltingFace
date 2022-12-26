#include "Graphics.h"

namespace MF
{
	Graphics::Graphics()
	{
		m_Direct3D = nullptr;
		m_Camera = nullptr;
		m_Model = nullptr;
		m_ColorShader = nullptr;
	}

	Graphics::Graphics(const Graphics&)
	{
	}

	Graphics::~Graphics()
	{
	}

	bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
	{
		m_Direct3D = new Direct3D;
		bool result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
		if (!result)
		{
			MessageBox(hwnd, L"Direct3D 초기화에 실패하였습니다.", L"Error", MB_OK);
			return false;
		}

		m_Camera = new Camera;
		m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

		m_Model = new Model;
		result = m_Model->Initialize(m_Direct3D->GetDevice(), "Sphere.obj");
		if (!result)
		{
			MessageBox(hwnd, L"Model 초기화에 실패하였습니다.", L"Error", MB_OK);
			return false;
		}

		m_ColorShader = new ColorShader;
		result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
		if (!result)
		{
			MessageBox(hwnd, L"ColorShader 초기화에 실패하였습니다.", L"Error", MB_OK);
			return false;
		}

		return true;
	}

	void Graphics::Shutdown()
	{
		if (m_ColorShader)
		{
			m_ColorShader->Shutdown();
			delete m_ColorShader;
			m_ColorShader = nullptr;
		}

		if (m_Model)
		{
			m_Model->Shutdown();
			delete m_Model;
			m_Model = nullptr;
		}

		if (m_Camera)
		{
			delete m_Camera;
			m_Camera = nullptr;
		}

		if (m_Direct3D)
		{
			m_Direct3D->Shutdown();
			delete m_Direct3D;
			m_Direct3D = nullptr;
		}
	}

	bool Graphics::Frame()
	{
		return Render();
	}

	bool Graphics::Render()
	{
		m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

		m_Camera->Render();

		XMMATRIX worldMatrix;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;

		m_Camera->GetViewMatrix(viewMatrix);
		m_Direct3D->GetWorldMatrix(worldMatrix);
		m_Direct3D->GetProjectionMatrix(projectionMatrix);

		m_Model->Render(m_Direct3D->GetDeviceContext());

		bool result = m_ColorShader->Render(
			m_Direct3D->GetDeviceContext(),
			m_Model->GetIndexCount(),
			worldMatrix,
			viewMatrix,
			projectionMatrix);
		if (!result)
		{
			return false;
		}

		m_Direct3D->EndScene();

		return true;
	}
}
