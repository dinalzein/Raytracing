#ifndef _Light_H
#define _Light_H

#include "Source.h"
#include "Vector.h"
#include "Color.h"

class Light : public Source {
	Vector position;
	Color color;

	public:

	Light ();

	Light (Vector, Color);

	// method functions
	virtual Vector getLightPosition () {
    return position;
  }

	virtual Color getLightColor () {
     return color;
  }

};

Light::Light () {
	position = Vector(0,0,0);
	color = Color(1,1,1, 0);
}

Light::Light (Vector p, Color c) {
	position = p;
	color = c;
}

#endif
