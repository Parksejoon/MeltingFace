#pragma once

namespace MF
{
	class Input
	{
	public:
		Input();
		Input(const Input&);
		~Input();

		void Initialize();

		void KeyDown(unsigned int);
		void KeyUp(unsigned int);

		bool IsKeyDown(unsigned int);

	private:
		static const int KeyCount = 256;

		bool m_keys[KeyCount];
	};
}
