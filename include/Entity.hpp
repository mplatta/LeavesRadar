#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_ value

#include <iostream>
#include <string>
// #include <stdlib.h>
// #include <stdio.h>

#include "opencv_headers.hpp"
#include "formatted_log.hpp"

class Entity
{
private:
	std::string            _name;
	std::vector<cv::Point> _contour;
	std::vector<cv::Vec4i> _hierarchy;

public:
	// getters
	std::string            getName      () { return this->_name;      };
	std::vector<cv::Point> getContour   () { return this->_contour;   };
	std::vector<cv::Vec4i> getHierarchy () { return this->_hierarchy; };

	// setters
	void setName      ( std::string name                 ) { this->_name      = name;      };
	void setContour   ( std::vector<cv::Point> contour   ) { this->_contour   = contour;   };
	void setHierarchy ( std::vector<cv::Vec4i> hierarchy ) { this->_hierarchy = hierarchy; };

	// func
	bool isThisEntity     ( std::string name ); 
	Entity *isThisEntityE ( std::string name ); 

	// constructors/destructors
	Entity()  {};
	~Entity() {};

	Entity( std::string name, std::vector<cv::Point> contour, std::vector<cv::Vec4i> hierarchy ) {
		this->_name = name;
		this->_contour = contour;
		this->_hierarchy = hierarchy;
	};

	Entity( std::string name, std::vector<cv::Point> contour ) {
		this->_name = name;
		this->_contour = contour;
	};
	
};

#endif