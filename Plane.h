#ifndef _Plane_H
#define _Plane_H

#include "math.h"
#include "Object.h"
#include "Vector.h"
#include "Color.h"

class Plane : public Object {
	Vector normal;
	double distance;
	Color color;

	public:

	Plane ();

	Plane (Vector, double, Color);

	// method functions
	Vector getPlaneNormal () {
    return normal;
  }
	double getPlaneDistance () {
    return distance;
  }
	virtual Color getColor () {
    return color;
  }

	virtual Vector getNormalAt(Vector point) {
		return normal;
	}

	virtual double findIntersection(Ray ray) {
		Vector ray_direction = ray.getRayDirection();

		double a = ray_direction.dotProduct(normal);

		if (a == 0) {
			// ray is parallel to the plane
			return -1;
		}
		else {
			double b = normal.dotProduct(ray.getRayOrigin().vectorAdd(normal.vectorMult(distance).negative()));
			return -1*b/a;
		}
	}

};

Plane::Plane () {
	normal = Vector(1,0,0);
	distance = 0;
	color = Color(0.5,0.5,0.5, 0);
}

Plane::Plane (Vector normalValue, double distanceValue, Color colorValue) {
	normal = normalValue;
	distance = distanceValue;
	color = colorValue;
}

#endif
