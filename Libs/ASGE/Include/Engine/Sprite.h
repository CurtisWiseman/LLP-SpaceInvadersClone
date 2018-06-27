#pragma once
#include <memory>

namespace ASGE {
	class Renderer;

	/**
	*  Sprite. An ASGE supported sprite which can be used 
	*  to render and load textures.
	*/

	class Sprite
	{
	public:
		virtual bool loadTexture(const char*) = 0;
		virtual bool render(std::shared_ptr<Renderer>) = 0;

		int position[2] { 0,0 };          /**< Sprite Position. The sprites position on the screen. */
		unsigned int size[2]{ 0,0 };      /**< Sprite Dimensions. The dimensions of the loaded texture file. */
		float rotation = 0.0f;            /**< Sprite Rotation. Rotation around the sprite's origin. */
		float scale = 1.0f;               /**< Sprite Scale. Scales the sprite equally in both dims. */
	};
}