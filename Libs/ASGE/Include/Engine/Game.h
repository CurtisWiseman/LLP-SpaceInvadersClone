#pragma once
#include <memory>
#include "Renderer.h"

namespace ASGE {
	class Game
	{

	public:
		Game() = default;
		~Game() = default;

		virtual bool init() = 0;
		virtual bool run()  = 0;

		virtual void beginFrame() = 0;
		virtual void drawFrame() = 0;
		virtual void endFrame() = 0;

		virtual bool initAPI() = 0;
		virtual bool exitAPI() = 0;

	protected:
		void toggleFPS();
		void updateFPS();

		std::shared_ptr<Renderer> renderer;
		std::shared_ptr<Input>    inputs;

		int  width      = 640;
		int  height     = 480;
		bool show_fps   = false;		
	};
}