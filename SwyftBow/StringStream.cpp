#include "StringStream.h"
#include <string>

StringStream::StringStream(std::string stream, std::vector<std::string> ignoreList)
{
	this->position = 0;
	this->stream = stream;
	this->ignoreList = ignoreList;
}


void StringStream::reset()
{
	position = 0;
}

// <INTERNAL FUNCTION> is pos in our ignore list?
bool StringStream::shouldIgnore(int pos, int &jump)
{
	for (size_t i = 0; i < ignoreList.size(); i++)
	{
		int ignore_pos = 0;

		while ((ignoreList[i][ignore_pos] == '?' || ignoreList[i][ignore_pos] == stream[pos + ignore_pos]) && ignoreList[i][ignore_pos] != '\0' && stream[pos + ignore_pos] != '\0')
			++ignore_pos;

		if (ignore_pos == ignoreList[i].length())
		{
			jump = ignore_pos;
			return true;
		}
	}

	return false;
}

// get num chars from string, chars on ignore list are appended, but not counted
// returns at maximum num chars, or an empty string when finished
std::string StringStream::get(int num, bool countSpaces = false)
{
	std::string feed = "";
	int jump;

	for (; position < stream.length() && num > 0; position++)
	{
		if (shouldIgnore(position, jump))
		{
			feed += stream.substr(position, jump);
			position += --jump;
		}
		else
		{
			feed += stream[position];

			if (!countSpaces)
				if (stream[position] != ' ')
					--num;
				else
					--num;

		}
	}

	return feed;
}