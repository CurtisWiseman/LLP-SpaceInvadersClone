#pragma once
class Texture
{
public:
	
	enum Format
	{
		MONOCHROME = 1,
		MONOCHROME_ALPHA = 2,
		RGB = 3,
		RGBA = 4
	};

	void setFormat(Format f) {
		format = f;
	}

	Format getFormat() {
		return format;
	}

	virtual ~Texture() = default;

private:
	Format format = RGB;
};