#pragma once
namespace ASGE{
	struct Font
	{
	public:
		Font() = default;
		virtual ~Font() = default;


		const char* font_name = ""; /**< Font Name.   The name of the font loaded. */
		int font_size = 0;          /**< Font Size.   The size of the font imported. */
		int line_height = 0;        /**< Line Height. The recommended height of each line. */
	};
}