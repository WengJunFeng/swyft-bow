#pragma once
#include <string>
#include <vector>
#include <iostream>

class StringStream 
{
private:
	std::string stream;
	std::vector<std::string> ignoreList;
	int position;

	void reset();
	bool shouldIgnore(int pos, int &jump);

public:
	StringStream(std::string stream, std::vector<std::string> ignoreList);
	std::string get(int num, bool countSpaces);

};

