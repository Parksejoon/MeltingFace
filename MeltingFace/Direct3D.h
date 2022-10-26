#pragma once

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

namespace MF
{
	class Direct3D
	{
	public:
		Direct3D();
		Direct3D(const Direct3D&);
		~Direct3D();

		bool Initialize(int, int, bool, HWND, bool, float, float);
		void Shutdown();

		void BeginScene(float, float, float, float);
		void EndScene();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		void GetProjectionMatrix(XMMATRIX&);
		void GetWorldMatrix(XMMATRIX&);
		void GetOrthoMatrix(XMMATRIX&);

		void GetVideoCardInfo(char*, int&);

	private:
		bool m_vSync;
		int m_videoCardMemory;
		char m_videoCardDesc[128];
		IDXGISwapChain* m_swapChain;

		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;
		ID3D11RenderTargetView* m_renderTargetView;
		ID3D11Texture2D* m_depthStencilBuffer;
		ID3D11DepthStencilState* m_depthStencilState;
		ID3D11DepthStencilView* m_depthStencilView;
		ID3D11RasterizerState* m_rasterState;

		XMMATRIX m_projectionMatrix;
		XMMATRIX m_worldMatrix;
		XMMATRIX m_orthoMatrix;
	};
}