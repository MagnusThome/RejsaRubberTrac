/*
  Library for 1-d splines
  Copyright Ryan Michael
  => modified for integer operations (less precision, more speed)
  Licensed under the LGPLv3 
*/
#ifndef spline_h
#define spline_h

#include "Arduino.h"

#define Hermite 10
#define Catmull 11

class Spline
{
  public:
    Spline( void );
    Spline( int16_t x[], int16_t y[], int numPoints, int degree = 1 );
    Spline( int16_t x[], int16_t y[], int16_t m[], int numPoints );
    int16_t value( int16_t x );
    void setPoints( int16_t x[], int16_t y[], int numPoints );
    void setPoints( int16_t x[], int16_t y[], int16_t m[], int numPoints );
    void setDegree( int degree );
    
  private:
    int16_t calc( int16_t, int);
    int16_t* _x;
    int16_t* _y;
    int16_t* _m;
    int _degree;
    int _length;
    int _prev_point;
    
    int16_t hermite( int16_t t, int16_t p0, int16_t p1, int16_t m0, int16_t m1, int16_t x0, int16_t x1 );
    int16_t hermite_00( int16_t t );
    int16_t hermite_10( int16_t t );
    int16_t hermite_01( int16_t t );
    int16_t hermite_11( int16_t t );
    int16_t catmull_tangent( int i );
};

#endif
