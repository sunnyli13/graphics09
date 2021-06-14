#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gmath.h"
#include "matrix.h"
#include "ml6.h"


/*============================================
  IMPORANT NOTE
  Ambient light is represeneted by a color value
  Point light sources are 2D arrays of doubles.
       - The fist index (LOCATION) represents the vector to the light.
       - The second index (COLOR) represents the color.
  Reflection constants (ka, kd, ks) are represened as arrays of
  doubles (red, green, blue)
  ============================================*/


//lighting functions
color get_lighting( double *normal, double *view, color alight, double light[2][3], double *areflect, double *dreflect, double *sreflect) {
  color i, a, d, s;
  a = calculate_ambient(alight, areflect);
  d = calculate_diffuse(light, dreflect, normal);
  s = calculate_specular(light, sreflect, view, normal);

  i.red = a.red + d.red + s.red;
  i.green = a.green + d.green + s.green;
  i.blue = a.blue + d.blue + s.blue;

  limit_color(&i);
  return i;
}

color calculate_ambient(color alight, double *areflect ) {
  color a;
  a.red = alight.red * areflect[RED];
  a.green = alight.green * areflect[GREEN];
  a.blue = alight.blue * areflect[BLUE];

  limit_color(&a);
  return a;
}

color calculate_diffuse(double light[2][3], double *dreflect, double *normal ) {
  color d;
  double product;
  double L[3];
  L[0] = light[LOCATION][0];
  L[1] = light[LOCATION][1];
  L[2] = light[LOCATION][2];

  normalize(L);
  normalize(normal);

  product = dot_product(normal, L);
  if(product < 0) product = 0;

  d.red = light[COLOR][RED] * dreflect[RED] * product;
  d.green = light[COLOR][GREEN] * dreflect[GREEN] * product;
  d.blue = light[COLOR][BLUE] * dreflect[BLUE] * product;

  limit_color(&d);
  return d;
}

color calculate_specular(double light[2][3], double *sreflect, double *view, double *normal ) {
  color s;
  double product;
  double L[3];
  double R[3];
  L[0] = light[LOCATION][0];
  L[1] = light[LOCATION][1];
  L[2] = light[LOCATION][2];

  normalize(L);
  normalize(normal);
  product = dot_product(normal, L);

  R[0] = 2 * normal[0] * product - L[0];
  R[1] = 2 * normal[1] * product - L[1];
  R[2] = 2 * normal[2] * product - L[2];

  product = dot_product(R, view);
  if(product < 0) product = 0; 

  s.red = light[COLOR][RED] * sreflect[RED] * pow(product, SPECULAR_EXP);
  s.green = light[COLOR][GREEN] * sreflect[GREEN] * pow(product, SPECULAR_EXP);
  s.blue = light[COLOR][BLUE] * sreflect[BLUE] * pow(product, SPECULAR_EXP);
  
  limit_color(&s);
  return s;
}

//limit each component of c to a max of 255
void limit_color( color * c ) {
  if(c->red < 0) c->red = 0;
  if(c->green < 0) c->green = 0;
  if(c->blue < 0) c->blue = 0;
  
  if(c->red > 255) c->red = 255;
  if(c->green > 255) c->green = 255;
  if(c->blue > 255) c->blue = 255;
}

//vector functions
//normalize vetor, should modify the parameter
void normalize( double *vector ) {
  double magnitude;
  magnitude = sqrt( vector[0] * vector[0] +
                    vector[1] * vector[1] +
                    vector[2] * vector[2] );
  int i;
  for (i=0; i<3; i++) {
    vector[i] = vector[i] / magnitude;
  }
}

//Return the dot porduct of a . b
double dot_product( double *a, double *b ) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}


//Calculate the surface normal for the triangle whose first
//point is located at index i in polygons
double *calculate_normal(struct matrix *polygons, int i) {

  double A[3];
  double B[3];
  double *N = (double *)malloc(3 * sizeof(double));

  A[0] = polygons->m[0][i+1] - polygons->m[0][i];
  A[1] = polygons->m[1][i+1] - polygons->m[1][i];
  A[2] = polygons->m[2][i+1] - polygons->m[2][i];

  B[0] = polygons->m[0][i+2] - polygons->m[0][i];
  B[1] = polygons->m[1][i+2] - polygons->m[1][i];
  B[2] = polygons->m[2][i+2] - polygons->m[2][i];

  N[0] = A[1] * B[2] - A[2] * B[1];
  N[1] = A[2] * B[0] - A[0] * B[2];
  N[2] = A[0] * B[1] - A[1] * B[0];

  return N;
}