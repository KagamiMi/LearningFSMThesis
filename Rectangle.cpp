#include "Rectangle.h"

Rectangle::Rectangle():xMin(0.0),xMax(0.0),yMin(0.0),yMax(0.0)
{
}

Rectangle::Rectangle(float xMin, float xMax, float yMin, float yMax):
	xMin(xMin),xMax(xMax),yMin(yMin),yMax(yMax)
{
}

void Rectangle::moveX(float xChange)
{
	xMin += xChange;
	xMax += xChange;
}

void Rectangle::moveY(float yChange)
{
	yMin += yChange;
	yMax += yChange;
}

void Rectangle::setPosition(float xMin, float xMax, float yMin, float yMax)
{
	this->xMin = xMin;
	this->xMax = xMax;
	this->yMin = yMin;
	this->yMax = yMax;
}

ColliderWall Rectangle::collision(Rectangle collider, float *collideValue)
{
	bool collideLeft = collider.xMin <= this->xMax && collider.xMax > this->xMax;
	bool collideRight = collider.xMax >= this->xMin && collider.xMin < this->xMin;
	bool collideMiddleHorizontal = collider.xMin >= this->xMin && collider.xMax <= this->xMax;
	
	bool collideBottom = collider.yMin <= this->yMax && collider.yMax > this->yMax;
	bool collideTop = collider.yMax >= this->yMin && collider.yMin < this->yMin;
	bool collideMiddleVertical = collider.yMin >= this->yMin && collider.yMax <= this->yMax;


	//float collideValue = 0;

	if (collideLeft && collideTop)
	{
		if (this->xMax - collider.xMin < collider.yMax - this->yMin)
		{
			*collideValue = this->xMax - collider.xMin;
			return Left;
		}
		else
		{
			*collideValue = collider.yMax - this->yMin;
			return Top;
		}
	}
	else if (collideLeft && collideBottom)
	{
		if (this->xMax - collider.xMin < this->yMax - collider.yMin)
		{
			*collideValue = this->xMax - collider.xMin;
			return Left;
		}
		else
		{
			*collideValue = this->yMax - collider.yMin;
			return Bottom;
		}
	}
	else if (collideRight && collideTop)
	{
		if (collider.xMax - this->xMin < collider.yMax - this->yMin)
		{
			*collideValue = collider.xMax - this->xMin;
			return Right;
		}
		else
		{
			*collideValue = collider.yMax - this->yMin;
			return Top;
		}
	}
	else if (collideRight && collideBottom)
	{
		if (collider.xMax - this->xMin < this->yMax - collider.yMin)
		{
			*collideValue = collider.xMax - this->xMin;
			return Right;
		}
		else
		{
			*collideValue = this->yMax - collider.yMin;
			return Bottom;
		}
	}
	else if (collideLeft && collideMiddleVertical)
	{
		*collideValue = this->xMax - collider.xMin;
		return Left;
	}
	else if (collideRight && collideMiddleVertical)
	{
		*collideValue = collider.xMax - this->xMin;
		return Right;
	}
	else if (collideTop && collideMiddleHorizontal)
	{
		*collideValue = collider.yMax - this->yMin;
		return Top;
	}
	else if (collideBottom && collideMiddleHorizontal)
	{
		*collideValue = this->yMax - collider.yMin;
		return Bottom;
	}
	else if (collideMiddleHorizontal && collideMiddleVertical)
	{
		*collideValue = 0;
		return Inside;
	}

	*collideValue = 0;
	return None;
}

std::list<float> Rectangle::getVerticesData()
{
	std::list<float> vertices = { xMin, yMin, 0.0,
									xMax, yMin, 0.0,
									xMin, yMax, 0.0,
									xMax, yMax, 0.0 };

	return vertices;
}

void Rectangle::getCenter(float* xVal, float* yVal)
{
	*xVal = xMin + (xMax - xMin) / 2.0;
	*yVal = yMin + (yMax - yMin) / 2.0;
}

float Rectangle::getXCenter()
{
	return xMin + (xMax - xMin) / 2.0;
}

float Rectangle::getYCenter()
{
	return yMin + (yMax - yMin) / 2.0;
}

float Rectangle::getXMin()
{
	return xMin;
}

float Rectangle::getXMax()
{
	return xMax;
}

float Rectangle::getYMin()
{
	return yMin;
}

float Rectangle::getYMax()
{
	return yMax;
}

