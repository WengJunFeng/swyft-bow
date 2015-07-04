#pragma once
class ColorConversion
{
	public:
		typedef struct 
		{
			double r;
			double g;
			double b;
		} RGB;

		typedef struct 
		{
			double h;
			double s;
			double v;
		} HSV;

		static HSV rgb2hsv(RGB in);
		static RGB hsv2rgb(HSV in);
};

