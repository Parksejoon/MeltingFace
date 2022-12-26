#include "LightShader.h"
#include <d3dcompiler.h>

namespace MF
{
	LightShader::LightShader()
	{
		m_vertexShader = nullptr;
		m_pixelShader = nullptr;
		m_layout = nullptr;
		m_samplerState = nullptr;
		m_matrixBuffer = nullptr;
		m_lightBuffer = nullptr;
	}

	LightShader::LightShader(const LightShader& other)
	{
	}

	LightShader::~LightShader()
	{
	}

	bool LightShader::Initailize(ID3D11Device* device, HWND hwnd)
	{
		bool result;

		result = InitializeShader(device, hwnd, L"LightVertexShader.hlsl", L"LightPixelShader.hlsl");
		if (!result)
		{
			return false;
		}

		return true;
	}

	void LightShader::Shutdown()
	{
		ShutdownShader();
	}

	bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
	{
		bool result;

		result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor);
		if (!result)
		{
			return false;
		}

		RenderShader(deviceContext, indexCount);
		return true;
	}

	bool LightShader::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFileName, const WCHAR* psFileName)
	{
		ID3D10Blob* errorMessage = nullptr;
		ID3D10Blob* vertexShaderBuffer = nullptr;
		ID3D10Blob* pixelShaderBuffer = nullptr;

		HRESULT ret = D3DCompileFromFile(
			vsFileName,
			nullptr,
			nullptr,
			"LightVertexShader",
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
			"LightPixelShader",
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

		D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		polygonLayout[1].SemanticName = "TEXCOORD";
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;

		polygonLayout[2].SemanticName = "NORMAL";
		polygonLayout[2].SemanticIndex = 0;
		polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[2].InputSlot = 0;
		polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[2].InstanceDataStepRate = 0;

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

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		ret = device->CreateSamplerState(&samplerDesc, &m_samplerState);
		if (FAILED(ret))
		{
			return false;
		}

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

		D3D11_BUFFER_DESC lightBufferDesc;
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.ByteWidth = sizeof(LightBufferType);
		lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lightBufferDesc.MiscFlags = 0;
		lightBufferDesc.StructureByteStride = 0;

		ret = device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer);
		if (FAILED(ret))
		{
			return false;
		}

		return true;
	}

	void LightShader::ShutdownShader()
	{
		if (m_lightBuffer)
		{
			m_lightBuffer->Release();
			m_lightBuffer = nullptr;
		}

		if (m_matrixBuffer)
		{
			m_matrixBuffer->Release();
			m_matrixBuffer = nullptr;
		}

		if (m_samplerState)
		{
			m_samplerState->Release();
			m_samplerState = nullptr;
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

	void LightShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFileName)
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

	bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
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
		//deviceContext->PSSetShaderResources(0, 1, &texture);

		result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			return false;
		}

		LightBufferType* dataPtr2 = (LightBufferType*)mappedResource.pData;
		dataPtr2->diffuseColor = diffuseColor;
		dataPtr2->lightDirection = lightDirection;
		dataPtr2->padding = 0.0f;

		deviceContext->Unmap(m_lightBuffer, 0);

		bufferNumber = 0;

		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

		return true;
	}

	void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
	{
		deviceContext->IASetInputLayout(m_layout);

		deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
		deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

		deviceContext->PSSetSamplers(0, 1, &m_samplerState);

		deviceContext->DrawIndexed(indexCount, 0, 0);
	}
}