#include "Model.h"

namespace MF
{
	Model::Model()
	{
		m_vertexBuffer = nullptr;
		m_indexBuffer = nullptr;
		m_vertexInfos = nullptr;
	}

	Model::Model(const Model& other)
	{
	}

	Model::~Model()
	{
	}

	bool Model::Initialize(ID3D11Device* device, const char* modelFilename)
	{
		if (!LoadModel(modelFilename))
		{
			return false;
		}

		if (!InitializeBuffers(device))
		{
			return false;
		}

		return true;
	}

	void Model::Shutdown()
	{
		ShutdownBuffers();
		ReleaseModel();
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

		vertices = new VertexType[m_vertexCount];
		indices = new unsigned long[m_indexCount];
		
		for (int i = 0; i < m_vertexCount; i++)
		{
			VertexInfo info = m_vertexInfos[i];
			vertices[i].position = XMFLOAT3(info.x, info.y, info.z);
			vertices[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			
			indices[i] = i;
		}

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

	// TODO 아래 함수 내용 클래스로 분할하기
	bool Model::LoadModel(const char* fileName)
	{
		ifstream fin;
		char input;

		fin.open(fileName);
		if (fin.fail())
		{
			return false;
		}

		int vertexCount = 0;
		int textureCount = 0;
		int normalCount = 0;
		int faceCount = 0;

		fin.get(input);
		while (!fin.eof())
		{
			if (input == 'v')
			{
				fin.get(input);

				if (input == ' ') vertexCount++;
				if (input == 't') textureCount++;
				if (input == 'n') normalCount++;
			}

			if (input == 'f')
			{
				fin.get(input);

				if (input == ' ') faceCount++;
			}

			while (input != '\n')
			{
				fin.get(input);
			}

			fin.get(input);
		}

		fin.clear();
		fin.seekg(0, ios::beg);

		struct FaceInfo
		{
			int vIndex1, vIndex2, vIndex3;
			int tIndex1, tIndex2, tIndex3;
			int nIndex1, nIndex2, nIndex3;
		};

		XMFLOAT3* vertices = new XMFLOAT3[vertexCount];
		XMFLOAT3* texcoords = new XMFLOAT3[textureCount];
		XMFLOAT3* normals = new XMFLOAT3[normalCount];
		FaceInfo* faces = new FaceInfo[faceCount];

		int vertexIndex = 0;
		int texcoordIndex = 0;
		int normalIndex = 0;
		int faceIndex = 0;

		fin.get(input);
		while (!fin.eof())
		{
			if (input == 'v')
			{
				fin.get(input);

				if (input == ' ')
				{
					fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

					// 왼손 좌표계로 변환
					vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
					vertexIndex++;
				}

				if (input == 't')
				{
					fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

					texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
					texcoordIndex++;
				}

				if (input == 'n')
				{
					fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

					normals[normalIndex].z = normals[normalIndex].z * -1.0f;
					normalIndex++;
				}
			}

			if (input == 'f')
			{
				fin.get(input);

				char foo;

				if (input == ' ')
				{
					fin >> faces[faceIndex].vIndex3 >> foo >> faces[faceIndex].tIndex3 >> foo >> faces[faceIndex].nIndex3
						>> faces[faceIndex].vIndex2 >> foo >> faces[faceIndex].tIndex2 >> foo >> faces[faceIndex].nIndex2
						>> faces[faceIndex].vIndex1 >> foo >> faces[faceIndex].tIndex1 >> foo >> faces[faceIndex].nIndex1;
					
					faceIndex++;
				}
			}

			while (input != '\n')
			{
				fin.get(input);
			}

			fin.get(input);
		}

		fin.close();

		m_vertexCount = faceCount * 3;
		m_indexCount = m_vertexCount;
		m_vertexInfos = new VertexInfo[m_vertexCount];
		int vertexInfoIndex = 0;

		for (int i = 0; i < faceCount; i++)
		{
			int vIndex;
			int tIndex;
			int nIndex;

			vIndex = faces[i].vIndex1 - 1;
			tIndex = faces[i].tIndex1 - 1;
			nIndex = faces[i].nIndex1 - 1;

			m_vertexInfos[vertexInfoIndex].x = vertices[vIndex].x;
			m_vertexInfos[vertexInfoIndex].y = vertices[vIndex].y;
			m_vertexInfos[vertexInfoIndex].z = vertices[vIndex].z;

			m_vertexInfos[vertexInfoIndex].tu = texcoords[tIndex].x;
			m_vertexInfos[vertexInfoIndex].tv = texcoords[tIndex].y;
			
			m_vertexInfos[vertexInfoIndex].nx = normals[nIndex].x;
			m_vertexInfos[vertexInfoIndex].ny = normals[nIndex].y;
			m_vertexInfos[vertexInfoIndex].nz = normals[nIndex].z;

			vertexInfoIndex++;


			vIndex = faces[i].vIndex2 - 1;
			tIndex = faces[i].tIndex2 - 1;
			nIndex = faces[i].nIndex2 - 1;

			m_vertexInfos[vertexInfoIndex].x = vertices[vIndex].x;
			m_vertexInfos[vertexInfoIndex].y = vertices[vIndex].y;
			m_vertexInfos[vertexInfoIndex].z = vertices[vIndex].z;

			m_vertexInfos[vertexInfoIndex].tu = texcoords[tIndex].x;
			m_vertexInfos[vertexInfoIndex].tv = texcoords[tIndex].y;

			m_vertexInfos[vertexInfoIndex].nx = normals[nIndex].x;
			m_vertexInfos[vertexInfoIndex].ny = normals[nIndex].y;
			m_vertexInfos[vertexInfoIndex].nz = normals[nIndex].z;

			vertexInfoIndex++;


			vIndex = faces[i].vIndex3 - 1;
			tIndex = faces[i].tIndex3 - 1;
			nIndex = faces[i].nIndex3 - 1;

			m_vertexInfos[vertexInfoIndex].x = vertices[vIndex].x;
			m_vertexInfos[vertexInfoIndex].y = vertices[vIndex].y;
			m_vertexInfos[vertexInfoIndex].z = vertices[vIndex].z;

			m_vertexInfos[vertexInfoIndex].tu = texcoords[tIndex].x;
			m_vertexInfos[vertexInfoIndex].tv = texcoords[tIndex].y;

			m_vertexInfos[vertexInfoIndex].nx = normals[nIndex].x;
			m_vertexInfos[vertexInfoIndex].ny = normals[nIndex].y;
			m_vertexInfos[vertexInfoIndex].nz = normals[nIndex].z;

			vertexInfoIndex++;
		}
	}

	void Model::ReleaseModel()
	{
		if (m_vertexInfos)
		{
			delete[] m_vertexInfos;
			m_vertexInfos = nullptr;
		}
	}
}

