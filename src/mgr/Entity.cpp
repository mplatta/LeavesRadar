#include "Entity.hpp"

bool Entity::isThisEntity(std::string name)
{
	return (this->_name.compare(name) == 0) ? true : false;
}

Entity *Entity::isThisEntityE(std::string name)
{
	return (this->_name.compare(name) == 0) ? this : NULL;
}

Entity Entity::clone()
{
	Entity e;

	e.setId        ( this->_id        );
	e.setName      ( this->_name      );
	e.setContour   ( this->_contour   );
	e.setHierarchy ( this->_hierarchy );
	e.setPointZero ( this->_poit_zero );
	e.setRec       ( this->_rec       );

	return e;
}