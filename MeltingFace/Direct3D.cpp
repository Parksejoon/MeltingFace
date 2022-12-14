#include "Direct3D.h"

namespace MF
{
	Direct3D::Direct3D()
	{
		m_swapChain = nullptr;
		m_device = nullptr;
	}

	Direct3D::Direct3D(const Direct3D&)
	{
	}

	Direct3D::~Direct3D()
	{
	}

	bool Direct3D::Initialize(int screenWidth, int screenHeight, bool vSync, HWND hwnd, bool fullscreen,
		float screenDepth, float screenNear)
	{
		HRESULT result;

		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;

		unsigned int numModes;
		unsigned int i;
		unsigned int numerator;
		unsigned int denominator;
		size_t stringLength;

		DXGI_MODE_DESC* displayModeList;
		DXGI_ADAPTER_DESC adapterDesc;

		int error;

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		D3D_FEATURE_LEVEL featureLevel;

		ID3D11Texture2D* backBufferPtr;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		D3D11_RASTERIZER_DESC rasterDesc;
		D3D11_VIEWPORT viewport;

		float fieldOfView;
		float screenAspect;

		m_vSync = vSync;

		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(result))
		{
			return false;
		}

		result = factory->EnumAdapters(0, &adapter);
		if (FAILED(result))
		{
			return false;
		}

		result = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(result))
		{
			return false;
		}

		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
		if (FAILED(result))
		{
			return false;
		}

		displayModeList = new DXGI_MODE_DESC[numModes];
		if (!displayModeList)
		{
			return false;
		}

		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if (FAILED(result))
		{
			return false;
		}

		for (int i = 0; i < numModes; i++)
		{
			DXGI_MODE_DESC dispalyMode = displayModeList[i];
			if (dispalyMode.Width != (unsigned int)screenWidth)
			{
				continue;
			}

			if (dispalyMode.Height != (unsigned int)screenHeight)
			{
				continue;
			}

			numerator = dispalyMode.RefreshRate.Numerator;
			denominator = dispalyMode.RefreshRate.Denominator;
			break;
		}

		result = adapter->GetDesc(&adapterDesc);
		if (FAILED(result))
		{
			return false;
		}

		m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		error = wcstombs_s(&stringLength, m_videoCardDesc, 128, adapterDesc.Description, 128);
		if (error != 0)
		{
			return false;
		}

		delete[] displayModeList;
		displayModeList = nullptr;

		adapterOutput->Release();
		adapterOutput = nullptr;

		adapter->Release();
		adapter = nullptr;

		factory->Release();
		factory = nullptr;

		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = screenWidth;
		swapChainDesc.BufferDesc.Height = screenHeight;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		if (m_vSync)
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}

		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;

		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		if (fullscreen)
		{
			swapChainDesc.Windowed = false;
		}
		else
		{
			swapChainDesc.Windowed = true;
		}

		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		swapChainDesc.Flags = 0;

		featureLevel = D3D_FEATURE_LEVEL_11_0;

		result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1,
			D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, nullptr, &m_deviceContext);
		if (FAILED(result))
		{
			return false;
		}

		result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
		if (FAILED(result))
		{
			return false;
		}

		result = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_renderTargetView);
		if (FAILED(result))
		{
			return false;
		}

		backBufferPtr->Release();
		backBufferPtr = nullptr;

		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		depthBufferDesc.Width = screenWidth;
		depthBufferDesc.Height = screenHeight;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
		if (FAILED(result))
		{
			return false;
		}


		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
		if (FAILED(result))
		{
			return false;
		}

		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(result))
		{
			return false;
		}

		m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
		if (FAILED(result))
		{
			return false;
		}

		m_deviceContext->RSSetState(m_rasterState);

		viewport.Width = screenWidth;
		viewport.Height = screenHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		m_deviceContext->RSSetViewports(1, &viewport);

		fieldOfView = XM_PI / 4.0f;
		screenAspect = screenWidth / screenHeight;

		m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
		m_worldMatrix = XMMatrixIdentity();
		m_orthoMatrix = XMMatrixOrthographicLH(screenWidth, screenHeight, screenNear, screenDepth);

		return true;
	}

	void Direct3D::Shutdown()
	{
		if (m_swapChain)
		{
			m_swapChain->SetFullscreenState(false, nullptr);
		}

		if (m_rasterState)
		{
			m_rasterState->Release();
			m_rasterState = nullptr;
		}

		if (m_depthStencilView)
		{
			m_depthStencilView->Release();
			m_depthStencilView = nullptr;
		}

		if (m_depthStencilState)
		{
			m_depthStencilState->Release();
			m_depthStencilState = nullptr;
		}

		if (m_depthStencilBuffer)
		{
			m_depthStencilBuffer->Release();
			m_depthStencilBuffer = nullptr;
		}

		if (m_renderTargetView)
		{
			m_renderTargetView->Release();
			m_renderTargetView = nullptr;
		}

		if (m_deviceContext)
		{
			m_deviceContext->Release();
			m_deviceContext = nullptr;
		}

		if (m_device)
		{
			m_device->Release();
			m_device = nullptr;
		}

		if (m_swapChain)
		{
			m_swapChain->Release();
			m_swapChain = nullptr;
		}
	}

	void Direct3D::BeginScene(float r, float g, float b, float a)
	{
		float color[4];

		color[0] = r;
		color[1] = g;
		color[2] = b;
		color[3] = a;

		m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
		m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void Direct3D::EndScene()
	{
		if (m_vSync)
		{
			m_swapChain->Present(1, 0);
			return;
		}

		m_swapChain->Present(0, 0);
	}

	ID3D11Device* Direct3D::GetDevice()
	{
		return m_device;
	}

	ID3D11DeviceContext* Direct3D::GetDeviceContext()
	{
		return m_deviceContext;
	}

	void Direct3D::GetProjectionMatrix(XMMATRIX& projectionMatrix)
	{
		projectionMatrix = m_projectionMatrix;
	}

	void Direct3D::GetWorldMatrix(XMMATRIX& worldMatrix)
	{
		worldMatrix = m_worldMatrix;
	}

	void Direct3D::GetOrthoMatrix(XMMATRIX& orthoMatrix)
	{
		orthoMatrix = m_orthoMatrix;
	}

	void Direct3D::GetVideoCardInfo(char* cardName, int& memory)
	{
		strcpy_s(cardName, 128, m_videoCardDesc);
		memory = m_videoCardMemory;
	}
}
