#pragma once
#include <qfont.h>

class Font :
	public QFont
{
	//Q_OBJECT
public:
	Font();
	Font(int pixelSize);
	Font(int pixelSize, bool bold);
	~Font();
};

