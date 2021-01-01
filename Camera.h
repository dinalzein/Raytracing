#ifndef _CAMERA_H
#define _CAMERA_H
#include "Vector.h"

class Camera{
  Vector position, direction, right, down;

  public:

  //constructors
  Camera();
  Camera(Vector, Vector, Vector, Vector);

  //getters
  Vector getCameraPosition(){
    return position;
  }

  Vector getCameraDirection(){
    return direction;
  }

  Vector getCameraRight(){
    return right;
  }

  Vector getCameraDown(){
    return down;
  }

};

Camera:: Camera(){
  position=Vector(0,0,0);
  direction=Vector(0,0,1);  // default camera position
  right= Vector(0,0,0);
  down= Vector(0,0,0);
}

Camera:: Camera(Vector pos, Vector dir, Vector r, Vector d){
  position=pos;
  direction=dir;
  right= r;
  down= d;
}

#endif
