#ifndef __IDRAWABLE_H__
#define __IDRAWABLE_H__

#pragma once

#include <QObject>
#include <QPainter>


enum DrawableType {
	RectangleObj = 0,
	EllipseObj = 1,
	ArrowObj = 2,
	BrushObj = 3,
	TransparentObj = 4,
	MaskObj = 5,
	TextObj = 6
};

enum PenSizeEnum {
	Small = 0,
	Middle = 1,
	Big = 2
};

enum MaskSize {
	Mask_6 = 6,
	Mask_8 = 8,
	Mask_10 = 10,
	Mask_12 = 12,
	Mask_14 = 14,
	Mask_16 = 16
};

union DrawableSize {
	PenSizeEnum PenSizeEnum;
	int PenSizeInt;
	MaskSize MaskSize;
};

struct DrawableProperty {
	DrawableSize size;
	int color;
};

class IDrawable : public QObject
{
	Q_OBJECT

public:
	IDrawable(DrawableType type, DrawableProperty pro,QObject *parent);
	~IDrawable();
	virtual void Draw(QPainter& painter) = 0;

	DrawableType GetType();

private:
	DrawableType m_type;

protected:
	bool m_bCanDrawed;
	DrawableProperty m_property;
};

#endif