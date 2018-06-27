#pragma once
#include <functional>
#include <vector>
#include <memory>
#include <map>

namespace ASGE {
	class Renderer;
	class Input
	{

	public:
		Input();
		virtual ~Input();
		virtual bool init(Renderer*) = 0;
		virtual void update() = 0;

		void sendEvent(int key, int state);

		template<typename T, typename T2>
		int addCallbackFnc(T fncPtr, T2* obj)
		{
			using namespace std::placeholders;
			return this->registerCallback(std::bind(fncPtr, obj, _1, _2));
		}

		template<typename T>
		int addCallbackFnc(T fncPtr)
		{
			using namespace std::placeholders;
			return this->registerCallback(std::bind(fncPtr, _1, _2));
		}

		void unregisterCallback(unsigned int id);

	private:
		using InputFnc = std::function<void(int, int)>;
		int registerCallback(InputFnc);
		using InputFncs = std::vector<InputFnc>;
		InputFncs callback_funcs;

	};
}

