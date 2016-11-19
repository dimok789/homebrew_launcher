#ifndef ___COLORXML_H_
#define ___COLORXML_H_

#include <string>
#include <vector>
#include "common/types.h"

typedef struct _ColorData
{
	u8 R;
	u8 G;
	u8 B;
	u8 A;
} ColorData;

class ColorXML
{
	public:
		ColorXML() { };
		ColorXML(const char* filename) { LoadColorXMLData(filename); };

		bool LoadColorXMLData(const char* filename);

		ColorData GetColor1();
		ColorData GetColor2();
		ColorData GetColor3();
		ColorData GetColor4();
	private:
		ColorData color1 = {79, 153, 239, 255};
		ColorData color2 = {59, 153, 223, 255};
		ColorData color3 = {42, 159, 217, 255};
		ColorData color4 = {13, 104, 133, 255};
};

#endif
