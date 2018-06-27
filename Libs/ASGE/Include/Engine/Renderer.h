#pragma once

#include <memory>
#include "Engine/Colours.h"
#include "Engine/Texture.h"

namespace ASGE {

	class  Sprite;
	class  Input;
	struct Font;

	class Renderer
	{

	public:

		enum class RenderLib
		{
			INVALID = -1,
			PDCURSES = 0,
			PDCURSES_W32 = 1,
			GLEW = 2
		}; RenderLib getRenderLibrary();

		Renderer(RenderLib render_lib)
			: lib(render_lib) {}

		virtual ~Renderer() = default;
		void     setClearColour(Colour rgb);

		// interface
		virtual int  loadFont(const char* font, int pt) = 0;
		virtual bool init(int w, int h) = 0;
		virtual bool exit() = 0;
		virtual void preRender() = 0;
		virtual void renderText(const char* str, int x, int y, float scale, const Colour&) = 0;
		virtual void renderText(const char* str, int x, int y, const Colour&) = 0;
		virtual void renderText(const char* str, int x, int y) = 0;
		virtual void renderSprite(Texture&, int pos[2], unsigned int size[2], float rotation, float scale, const Colour&) const = 0;
		virtual void setDefaultTextColour(const Colour&) = 0;
		virtual void setFont(int id) = 0;
		virtual void setWindowTitle(const char* str) = 0;
		virtual void swapBuffers() = 0;
		virtual const Font& getActiveFont() const = 0;
		virtual std::unique_ptr<Sprite> createSprite() = 0;
		virtual std::shared_ptr<Input>  inputPtr() = 0;
		

	protected:
		RenderLib lib = RenderLib::INVALID;
		Colour cls = COLOURS::STEELBLUE;
	};
}