#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include "lodepng.h"
using namespace std;
namespace fs = boost::filesystem;
void processing_all_files( fs::path & full_path );
int print_png_info(const char* filename);
std::string get_png_size(const char* filename);

int main(int argc, char **argv)
{
	fs::path full_path("./", fs::native);
	processing_all_files( full_path );
	char buf[256] = {0};
	cin>>buf;
	return 0;
}
void processing_all_files( fs::path & full_path )
{
	if ( fs::exists(full_path) )
	{
		fs::directory_iterator item_begin(full_path);
		fs::directory_iterator item_end;
		for ( ; item_begin != item_end; item_begin++ )
		{
			if (! fs::is_directory(*item_begin) ){
				std::string file_name  = item_begin->path().filename().string();
				if (file_name.find("bsprite")!=file_name.npos){
					std::string name = file_name.substr(0,file_name.find(".bsprite"));
					std::string png_name = name;
					png_name.append(".png");
					std::string plist_str ="";

					plist_str.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
					plist_str.append("<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n");
					plist_str.append("<plist version=\"1.0\">\n");
					plist_str.append("<dict>\n");
					
					plist_str.append("<key>metadata</key>\n");
					plist_str.append("<dict>\n");
					plist_str.append("<key>format</key>\n");
					plist_str.append("<integer>2</integer>\n");
					plist_str.append("<key>realTextureFileName</key>\n");
					plist_str.append("<string>").append(png_name.c_str()).append("</string>\n");
					plist_str.append("<key>size</key>\n");
					plist_str.append("<string>").append(get_png_size(png_name.c_str())).append("</string>\n");
					plist_str.append("<key>smartupdate</key>\n");
					plist_str.append("<string>$TexturePacker:SmartUpdate:a76ab1f1e427e88f25db62c34a520c0f$</string>\n");
					plist_str.append("<key>textureFileName</key>\n");
					plist_str.append("<string>").append(png_name.c_str()).append("</string>\n");
					plist_str.append("</dict>\n");

					plist_str.append("<key>frames</key>\n");
					plist_str.append("<dict>\n");

					
					FILE *file = fopen(item_begin->path().filename().string().c_str(),"rb");
					char head [7] = {0};
					fread(head,1,7,file);
					printf("file %s ",item_begin->path().filename().string().c_str());
					printf("fram count %d\r\n",head[6]);
					int count = head[6];
					int read_size = count *9;
					char frame [1024] = {0};
					fseek(file,7,0);
					fread(frame,1,read_size,file);
					for (int i = 0;i<count; i++)
					{
							char temp[2]={0};
							char unknow = frame[0+9*i];
							short  x ;
							temp[1] =* (frame +1+9*i);
							temp[0] =* (frame +2+9*i);
							memcpy(&x,temp,2);
				
							short  y ;
							temp[1] =* (frame +3+9*i);
							temp[0] =* (frame +4+9*i);
							memcpy(&y ,temp,2);
						
							short  width ;
							temp[1] =* (frame +5+9*i);
							temp[0] =* (frame +6+9*i);
							memcpy(&width,temp,2);
						
							short  height ;
							temp[1] =* (frame +7+9*i);
							temp[0] =* (frame +8+9*i);
							memcpy(&height,temp,2);
							printf("fram %d {%d,%d,%d,%d}\r\n",i,x,y,width,height);

							char frame[256]={0};
							char sourceColorRect[256]={0};
							char sourceSize[256]={0};
							char key[256]={0};

							sprintf(frame,"{{%d,%d},{%d,%d}}",x,y,width,height);
							sprintf(sourceColorRect,"{{%d,%d},{%d,%d}}",0,0,width,height);
							sprintf(sourceSize,"{%d,%d}",width,height);
							sprintf(key,"{%s_%d.png}",name.c_str(),i);

							plist_str.append("<key>").append(key).append("</key>\n");
							plist_str.append("<dict>\n");
							plist_str.append("<key>frame</key>\n");
							plist_str.append("<string>").append(frame).append("</string>\n");
							plist_str.append("<key>offset</key>\n");
							plist_str.append("<string>{0,0}</string>\n");
							plist_str.append("<key>rotated</key>\n");
							plist_str.append("<false/>\n");
							plist_str.append("<key>sourceColorRect</key>\n");
							plist_str.append("<string>").append(sourceColorRect).append("</string>\n");
							plist_str.append("<key>sourceSize</key>\n");
							plist_str.append("<string>").append(sourceSize).append("</string>\n");
							plist_str.append("</dict>\n");
					}
					plist_str.append("</dict>\n");
					plist_str.append("</dict>\n");
					plist_str.append("</plist>");
					FILE *new_file = fopen(name.append(".plist").c_str(),"wb");
					fwrite(plist_str.c_str(),1,plist_str.size(),new_file);
					fclose(new_file);
				}
			}
		}
	}
} 

std::string get_png_size(const char* filename){
	std::vector<unsigned char> buffer;
	std::vector<unsigned char> image;
	unsigned w, h;
	lodepng::load_file(buffer, filename); //load the image file with given filename

	lodepng::State state;
	unsigned error = lodepng::decode(image, w, h, state, buffer);
	if(error)
	{
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return 0;
	}
	char size_buf[256]={0};
	sprintf(size_buf,"{%d,%d}",w, h);
	return std::string(size_buf);
}
int print_png_info(const char* filename){

	std::vector<unsigned char> buffer;
	std::vector<unsigned char> image;
	unsigned w, h;

	lodepng::load_file(buffer, filename); //load the image file with given filename

	lodepng::State state;
	unsigned error = lodepng::decode(image, w, h, state, buffer);

	if(error)
	{
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return 0;
	}

	LodePNGInfo& info = state.info_png;
	LodePNGColorMode& color = state.info_png.color;

	std::cout << "Width: " << w << std::endl;
	std::cout << "Height: " << h << std::endl;
	std::cout << "Num pixels: " << w * h << std::endl;
	std::cout << "Compression method: " << info.compression_method << std::endl;
	std::cout << "Filter method: " << info.filter_method << std::endl;
	std::cout << "Interlace method: " << info.interlace_method << std::endl;
	std::cout << "Color type: " << color.colortype << std::endl;
	std::cout << "Bit depth: " << color.bitdepth << std::endl;
	std::cout << "Bits per pixel: " << lodepng_get_bpp(&color) << std::endl;
	std::cout << "Channels per pixel: " << lodepng_get_channels(&color) << std::endl;
	std::cout << "Is greyscale type: " << lodepng_is_greyscale_type(&color) << std::endl;
	std::cout << "Can have alpha: " << lodepng_can_have_alpha(&color) << std::endl;
	std::cout << "Palette size: " << color.palettesize << std::endl;
	std::cout << "Has color key: " << color.key_defined << std::endl;
	if(color.key_defined)
	{
		std::cout << "Color key r: " << color.key_r << std::endl;
		std::cout << "Color key g: " << color.key_g << std::endl;
		std::cout << "Color key b: " << color.key_b << std::endl;
	}
	std::cout << "Texts: " << info.text_num << std::endl;
	for(size_t i = 0; i < info.text_num; i++)
	{
		std::cout << "Text: " << info.text_keys[i] << ": " << info.text_strings[i] << std::endl << std::endl;
	}
	std::cout << "International texts: " << info.itext_num << std::endl;
	for(size_t i = 0; i < info.itext_num; i++)
	{
		std::cout << "Text: "
			<< info.itext_keys[i] << ", "
			<< info.itext_langtags[i] << ", "
			<< info.itext_transkeys[i] << ": "
			<< info.itext_strings[i] << std::endl << std::endl;
	}
	std::cout << "Time defined: " << info.time_defined << std::endl;
	if(info.time_defined)
	{
		LodePNGTime& time = info.time;
		std::cout << "year: " << time.year << std::endl;
		std::cout << "month: " << time.month << std::endl;
		std::cout << "day: " << time.day << std::endl;
		std::cout << "hour: " << time.hour << std::endl;
		std::cout << "minute: " << time.minute << std::endl;
		std::cout << "second: " << time.second << std::endl;
	}
	std::cout << "Physics defined: " << info.phys_defined << std::endl;
	if(info.phys_defined)
	{
		std::cout << "physics X: " << info.phys_x << std::endl;
		std::cout << "physics Y: " << info.phys_y << std::endl;
		std::cout << "physics unit: " << info.phys_unit << std::endl;
	}
	if(w > 0 && h > 0)
	{
		std::cout << "Top left pixel color:"
			<< " r: " << (int)image[0]
		<< " g: " << (int)image[1]
		<< " b: " << (int)image[2]
		<< " a: " << (int)image[3]
		<< std::endl;
	}


	// Listing chunks is based on the original file, not the decoded png info.
	unsigned char *chunk, *begin, *end;
	end = &buffer.back() + 1;
	begin = chunk = &buffer.front() + 8;

	std::cout << std::endl << "Chunks:" << std::endl;
	std::cout << "  type : length" << std::endl;
	std::cout << "  ---- : ------" << std::endl;
	while(chunk + 8 < end && chunk >= begin)
	{
		char type[5];
		lodepng_chunk_type(type, chunk);
		if(std::string(type).size() != 4)
		{
			std::cout << "this is probably not a PNG" << std::endl;
			return 0;
		}
		std::cout << "  " << type << " : " << lodepng_chunk_length(chunk) << std::endl;
		chunk = lodepng_chunk_next(chunk);
	}

	//Show ASCII art preview of the image
	if(w > 0 && h > 0)
	{
		std::cout << std::endl << "ASCII Art Preview: " << std::endl;
		unsigned w2 = 40;
		if(w < w2) w2 = w;
		unsigned h2 = h * w2 / w;
		h2 = (h2 * 2) / 3; //compensate for non-square characters in terminal
		if(h2 > (w2 * 2)) h2 = w2 * 2; //avoid too large output

		std::cout << '+';
		for(unsigned x = 0; x < w2; x++) std::cout << '-';
		std::cout << '+' << std::endl;
		for(unsigned y = 0; y < h2; y++)
		{
			std::cout << "|";
			for(unsigned x = 0; x < w2; x++)
			{
				unsigned x2 = x * w / w2;
				unsigned y2 = y * h / h2;
				int r = image[y2 * w * 4 + x2 * 4 + 0];
				int g = image[y2 * w * 4 + x2 * 4 + 1];
				int b = image[y2 * w * 4 + x2 * 4 + 2];
				int a = image[y2 * w * 4 + x2 * 4 + 3];
				int lightness = ((r + g + b) / 3) * a / 255;
				int min = (r < g && r < b) ? r : (g < b ? g : b);
				int max = (r > g && r > b) ? r : (g > b ? g : b);
				int saturation = max - min;
				int letter = 'i'; //i for grey, or r,y,g,c,b,m for colors
				if(saturation > 32)
				{
					int h = lightness >= (min + max) / 2;
					if(h) letter = (min == r ? 'c' : (min == g ? 'm' : 'y'));
					else letter = (max == r ? 'r' : (max == g ? 'g' : 'b'));
				}
				int symbol = ' ';
				if(lightness > 224) symbol = '@';
				else if(lightness > 128) symbol = letter - 32;
				else if(lightness > 32) symbol = letter;
				else if(lightness > 16) symbol = '.';
				std::cout << (char)symbol;
			}
			std::cout << "|";
			std::cout << std::endl;
		}
		std::cout << '+';
		for(unsigned x = 0; x < w2; x++) std::cout << '-';
		std::cout << '+' << std::endl;
	}
}