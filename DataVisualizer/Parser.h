#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <string.h>
#include <streambuf>
#include <cerrno>
#include <iomanip>      // std::setprecision

std::string get_file_contents(const char *filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		return(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
	}
	throw(errno);
}
void removeCharsFromString(std::string &str, char* charsToRemove) {
	for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
		str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
	}
}

double grabDouble(std::string &from)
{
	size_t indexOfNextWS = from.find(' ');
	std::string token = from.substr(0, indexOfNextWS);

	std::string::size_type sz;
	long double grabbedDoub = std::stod(token, &sz);
	if (from.length() > indexOfNextWS + 1)
		from.erase(0, indexOfNextWS + 1);
	return grabbedDoub;
}