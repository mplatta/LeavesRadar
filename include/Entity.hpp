#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_ value

#include <iostream>
#include <string>

#include "opencv_headers.hpp"
#include "formatted_log.hpp"
#include "rectification.hpp"

class Entity
{
private:
	int                    _id       ;
	std::string            _name     ;
	std::vector<cv::Point> _contour  ;
	std::vector<cv::Vec4i> _hierarchy;
	// cv::Mat                _img      ;
	cv::Point              _poit_zero;
	Rectification          _rec      ;

public:
	// getters
	int                    getId        () { return this->_id       ; };
	std::string            getName      () { return this->_name     ; };
	std::vector<cv::Point> getContour   () { return this->_contour  ; };
	std::vector<cv::Vec4i> getHierarchy () { return this->_hierarchy; };
	// cv::Mat                getImage     () { return this->_img      ; };
	cv::Point              getPointZero () { return this->_poit_zero; };
	Rectification          getRect      () { return this->_rec      ; };

	// setters
	void setId        ( int                    id        ) { this->_id        = id       ; };
	void setName      ( std::string            name      ) { this->_name      = name     ; };
	void setContour   ( std::vector<cv::Point> contour   ) { this->_contour   = contour  ; };
	void setHierarchy ( std::vector<cv::Vec4i> hierarchy ) { this->_hierarchy = hierarchy; };
	// void setImage     ( cv::Mat                image     ) { this->_img       = image    ; };
	void setPointZero ( cv::Point              poit_zero ) { this->_poit_zero = poit_zero; };
	void setRec       ( Rectification          rec       ) { this->_rec       = rec      ; };

	// func
	bool   isThisEntity   ( std::string name ); 
	Entity *isThisEntityE ( std::string name );
	Entity clone          ( );

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

	// Entity( std::string name, cv::Mat image ) {
	// 	this->_name = name;
	// 	this->_img = image;
	// };

	Entity( std::string name, Rectification rec, cv::Point poit_zero ) {
		this->_name = name;
		this->_rec = rec;
		this->_poit_zero = poit_zero;
	}
	
};

#endif