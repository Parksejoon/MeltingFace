#include "Model.h"

namespace MF
{
	Model::Model()
	{
		m_vertexBuffer = nullptr;
		m_indexBuffer = nullptr;
	}

	Model::Model(const Model& other)
	{
	}

	Model::~Model()
	{
	}

	bool Model::Initialize(ID3D11Device* device)
	{
		return InitializeBuffers(device);
	}

	void Model::Shutdown()
	{
		ShutdownBuffers();
	}

	void Model::Render(ID3D11DeviceContext* deviceContext)
	{
		RenderBuffers(deviceContext);
	}

	int Model::GetIndexCount()
	{
		return m_indexCount;
	}

	bool Model::InitializeBuffers(ID3D11Device* device)
	{
		VertexType* vertices;
		unsigned long* indices;

		D3D11_BUFFER_DESC vertexBufferDesc;
		D3D11_BUFFER_DESC indexBufferDesc;

		D3D11_SUBRESOURCE_DATA vertexData;
		D3D11_SUBRESOURCE_DATA indexData;

		HRESULT result;

		m_vertexCount = 3;
		m_indexCount = 3;

		vertices = new VertexType[m_vertexCount];
		indices = new unsigned long[m_indexCount];

		vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
		vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

		vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

		vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
		vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		vertexData.pSysMem = vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
		if (FAILED(result))
		{
			return false;
		}

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		indexData.pSysMem = indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
		if (FAILED(result))
		{
			return false;
		}

		delete[] vertices;
		vertices = nullptr;

		delete[] indices;
		indices = nullptr;

		return true;
	}

	void Model::ShutdownBuffers()
	{
		if (m_indexBuffer)
		{
			m_indexBuffer->Release();
			m_indexBuffer = nullptr;
		}

		if (m_vertexBuffer)
		{
			m_vertexBuffer->Release();
			m_vertexBuffer = nullptr;
		}
	}

	void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
	{
		unsigned int stride;
		unsigned int offset;

		stride = sizeof(VertexType);
		offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}

