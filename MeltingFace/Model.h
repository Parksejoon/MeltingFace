#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include "ModelType.h"

using namespace DirectX;
using namespace std;

namespace MF
{
	class Model
	{
	private:
		struct VertexType
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};

	public:
		Model();
		Model(const Model&);
		~Model();

		bool Initialize(ID3D11Device*, const char*);
		void Shutdown();
		void Render(ID3D11DeviceContext*);

		int GetIndexCount();

	private:
		bool InitializeBuffers(ID3D11Device*);
		void ShutdownBuffers();
		void RenderBuffers(ID3D11DeviceContext*);
		bool LoadModel(const char*);
		void ReleaseModel();

	private:
		VertexInfo* m_vertexInfos;
		ID3D11Buffer* m_vertexBuffer;
		ID3D11Buffer* m_indexBuffer;
		int m_vertexCount;
		int m_indexCount;
	};
}
