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
static cv::Point2f getPointOnStraightY( straight_t A, double y );
static cv::Point2f getPointOnStraightX( straight_t A, double x );

/* --------------------------------------------------------------------------------------------*/

static straight_t createStraightFrom2Point(cv::Point A, cv::Point B)
{	
	double a, b;

	a = B.y - A.y;
	b = A.x - B.x;
	
	return { a, b, ( a * A.x ) + (b * A.y) }; 
}

static cv::Point2f getPointOnStraightY(straight_t A, double y){
	// formatted_log("y = %f", y);
	// formatted_log("A = %f, B = %f, C = %f", A.coeff.a, A.coeff.b, A.coeff.c);	
	// formatted_log("x = %f", (A.coeff.c - A.coeff.b * y)/A.coeff.a);
	cv::Point2f result(((A.coeff.c - A.coeff.b * y)/A.coeff.a), y);
	// formatted_log("Point on straight [%f, %f]", result.x, result.y);
	return result;
}

static cv::Point2f getPointOnStraightX(straight_t A, double x){
	// formatted_log("x = %f", x);
	// formatted_log("A = %f, B = %f, C = %f", A.coeff.a, A.coeff.b, A.coeff.c);	
	// formatted_log("y = %f", (A.coeff.c - A.coeff.a * x)/A.coeff.b);
	cv::Point2f result(x, ((A.coeff.c - A.coeff.a * x)/A.coeff.b));
	// formatted_log("Point on straight [%f, %f]", result.x, result.y);
	return result;
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