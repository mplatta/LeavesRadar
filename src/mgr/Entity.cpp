#include "Entity.hpp"

bool Entity::isThisEntity(std::string name)
{
	return (this->_name.compare(name) == 0) ? true : false;
}