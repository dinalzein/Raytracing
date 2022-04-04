#ifndef _OBJECT_H
#define _OBJECT_H

#include "Ray.h"
#include "Vector.h"
#include "Color.h"

class Object {
	public:

	Object ();

	// method functions
	virtual Color getColor () {
    return Color (0.0, 0.0, 0.0, 0);
  }

	virtual Vector getNormalAt(Vector intersection_position) {
		return Vector (0, 0, 0);
	}

	virtual double findIntersection(Ray ray) {
		return 0;
	}

};

Object::Object () {}

#endif
