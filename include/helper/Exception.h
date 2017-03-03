#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_


#include <iostream>
#include <sstream>


#define _EXCEPTION(cstr)			(Exception(__FILE__, __LINE__, (cstr)))

class Exception : public std::exception {
public:

	Exception() : std::exception() 
	{ 
		_reason = "Error";
	}

	Exception(const char *c_str) : std::exception() 
	{
		_reason = c_str;
	}

	Exception(const char *c_str, int in_error_code) : std::exception() 
	{
		_reason = c_str;
		_error_code = in_error_code;
	}

	Exception(const char *file, unsigned long line, const char *c_str) : std::exception() 
	{
		std::stringstream sstream;
  		sstream << file << ": " << line << ": " << c_str;
  		
  		_reason = sstream.str();
	}

	Exception(const char *file, unsigned long line, const char *c_str, int in_error_code) : std::exception() 
	{
		_error_code = in_error_code;
		
		std::stringstream sstream;
  		sstream << file << ": " << line << ": " << c_str;
  		
  		_reason = sstream.str();
	}

	Exception(int in_error_code) : std::exception() 
	{
		_error_code = in_error_code;

		std::stringstream sstream;
  		sstream << "Error Code: " << _error_code;
  		
  		_reason = sstream.str();
	}

	const char *what() const throw()
	{	
		return _reason.c_str();
	}

	const int getErrCode() const throw()
	{
		return _error_code;
	}

	~Exception() throw() 
	{

 	}

private:

	std::string _reason;
	int _error_code;
};

#endif