#pragma once

#ifndef COLLIDER
#define COLLIDER

#include <GLM/geometric.hpp>
#include <GLM/vec2.hpp>

class Collider
{
public:
	Collider(const Collider& collider);
	Collider(double x, double y, double r);
	Collider(double x, double y, double width, double height);

	bool intersect(const Collider& other);

	void setX(const double& x);
	void setY(const double& y);
	void setW(const double& width);
	void setH(const double& height);
	void setR(const double& radius);

	double getX() const;
	double getY() const;
	double getW() const;
	double getH() const;
	double getR() const;

	Collider& operator=(const Collider& col);

private:
	bool intersectCircle(const Collider& other);
	bool intersectRectangle(const Collider& other);

	explicit Collider();

	const enum TypeCollider { CIRCLE, RECTANGLE };

	TypeCollider type;
	double x, y, width, height;
};

#endif // !COLLIDER