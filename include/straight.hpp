#ifndef _STRAIGHT_HPP_
#define _STRAIGHT_HPP_

#include "opencv_headers.hpp"
#include "formatted_log.hpp"

typedef struct Coeff_t
{
	double a;
	double b;
	double c;
} coeff_t;

// Line is represented as ax + by = c 
typedef union Straight_t
{
	double m[3];
	coeff_t coeff;
} straight_t;

static straight_t createStraightFrom2Point ( cv::Point  A, cv::Point  B );
static cv::Point  *isIntersectStright_t    ( straight_t A, straight_t B );

/* --------------------------------------------------------------------------------------------*/

static straight_t createStraightFrom2Point(cv::Point A, cv::Point B)
{	
	formatted_log("Createing straight");

	double a, b;

	a = B.y - A.y;
	b = A.x - B.x;
	
	return { a, b, ( a * A.x ) + (b * A.y) }; 
}

static cv::Point *isIntersectStright_t(straight_t A, straight_t B) 
{
	double det = ( A.coeff.a * B.coeff.b ) - 
	             ( B.coeff.a * A.coeff.b );

	if (det == 0) 
	{
		formatted_log("Lines not intersecting");

		return NULL;
	}

	double x = (double)( ( B.coeff.b * A.coeff.c ) - 
				 ( A.coeff.b * B.coeff.c ) 
			   ) / det ;

	double y = (double)( ( A.coeff.a * B.coeff.c ) - 
				 ( B.coeff.a * A.coeff.c ) 
			   ) / det ;

	cv::Point *result = new cv::Point();
	
	result -> x = x;
	result -> y = y;

	formatted_log("Lines intersecting in (double) (%f, %f)", x, y);
	formatted_log("Lines intersecting in (int   ) (%d, %d)", (int)x, (int)y);

	return result;
}

#endif