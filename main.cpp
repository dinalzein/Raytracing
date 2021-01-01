#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<cmath>
#include<limits>
#include<stdlib.h>
#include<time.h>

#include "Vector.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Source.h"
#include "Light.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
using namespace std;

struct RGBType{
  double red;
  double green;
  double blue;
};
void savebmp(const char *filename, int width, int height, int dpi, RGBType*data){
  FILE *f;
  int k=width*height; //total number of pixels
  int s=4*k;
  int filesize=54+s;
  double factor=39.375;
  int m=static_cast<int>(factor);
  int ppm=dpi*m;
  unsigned char bmpfileheader[14]={'B', 'M', 0,0,0,0 ,0,0,0,0, 54,0,0,0};
  unsigned char bmpinfoheader[40]={40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};
  bmpfileheader[ 2]=(unsigned char)(filesize);
  bmpfileheader[ 3]=(unsigned char)(filesize>>8);
  bmpfileheader[ 4]=(unsigned char)(filesize>>16);
  bmpfileheader[ 5]=(unsigned char)(filesize>>24);

  bmpinfoheader[ 4]=(unsigned char)(width);
  bmpinfoheader[ 5]=(unsigned char)(width>>8);
  bmpinfoheader[ 6]=(unsigned char)(width>>16);
  bmpinfoheader[ 7]=(unsigned char)(width>>24);

  bmpinfoheader[ 8]=(unsigned char)(height);
  bmpinfoheader[ 9]=(unsigned char)(height>>8);
  bmpinfoheader[ 10]=(unsigned char)(height>>16);
  bmpinfoheader[ 11]=(unsigned char)(height>>24);

  bmpinfoheader[ 21]=(unsigned char)(s);
  bmpinfoheader[ 22]=(unsigned char)(s>>8);
  bmpinfoheader[ 23]=(unsigned char)(s>>16);
  bmpinfoheader[ 24]=(unsigned char)(s>>24);

  bmpinfoheader[ 25]=(unsigned char)(ppm);
  bmpinfoheader[ 26]=(unsigned char)(ppm>>8);
  bmpinfoheader[ 27]=(unsigned char)(ppm>>16);
  bmpinfoheader[ 28]=(unsigned char)(ppm>>24);

  bmpinfoheader[ 29]=(unsigned char)(ppm);
  bmpinfoheader[ 30]=(unsigned char)(ppm>>8);
  bmpinfoheader[ 31]=(unsigned char)(ppm>>16);
  bmpinfoheader[ 32]=(unsigned char)(ppm>>24);

  f=fopen(filename, "wb");
  fwrite(bmpfileheader,1,14,f);
  fwrite(bmpinfoheader,1,40,f);

  for(int i=0; i<k; i++){
    RGBType rgb=data[i];
    double red=(data[i].red)*255;
    double green=(data[i].green)*255;
    double blue=(data[i].blue)*255;

    unsigned char color[3]={(int)floor(blue),(int)floor(green), (int)floor(red)};
    fwrite(color,1,3,f);
  }

  fclose(f);
}

int winningObjectIndex(vector<double> object_intersections){

  //return the index of the winning intersection,(smallest one)
  int index_of_winning_object;
  // no intersections
  if(object_intersections.size()==0){
    return -1;
  }
  else if(object_intersections.size()==1){
    if(object_intersections.at(0)>0){
      return 0; //index of the intersection
    }
    else{
      //intersection is negative, so no interscetion
      return -1;
    }
  }
  else{
    double max=0;
    for (int i=0; i<object_intersections.size(); i++){
      if (object_intersections.at(i)>max){
        max=object_intersections.at(i);
      }
    }
    //find the minimum value from the max value
    if(max>0){
      for(int index=0; index<object_intersections.size(); index++){
        //first cond just to re-confirm
        if(object_intersections.at(index)>0 && object_intersections.at(index)<=max){
          max=object_intersections.at(index);
          index_of_winning_object =index;
        }
      }
      return index_of_winning_object;
    }
    else{
      // all the intersections are negative and so there is no intersection
      return -1;
    }

  }

}

Color getColorAt(Vector intersection_position, Vector intersecting_ray_direction, vector<Object*>scene_objects, int index_of_winning_object, vector<Source*>light_sources, double accuracy, double ambientlight){

  Color winning_object_color = scene_objects.at(index_of_winning_object)->getColor();
  Vector winning_object_normal = scene_objects.at(index_of_winning_object)->getNormalAt(intersection_position);

  if (winning_object_color.getColorSpecial()==2){
    //tile floor pattern
    int square= (int)floor(intersection_position.getVectorX()+(int)floor(intersection_position.getVectorZ()));
    if(square%2==0){
      //changing the color of the winning object to black atthat pixel
      winning_object_color.setColorRed(0);
      winning_object_color.setColorGreen(0);
      winning_object_color.setColorBlue(0);

    }
    else{
      //white color
      winning_object_color.setColorRed(1);
      winning_object_color.setColorGreen(1);
      winning_object_color.setColorBlue(1);
    }
  }

  Color final_color = winning_object_color.colorScalar(ambientlight);

  //check what the ray intersects with first so that we can reflect the light of that object
  //if ray intersects with the tilefloor, we have to reflect the color of the tile floor to the sphere
  if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
		// reflection from objects with specular intensity
		double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
		Vector scalar1 = winning_object_normal.vectorMult(dot1);
		Vector add1 = scalar1.vectorAdd(intersecting_ray_direction);
		Vector scalar2 = add1.vectorMult(2);
		Vector add2 = intersecting_ray_direction.negative().vectorAdd(scalar2);
		Vector reflection_direction = add2.normalize();

		Ray reflection_ray (intersection_position, reflection_direction);

		// determine what the ray intersects with first
		vector<double> reflection_intersections;

		for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++) {
			reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
		}

		int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

		if (index_of_winning_object_with_reflection != -1) {
			// reflection ray missed everthing else
			if (reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy) {
				// determine the position and direction at the point of intersection with the reflection ray
				// the ray only affects the color if it reflected off something

				Vector reflection_intersection_position = intersection_position.vectorAdd(reflection_direction.vectorMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
				Vector reflection_intersection_ray_direction = reflection_direction;

				Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

				final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(winning_object_color.getColorSpecial()));
			}
		}
	}

  for (int light_index = 0; light_index < light_sources.size(); light_index++) {
		Vector light_direction = light_sources.at(light_index)->getLightPosition().vectorAdd(intersection_position.negative()).normalize();

		float cosine_angle = winning_object_normal.dotProduct(light_direction);

		if (cosine_angle > 0) {
			// test for shadows
			bool shadowed = false;

			Vector distance_to_light = light_sources.at(light_index)->getLightPosition().vectorAdd(intersection_position.negative()).normalize();
			float distance_to_light_magnitude = distance_to_light.magnitude();

			Ray shadow_ray (intersection_position, light_sources.at(light_index)->getLightPosition().vectorAdd(intersection_position.negative()).normalize());

			vector<double> secondary_intersections;

			for (int object_index = 0; object_index < scene_objects.size() && shadowed == false; object_index++) {
				secondary_intersections.push_back(scene_objects.at(object_index)->findIntersection(shadow_ray));
			}

			for (int c = 0; c < secondary_intersections.size(); c++) {
				if (secondary_intersections.at(c) > accuracy) {
					if (secondary_intersections.at(c) <= distance_to_light_magnitude) {
						shadowed = true;
					}
				}
				break;
			}

			if (shadowed == false) {
				final_color = final_color.colorAdd(winning_object_color.colorMultiply(light_sources.at(light_index)->getLightColor()).colorScalar(cosine_angle));

				if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
					// special [0-1]
					double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
					Vector scalar1 = winning_object_normal.vectorMult(dot1);
					Vector add1 = scalar1.vectorAdd(intersecting_ray_direction);
					Vector scalar2 = add1.vectorMult(2);
					Vector add2 = intersecting_ray_direction.negative().vectorAdd(scalar2);
					Vector reflection_direction = add2.normalize();

					double specular = reflection_direction.dotProduct(light_direction);
					if (specular > 0) {
						specular = pow(specular, 10);
						final_color = final_color.colorAdd(light_sources.at(light_index)->getLightColor().colorScalar(specular*winning_object_color.getColorSpecial()));
					}
				}
			}
		}
	}

	return final_color.clip();

}

//nested for loop to loop over each pixel, then created a ray at each pixel
//check if the ray intersects wiht anything
//scale the color of the object at the intersection with the color of the light
//so we create a rendering of shadow

int main(int argc, char *argv[]){
  cout << "rendering..." << endl;
  clock_t t1, t2;
  t1=clock();
  int index;
  int dpi=72;
  int width=640;
  int height= 480;
  int n=width*height; // total number of pixels in an image
  RGBType*pixels= new RGBType[n];

  int aadepth=4;
  double aathreshold=0.1;
  double ratio =(double)width/(double)height;
  double ambientlight=0.2;
  double accuracy =0.000001; //0.00000001

  // vectors for x,y,z directions
  Vector O(0,0,0);
  Vector X(1,0,0);
  Vector Y(0,1,0);
  Vector Z(0,0,1);

  Vector new_sphere_location(2,0,0); //right of the other sphere

  //define camera vectors
  Vector camera_position (3, 1.5, -4);
  Vector point(0,0,0); // camera look at this point initially
  Vector diff_btw (camera_position.getVectorX() - point.getVectorX(), camera_position.getVectorY() - point.getVectorY(), camera_position.getVectorZ() - point.getVectorZ());
  Vector camera_direction =diff_btw.negative().normalize();
  Vector camera_right= Y.crossProduct(camera_direction).normalize();
  Vector camera_down= camera_right.crossProduct(camera_direction);

  Camera camera_scene (camera_position, camera_direction, camera_right, camera_down);


  //create colors
  Color white_light(1.0, 1.0, 1.0, 0);
  Color green_color(0.5, 1, 0.5, 0.3);
  Color marron_color(0.5, 0.25, 0.25, 0);
  Color tile_floor(1,1,1,2);
  Color gray_color(0.5, 0.5, 0.5, 0);
  Color black_color(0, 0, 0, 0);

  //create light source


  Vector light_position(-7,10,-10);
  Light light_scene(light_position, white_light);

  vector <Source*> light_sources;
  light_sources.push_back(dynamic_cast<Source*>(&light_scene));

  //object sphere
  Sphere sphere_scene(O,1, green_color);
  Sphere sphere_scene2(new_sphere_location,0.5, marron_color);

  //create a Plane
  //distance between center of sphere and center of plane =-1, sphere is at origin, plane benhind the sphere
  Plane plane_scene(Y, -1, tile_floor); // 2 for the tilecolor

  //create a vector of objects
  vector <Object*> scene_objects;
  //add sphere to it
  scene_objects.push_back(dynamic_cast<Object*>(&sphere_scene));
  scene_objects.push_back(dynamic_cast<Object*>(&sphere_scene2));
  scene_objects.push_back(dynamic_cast<Object*>(&plane_scene));

  int aa_index;
  double xamnt,yamnt;
  double tempRed, tempBlue, tempGreen;

  for(int i=0; i<width; i++){
    for(int j=0; j<height; j++){
      index= j*width+i; //pixel in the photo

      //add anti aliasing
      //start with a black pixel
      double tempRed[aadepth*aadepth];
      double tempGreen[aadepth*aadepth];
      double tempBlue[aadepth*aadepth];

      //add double for loop for anti  aliasing
      for(int aax=0; aax<aadepth; aax++){
        for(int aay=0;  aay<aadepth; aay++){
          aa_index=aay*aadepth +aax;
          srand(time(0));
          // create a ray from the camera to this pixel
          //aadepth=1  for alising
          if(aadepth==1){

            //start with no anti-aliasing
            if(width>height){
              //the image is wider here
              xamnt=((i+0.5)/width)*ratio -(((width-height)/(double)height)/2);
              yamnt= ((height-j)+0.5)/height;
            }
            else if (height>width){
              //the image is taller here
              xamnt= (i+0.5)/width;
              yamnt= (((height-j)+0.5)/height)/ratio - (((height-width)/(double)width)/2);
            }
            else{
              //square image
              xamnt=(i+0.5)/width;
              yamnt= ((height-j)+0.5)/height;
            }
          }
          else{
            // anti-aliasing

            if(width>height){
              //the image is wider here
              xamnt=((i+(double) aax/((double)aadepth -1))/width)*ratio -(((width-height)/(double)height)/2);
              yamnt= ((height-j)+(double) aax/((double)aadepth -1))/height;
            }
            else if (height>width){
              //the image is taller here
              xamnt= (i+(double) aax/((double)aadepth -1))/width;
              yamnt= (((height-j)+(double) aax/((double)aadepth -1))/height)/ratio - (((height-width)/(double)width)/2);
            }
            else{
              //square image
              xamnt=(i+(double) aax/((double)aadepth -1))/width;
              yamnt= ((height-j)+(double) aax/((double)aadepth -1))/height;
            }
          }

          //creating rays
          Vector camera_ray_origin= camera_scene.getCameraPosition();
          Vector camera_ray_direction= camera_direction.vectorAdd(camera_right.vectorMult(xamnt-0.5).vectorAdd(camera_down.vectorMult(yamnt-0.5))).normalize();

          Ray camera_ray(camera_ray_origin, camera_ray_direction);

          vector<double> intersections; //check intersections between objects and rays


          //find the intersection between the objects and the camera ray we create
          for(int k=0; k<scene_objects.size(); k++){
            intersections.push_back(scene_objects.at(k)->findIntersection(camera_ray));
          }
          // find the smallest intersection
          int index_of_winning_object= winningObjectIndex(intersections);

          //cout<< index_of_winning_object;
          if(index_of_winning_object==-1){ //restrict the color of the image to a certain size
            //assigning a color to every single pixel
            //assigning a black color to every missed ray (no intersection between ray and the object)
            tempRed[aa_index]=0;
            tempGreen[aa_index]=0;
            tempBlue[aa_index]=0;
          }
          else{
              //index corresponds to an object (plane or sphere)
            if (intersections.at(index_of_winning_object) > accuracy){
              //determine direction and origin of the vector at the point of intersections
              Vector intersection_position = camera_ray_origin.vectorAdd(camera_ray_direction.vectorMult(intersections.at(index_of_winning_object)));
							Vector intersecting_ray_direction = camera_ray_direction;

              //this one is for getting the color at the intersection
							Color intersection_color = getColorAt(intersection_position, intersecting_ray_direction, scene_objects, index_of_winning_object, light_sources, accuracy, ambientlight);

              //this line is when the color of the pixel is the same as the color of the object
              //Color this_color = scene_objects.at(index_of_winning_object)->getColor();
							tempRed[aa_index] = intersection_color.getColorRed();
							tempGreen[aa_index] = intersection_color.getColorGreen();
							tempBlue[aa_index] = intersection_color.getColorBlue();

            }

          }

        }
      }

      //avaerge pixel color
      double totalRed = 0;
      double totalGreen = 0;
      double totalBlue = 0;

      for (int iRed = 0; iRed < aadepth*aadepth; iRed++) {
        totalRed = totalRed + tempRed[iRed];
      }
      for (int iGreen = 0; iGreen < aadepth*aadepth; iGreen++) {
        totalGreen = totalGreen + tempGreen[iGreen];
      }
      for (int iBlue = 0; iBlue < aadepth*aadepth; iBlue++) {
        totalBlue = totalBlue + tempBlue[iBlue];
      }

      double avgRed = totalRed/(aadepth*aadepth);
      double avgGreen = totalGreen/(aadepth*aadepth);
      double avgBlue = totalBlue/(aadepth*aadepth);

      pixels[index].red=avgRed;
      pixels[index].blue=avgGreen;
      pixels[index].green=avgBlue;

    }
  }


  savebmp("anti_aliased.bmp", width, height, dpi, pixels);

  delete pixels, tempRed, tempBlue, tempGreen;
  t2=clock();
  float diff= ((float)t2 -(float)t1)/1000;
  cout << diff << "seconds" <<endl;

  return 0;
}
