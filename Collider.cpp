#include "Collider.hpp"

// Construcots
Collider::Collider(const Collider& col) : x{ col.x }, y{ col.y }, width{ col.width }, height{ col.height }, type{ col.type } {}

Collider::Collider(double x, double y, double r) : x{ x }, y{ y }, width{ r }, height{ 0 }
{
	this->type = TypeCollider::CIRCLE;
}

Collider::Collider(double x, double y, double width, double height) : x{ x }, y{ y }, width{ width }, height{ height }
{
	this->type = TypeCollider::RECTANGLE;
}

Collider::Collider() : x{ 0 }, y{ 0 }, width{ 0 }, height{ 0 }, type{ TypeCollider::CIRCLE } {}

bool Collider::intersect(const Collider& other)
{
	if (other.type == TypeCollider::RECTANGLE)
		return intersectRectangle(other);
	return intersectCircle(other);
}

// Setters
void Collider::setX(const double& x)
{
	this->x = x;
}

void Collider::setY(const double& y)
{
	this->y = y;
}

void Collider::setW(const double& width)
{
	this->width = width;
}

void Collider::setH(const double& height)
{
	this->height = height;
}

void Collider::setR(const double& radius)
{
	this->width = radius;
}

// Getters
double Collider::getX() const
{
	return this->x;
}

double Collider::getY() const
{
	return this->y;
}

double Collider::getW() const
{
	return this->width;
}

double Collider::getH() const
{
	return this->height;
}

double Collider::getR() const
{
	return this->width;
}

// Operators
Collider& Collider::operator=(const Collider& col)
{
	this->x = col.x;
	this->y = col.y;
	this->width = col.width;
	this->height = col.height;
	this->type = col.type;
	return *this;
}

// Private functions

bool Collider::intersectCircle(const Collider& other)
{
	glm::vec2 v(other.x - this->x, other.y - this->y);
	return glm::length(v) - (this->width + other.width) < 0.0000001f;
}

bool Collider::intersectRectangle(const Collider& other)
{
	bool noOverlap = this->x > other.x + other.width ||
		other.x > this->x + this->width ||
		this->y > other.y + other.height ||
		other.y > this->y + this->height;
	return !noOverlap;
}