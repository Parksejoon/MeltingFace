#pragma once

#include <Windows.h>

#include "Direct3D.h"
#include "Camera.h"
#include "Model.h"
//#include "ColorShader.h"
#include "LightShader.h"
#include "Light.h"

namespace MF
{
	class Graphics
	{
	public:
		static const bool FULL_SCREEN = false;
		static const bool VSYNC_ENABLED = true;
		static constexpr float SCREEN_DEPTH = 1000.0f;
		static constexpr float SCREEN_NEAR = 0.1f;
		
		Graphics();
		Graphics(const Graphics&);
		~Graphics();

		bool Initialize(int, int, HWND);
		void Shutdown();
		bool Frame();

	private:
		bool Render(float);

	private:
		Direct3D* m_Direct3D;
		Camera* m_Camera;
		Model* m_Model;
		//ColorShader* m_ColorShader;
		LightShader* m_LightShader;
		Light* m_Light;
	};
}
