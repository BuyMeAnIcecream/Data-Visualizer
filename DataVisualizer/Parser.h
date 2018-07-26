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
#include <experimental/filesystem>
#include <filesystem>
namespace fs = std::experimental::filesystem::v1;

struct Point {
	long double time;
	long double x;
	long double y;
	long double z;
	bool newPath;

	Point* next;

	Point(long double t, long double X, long double Y, long double Z, bool NewPath = false)
	{
		time = t;
		x = X;
		y = Y;
		z = Z;
		next = nullptr;
		newPath = NewPath;

	}
};

std::string getFileContents(const char *filename)
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

Point* parseFolderContent(std::string path)
{
	int count = 1;
	Point* head = nullptr;
	
	size_t pos = 0;

	long double t = 0;
	long double x = 0;
	long double y = 0;
	long double z = 0;
	bool newPath = true; //if this is the head of the locations

	head = new Point(t, x, y, z, newPath);
	//	cameraSpots.push_back(glm::vec3(x, y, z));
	Point* currentPoint = head;
	

	for (auto & p : fs::directory_iterator(path)) {
		//casting operator to string (<3 c++ for the ease of that)
		std::cout << p << std::endl;
		std::ostringstream oss;
		oss << p;

		std::string fileName = oss.str();
		string input = getFileContents(fileName.c_str());
	//	cout << input;
		removeCharsFromString(input, "[],");
		//since it's a beginning of a new file
		newPath = true;
		cout << "here: ";
		do{
			t = grabDouble(input);
			x = grabDouble(input);
			y = grabDouble(input);
			z = grabDouble(input);

			currentPoint->next = new Point(t, x, y, z, newPath);
			
			if (newPath)
				newPath = false;
			
//			cout << " " << currentPoint->x<<'\n';
			currentPoint = currentPoint->next;
			count++;
		} while (pos = input.find(' ') != std::string::npos);
		cout << count << "count";
		
	}
	cout << "here: " << t;
	return head;
}

