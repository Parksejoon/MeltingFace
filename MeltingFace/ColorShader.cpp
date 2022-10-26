#include "ColorShader.h"

namespace MF
{
	ColorShader::ColorShader()
	{
		m_vertexShader = nullptr;
		m_pixelShader = nullptr;
		m_layout = nullptr;
		m_matrixBuffer = nullptr;
	}

	ColorShader::~ColorShader()
	{
	}

	bool ColorShader::Initialize(ID3D11Device* device, HWND hwnd)
	{
		return InitializeShader(device, hwnd, L"ColorVertexShader.hlsl", L"ColorPixelShader.hlsl");
	}

	void ColorShader::Shutdown()
	{
		ShutdownShader();
	}

	bool ColorShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
	{
		bool ret = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
		if (!ret)
		{
			return false;
		}

		RenderShader(deviceContext, indexCount);

		return true;
	}

	bool ColorShader::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFileName, const WCHAR* psFileName)
	{
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* vertexShaderBuffer = nullptr;
		ID3DBlob* pixelShaderBuffer = nullptr;

		HRESULT ret = D3DCompileFromFile(
			vsFileName,
			nullptr,
			nullptr,
			"ColorVertexShader",
			"vs_5_0",
			D3D10_SHADER_ENABLE_STRICTNESS,
			0,
			&vertexShaderBuffer,
			&errorMessage);

		if (FAILED(ret))
		{
			if (errorMessage)
			{
				OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
			}
			else
			{
				MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
			}

			return false;
		}

		ret = D3DCompileFromFile(
			psFileName,
			nullptr,
			nullptr,
			"ColorPixelShader",
			"ps_5_0",
			D3D10_SHADER_ENABLE_STRICTNESS,
			0,
			&pixelShaderBuffer,
			&errorMessage);
		if (FAILED(ret))
		{
			if (errorMessage)
			{
				OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
			}
			else
			{
				MessageBox(hwnd, psFileName, L"Missing Shader File", MB_OK);
			}

			return false;
		}

		ret = device->CreateVertexShader(
			vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(),
			nullptr,
			&m_vertexShader);
		if (FAILED(ret))
		{
			return false;
		}

		ret = device->CreatePixelShader(
			pixelShaderBuffer->GetBufferPointer(),
			pixelShaderBuffer->GetBufferSize(),
			nullptr,
			&m_pixelShader);
		if (FAILED(ret))
		{
			return false;
		}

		D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		polygonLayout[1].SemanticName = "COLOR";
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;
		
		unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

		ret = device->CreateInputLayout(
			polygonLayout,
			numElements,
			vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(),
			&m_layout);
		if (FAILED(ret))
		{
			return false;
		}

		vertexShaderBuffer->Release();
		vertexShaderBuffer = nullptr;

		pixelShaderBuffer->Release();
		pixelShaderBuffer = nullptr;

		D3D11_BUFFER_DESC matrixBufferDesc;

		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;

		ret = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
		if (FAILED(ret))
		{
			return false;
		}

		return true;
	}

	void ColorShader::ShutdownShader()
	{
		if (m_matrixBuffer)
		{
			m_matrixBuffer->Release();
			m_matrixBuffer = nullptr;
		}

		if (m_layout)
		{
			m_layout->Release();
			m_layout = nullptr;
		}

		if (m_pixelShader)
		{
			m_pixelShader->Release();
			m_pixelShader = nullptr;
		}

		if (m_vertexShader)
		{
			m_vertexShader->Release();
			m_vertexShader = nullptr;
		}
	}

	void ColorShader::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, const WCHAR* shaderFileName)
	{
		ofstream fout;

		char* compileErrors = (char*)errorMessage->GetBufferPointer();
		unsigned long bufferSize = errorMessage->GetBufferSize();

		fout.open("shader-error.txt");
		for (int i = 0; i < bufferSize; i++)
		{
			fout << compileErrors[i];
		}
		fout.close();

		errorMessage->Release();
		errorMessage = nullptr;

		MessageBox(hwnd, L"Error comiling shader. Check shader-error.txt for message.", shaderFileName, MB_OK);
	}

	bool ColorShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
	{
		worldMatrix = XMMatrixTranspose(worldMatrix);
		viewMatrix = XMMatrixTranspose(viewMatrix);
		projectionMatrix = XMMatrixTranspose(projectionMatrix);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			return false;
		}

		MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
		dataPtr->world = worldMatrix;
		dataPtr->view = viewMatrix;
		dataPtr->projection = projectionMatrix;

		deviceContext->Unmap(m_matrixBuffer, 0);

		unsigned int bufferNumber = 0;

		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

		return true;
	}

	void ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
	{
		deviceContext->IASetInputLayout(m_layout);

		deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
		deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

		deviceContext->DrawIndexed(indexCount, 0, 0);
	}
}