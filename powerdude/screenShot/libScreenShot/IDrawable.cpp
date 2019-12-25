#include "IDrawable.h"

IDrawable::IDrawable(DrawableType type, DrawableProperty pro, QObject *parent)
	: QObject(parent),
	m_type(type),
	m_bCanDrawed(false),
	m_property(pro)
{

}

IDrawable::~IDrawable()
{

}

DrawableType IDrawable::GetType()
{
	return m_type;
}