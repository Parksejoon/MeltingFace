#include "Input.h"

namespace MF
{
	Input::Input()
	{
	}

	Input::Input(const Input& other)
	{
	}

	Input::~Input()
	{
	}

	void Input::Initialize()
	{
		for (bool& key : m_keys)
		{
			key = false;
		}
	}

	void Input::KeyDown(unsigned int input)
	{
		m_keys[input] = true;
	}

	void Input::KeyUp(unsigned int input)
	{
		m_keys[input] = false;
	}

	bool Input::IsKeyDown(unsigned int input)
	{
		return m_keys[input];
	}
}
