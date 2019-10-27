#ifndef _DIR_FUNCIONS_HPP_
#define _DIR_FUNCIONS_HPP_

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include "formatted_log.hpp"

static bool        dir_exists         ( const char  *path );
static std::string delete_last_slash  ( std::string  path );
static std::string extract_name       ( std::string  path );

// ---------------- IMPLEMENTATION --------------------

static bool dir_exists(const char *path)
{
	struct stat info;

	if( stat( path, &info ) != 0 )
    	formatted_err( "Cannot access %s", path );
	else if( info.st_mode & S_IFDIR )
	{
    	formatted_log( "%s directory exists", path );

    	return true;
	}
	else
    	formatted_err( "%s is no directory", path );
	
	return false;
}

static std::string delete_last_slash(std::string path)
{
	if (path[path.length() - 1] == '/') 
	{	
		printf("%s\n", path.substr(0, path.length() - 1).c_str());
		return path.substr(0, path.length() - 1);
	}
	else return path;
}

static std::string extract_name(std::string path)
{
	std::string result;
	int l = 0;
	// formatted_log("%s", path.c_str());
	for (int i = path.length() - 1; i >= 0; --i, ++l)
	{	
		// formatted_log("%c", path[i]);
		if (path[i] == '/') 
		{
			result = path.substr(i + 1, l);
			break;
		}
	}

	return result;
}


#endif