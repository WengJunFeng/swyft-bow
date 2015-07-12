#pragma once
#include <string>
#include <vector>
#include <iostream>

class StringStream 
{
private:
	std::string stream;
	std::vector<std::string> &ignoreList;
	int position;
	bool shouldIgnore(int pos, int &jump);

public:
	StringStream(std::string stream, std::vector<std::string> ignoreList) : stream(stream), ignoreList(ignoreList), position(0) {}
	std::string get(int num, bool countSpaces);
	void reset();
};

