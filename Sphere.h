#ifndef _SPHERE_H
#define _SPHERE_H

#include "math.h"
#include "Object.h"
#include "Vector.h"
#include "Color.h"

class Sphere : public Object {
	Vector center;
	double radius;
	Color color;

	public:

	Sphere ();

	Sphere (Vector, double, Color);

	// method functions
	Vector getSphereCenter () {
    return center;
  }
	double getSphereRadius () {
    return radius;
  }
	virtual Color getColor () {
    return color;
  }

	virtual Vector getNormalAt(Vector point) {
		// normal always points away from the center of a sphere
		Vector normal_Vect = point.vectorAdd(center.negative()).normalize();
		return normal_Vect;
	}

	virtual double findIntersection(Ray ray) {
		Vector ray_origin = ray.getRayOrigin();
		double ray_origin_x = ray_origin.getVectorX();
		double ray_origin_y = ray_origin.getVectorY();
		double ray_origin_z = ray_origin.getVectorZ();

		Vector ray_direction = ray.getRayDirection();
		double ray_direction_x = ray_direction.getVectorX();
		double ray_direction_y = ray_direction.getVectorY();
		double ray_direction_z = ray_direction.getVectorZ();

		Vector sphere_center = center;
		double sphere_center_x = sphere_center.getVectorX();
		double sphere_center_y = sphere_center.getVectorY();
		double sphere_center_z = sphere_center.getVectorZ();

		double a = 1; // normalized
		double b = (2*(ray_origin_x - sphere_center_x)*ray_direction_x) + (2*(ray_origin_y - sphere_center_y)*ray_direction_y) + (2*(ray_origin_z - sphere_center_z)*ray_direction_z);
		double c = pow(ray_origin_x - sphere_center_x, 2) + pow(ray_origin_y - sphere_center_y, 2) + pow(ray_origin_z - sphere_center_z, 2) - (radius*radius);

		double discriminant = b*b - 4*c;

		if (discriminant > 0) {
			/// the ray intersects the sphere

			// the first root
			double root_1 = ((-1*b - sqrt(discriminant))/2) - 0.000001;

			if (root_1 > 0) {
				// the first root is the smallest positive root
				return root_1;
			}
			else {
				// the second root is the smallest positive root
				double root_2 = ((sqrt(discriminant) - b)/2) - 0.000001;
				return root_2;
			}
		}
		else {
			// the ray missed the sphere
			return -1;
		}
	}

};

Sphere::Sphere () {
	center = Vector(0,0,0);
	radius = 1.0;
	color = Color(0.5,0.5,0.5, 0);
}

Sphere::Sphere (Vector centerValue, double radiusValue, Color colorValue) {
	center = centerValue;
	radius = radiusValue;
	color = colorValue;
}

#endif
