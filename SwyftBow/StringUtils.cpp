#include "StringUtils.h"
#include "ColorConversion.h"
#include  <iomanip>
#include  <iostream>

const int NUM_ENTITIES = 5;

std::string entities[NUM_ENTITIES][2] = {
		{ "&amp;", "&" },
		{ "&gt;", ">" },
		{ "&lt;", "<" },
		{ "&apos;", "'" },
		{ "&quot;" "\"" },
};

double hue = 0;

string ReplaceString(std::string subject, const std::string& search, const std::string& replace)
{
	size_t pos = 0;
	
	while ((pos = subject.find(search, pos)) != std::string::npos) 
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}

	return subject;
}

//Name color regex: <n(\w*)\/>
//Message filter regex: b?msg:(.*):(.*)

bool StringUtils::ProcessMessage(string* text)
{
	regex filter("(b?msg:([^:]*):)(.*)\r\n");
	
	if (regex_match(*text, filter))
	{
		std::smatch sm;
		std::string prefix;
		std::string param;
		std::string message;

		regex_match(*text, sm, filter);

		prefix = sm[1];
		param = sm[2];
		message = sm[3];

		message = StringUtils::StripHTML(message);
		message = StringUtils::Rainbowify(message, (prefix[0] == 'b'));

		prefix.append("<n7/><m v=\"1\">");
		message = prefix.append(message);
		message.append("</m>\r\n");

		*text = message;
		return true;
	}
	return false;
}

string StringUtils::Rainbowify(string text, bool isRoom)
{
	char strcolor[30];
	std::string outmsg;

	//TODO: Add GUI overlay to change these settings
	int loopLength = 30;
	int fontSize = 11;
	float saturation = 0.8f;
	float value = 0.8f;
	

	for (unsigned int i = 0; i < text.size(); i+=2)
	{
		if (hue + (double)(360 / (double)loopLength) > 360)
			hue = 0;
		else
			hue += (double)(360 / (double)loopLength);

		ColorConversion::RGB color = ColorConversion::hsv2rgb(ColorConversion::HSV{ hue, saturation, value });
		
		if (isRoom)
		{
			sprintf_s(strcolor, "<f x%d%02x%02x%02x=\"0\">", fontSize, (int)(color.r * 0xFF), (int)(color.g * 0xFF), (int)(color.b * 0xFF));
			outmsg.append(strcolor);
			outmsg.append(text.substr(i, 2));
		}
		else
		{
			sprintf_s(strcolor, "<g x%ds%02x%02x%02x=\"\">", fontSize, (int)(color.r * 0xFF), (int)(color.g * 0xFF), (int)(color.b * 0xFF));
			outmsg.append(strcolor);
			outmsg.append(text.substr(i, 2));
			outmsg.append("</g>");
		}
	}

	return outmsg;
}

string StringUtils::MakeHTML(string text)
{
	for (int i = 0; i < NUM_ENTITIES; i++)
		text = ReplaceString(text, entities[i][1], entities[i][0]);

	return text;
}

string StringUtils::StripHTML(string text)
{
	std::regex regex("<[^<>]*>");
	std::string result = std::regex_replace(text, regex, "");

	for (int i = 0; i < NUM_ENTITIES; i++)
		result = ReplaceString(result, entities[i][0], entities[i][1]);

	return result;
}
