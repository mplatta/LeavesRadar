#include "cartographer.hpp"

/* ------------ PRIVATE FUNCTIONS ------------*/
void Cartographer::getBorder() 
{
	//TODO: function to get border from image and save it to bordered
	;
}

/* ------------ PUBLIC FUNTIONS ------------*/
void Cartographer::setSrcImg(Mat src) 
{
	//TODO: add some allert
	if (!src) return;

	this->bordered = src;
}

Mat Cartographer::getBordered()
{
	return this->bordered;
}

Cartographer::Cartographer() 
{
	this->bordered = NULL;
}