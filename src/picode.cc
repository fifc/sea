
#include <string>
#include <memory>
#include <iostream>


#include "picode.h"
#define cimg_display 0
#define cimg_use_png 1
#include <CImg.h>

using namespace cimg_library;

static void GenVericode(char *text, int len) {
	for (int i = 0; i < len; ++i) {
		auto k = static_cast<int>(cimg::rand(0, 10000))%62;
		if (k < 26) {
			text[i] = k + 'A';
		} else if (k < 52) {
			text[i] = k - 26 + 'a';
		} else {
			text[i] = k - 52 + '0';
		}
	}
	text[len] = 0;
}

/*
args
0-验证码文字
1-保存文件名
2-验证码文字长度
3-图片宽度
4-图片高度
5-偏移
return 文件的生成路径给node
*/
std::shared_ptr<CImg<unsigned char>> CreateTextImg(const ImgParam& args) {
   const char *text = args.text.c_str();
   int count = args.count;
   int width = args.width;
   int height = args.height;
   int offset = args.offset;
   bool border = args.border;

  // Create captcha image
  //----------------------

  // Write colored and distorted text
  CImg<unsigned char> color(3);
  auto img = std::make_shared<CImg<unsigned char>>(width,height,1,3,0);
  CImg<unsigned char>& captcha = *img.get();

  char letter[2] = { 0 };
  for (unsigned int k = 0; k < count; ++k) {
    CImg<unsigned char> tmp;
    *letter = text[k];
    if (*letter) {
      cimg_forX(color,i) color[i] = (unsigned char)(128 + (std::rand()%127));
      tmp.draw_text((int)(2 + 8*cimg::rand()),
                    (int)(12*cimg::rand()),
                    letter,color.data(),0,1,std::rand()%2?38:57).resize(-100,-100,1,3);
      const unsigned int dir = std::rand()%4, wph = tmp.width() + tmp.height();
      cimg_forXYC(tmp,x,y,v) {
        const int val = dir==0?x + y:(dir==1?x + tmp.height() - y:(dir==2?y + tmp.width() - x:
                                                                   tmp.width() - x + tmp.height() - y));
        tmp(x,y,v) = (unsigned char)std::max(0.0f,std::min(255.0f,1.5f*tmp(x,y,v)*val/wph));
      }
      if (std::rand()%2) tmp = (tmp.get_dilate(3)-=tmp);
      tmp.blur((float)cimg::rand()*0.8f).normalize(0,255);
      const float sin_offset = (float)cimg::rand(-1,1)*3, sin_freq = (float)cimg::rand(-1,1)/7;
      cimg_forYC(captcha,y,v) captcha.get_shared_row(y,0,v).shift((int)(4*std::cos(y*sin_freq + sin_offset)));
      captcha.draw_image(6 + 40*k,tmp);
    }
  }

  // Add geometric and random noise
  CImg<unsigned char> copy = (+captcha).fill(0);
  for (unsigned int l = 0; l<3; ++l) {
    if (l) copy.blur(0.5f).normalize(0,148);
    for (unsigned int k = 0; k<10; ++k) {
      cimg_forX(color,i) color[i] = (unsigned char)(128 + cimg::rand()*127);
      if (cimg::rand()<0.5f) copy.draw_circle((int)(cimg::rand()*captcha.width()),
                                              (int)(cimg::rand()*captcha.height()),
                                              (int)(cimg::rand()*30),
                                              color.data(),0.6f,~0U);
      else copy.draw_line((int)(cimg::rand()*captcha.width()),
                          (int)(cimg::rand()*captcha.height()),
                          (int)(cimg::rand()*captcha.width()),
                          (int)(cimg::rand()*captcha.height()),
                          color.data(),0.6f);
    }
  }
  captcha|=copy;
  // captcha.noise(10,2);

  if (border)
    captcha.draw_rectangle(0,0,captcha.width() - 1,captcha.height() - 1,
                           CImg<unsigned char>::vector(255,255,255).data(),1.0f,~0U);
  captcha = (+captcha).fill(255) - captcha;

  return img;
}

static std::string CreatePicVericode(FILE *fp) {
	ImgParam args;
	char text[7];
	GenVericode(text, (sizeof text) - 1);
	args.text = text;
	args.count = args.text.size();
	args.width = 256;
	args.height =  64;
	args.border =  false;
	 // args.fontSize = 24;

	auto img = CreateTextImg(args);

  	img->save_png(fp);

	return text;
}

std::string CreatePicVericode(std::string *img) {
	char *ptr = nullptr;
	size_t size = 0;
	auto fp = open_memstream(&ptr, &size);
	auto vcode = CreatePicVericode(fp);
	fclose(fp);
	if (ptr != nullptr) {
		img->assign(ptr,size);
		free(ptr);
	}
	return vcode;
}

std::string CreatePicVericode(const std::string& file) {
	FILE *fp = fopen(file.c_str(), "w");
	if (fp == nullptr) return std::string();
	auto vcode = CreatePicVericode(fp);
	fclose(fp);
	return vcode;
}

#ifdef TEST_PIC_CODE
int main(int argc, char *argv[]) {
	std::srand(std::time(0));
	std::string buf;
	std::string vcode = CreatePicVericode(&buf);
	std::cerr << "verify code: " << vcode << ", data size: " << buf.size() << "\n";
	write(1,buf.data(),buf.size());
}
#endif // TEST_PIC_CODE
