#include "font.h"



Font::Font()
	:QFont("SourceHanSansCN-Regular")
{
	setPixelSize(16);
}

Font::Font(int pixelSize)
	: QFont("SourceHanSansCN-Regular")
{
	setPixelSize(pixelSize);
}

Font::Font(int pixelSize, bool bold)
	: QFont("SourceHanSansCN-Regular")
{
	setPixelSize(pixelSize);
	if (bold) {
		setWeight(bold ? Bold : Normal);
	}
}


Font::~Font()
{
}
