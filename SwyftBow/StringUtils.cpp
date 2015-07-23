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

//TODO: Add GUI overlay to change these settings
int loopLength = 30;
float saturation = 0.8f;
float value = 0.8f;
string fontName = "Comic Sans MS";
string nameColor = "CCC";
string fontSize = "11";

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

		bool isRoom = prefix[0] == 'b';

		message = StringUtils::StripHTML(message);
		message = StringUtils::Rainbowify(message, isRoom);

		if (isRoom)
		{
			prefix.append("<n" + nameColor + "/><f x" + fontSize + "=\"" + fontName + "\">");
			message = prefix.append(message);
			message.append("\r\n");
		}
		else
		{
			prefix.append("<n" + nameColor + "/><m v=\"1\">");
			message = prefix.append(message);
			message.append("</m>\r\n");
		}

		*text = message;
		return true;
	}
	return false;
}

string StringUtils::Rainbowify(string text, bool isRoom)
{
	char strcolor[200];
	std::string outmsg;

	for (unsigned int i = 0; i < text.size(); i+=2)
	{
		if (hue + (double)(360 / (double)loopLength) > 360)
			hue = 0;
		else
			hue += (double)(360 / (double)loopLength);

		ColorConversion::RGB color = ColorConversion::hsv2rgb(ColorConversion::HSV{ hue, saturation, value });
		
		if (isRoom)
		{
			sprintf_s(strcolor, "<f x%02x%02x%02x=\"\">", (int)(color.r * 0xFF), (int)(color.g * 0xFF), (int)(color.b * 0xFF));
			outmsg.append(strcolor);
			outmsg.append(StringUtils::MakeHTML(text.substr(i, 2)));
			outmsg.append("</f>");
		}
		else
		{
			sprintf_s(strcolor, "<g x%ss%02x%02x%02x=\"%s\">", fontSize.c_str(), (int)(color.r * 0xFF), (int)(color.g * 0xFF), (int)(color.b * 0xFF), fontName.c_str());
			outmsg.append(strcolor);
			outmsg.append(StringUtils::MakeHTML(text.substr(i, 2)));
			outmsg.append("</g>");
		}
	}

	return outmsg;
}

string StringUtils::MakeHTML(string text)
{
	bool replaced = false;
	string result;

	for (unsigned int i = 0; i < text.size(); i++)
	{
		for (int j = 0; j < NUM_ENTITIES; j++)
		{
			if (text.substr(i, 1).compare(entities[j][1]) == 0)
			{
				result.append(entities[j][0]);
				replaced = true;
			}
		}

		if (replaced == false)
				result.append(text.substr(i, 1));
	
		replaced = false;
	}
		
	return result;
}

string StringUtils::StripHTML(string text)
{
	std::regex regex("<[^<>]*>");
	std::string result = std::regex_replace(text, regex, "");

	for (int i = 0; i < NUM_ENTITIES; i++)
		result = ReplaceString(result, entities[i][0], entities[i][1]);

	return result;
}
