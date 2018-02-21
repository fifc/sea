#ifndef _PIC_CODE_H_
#define _PIC_CODE_H_
#include <string>

class ImgParam {
public:
	std::string text;
	std::string filename;
	int count;
	int width;
	int height;
	int offset;
	bool border;
};

std::string CreatePicVericode(std::string *img);
std::string CreatePicVericode(const std::string& file);

#endif // PIC_CODE_H_
