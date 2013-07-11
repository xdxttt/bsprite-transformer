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
std::string target_png = "";
int main(int argc, char **argv)
{
	std::string path;
	if(argc>1){
		path = argv[1];
	}else{
		path = "./";
	}
	
	if(argc>2){
		target_png = argv[2];
	
	}

	fs::path full_path(path.c_str(), fs::native);
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
				std::string name = file_name.substr(0,file_name.find(".bsprite"));

				if (file_name.find("bsprite")!=file_name.npos){
					std::string png_name = "";
					if (target_png.length()>0){
						png_name = target_png;
						printf(png_name.c_str());
					}else{
						png_name = name;
						png_name.append(".png");
					}

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
					std::string size_str = get_png_size(png_name.c_str());
					if(size_str.length()==0){
						continue;
					}
					plist_str.append("<string>").append(size_str).append("</string>\n");
					plist_str.append("<key>smartupdate</key>\n");
					plist_str.append("<string>$TexturePacker:SmartUpdate:a76ab1f1e427e88f25db62c34a520c0f$</string>\n");
					plist_str.append("<key>textureFileName</key>\n");
					plist_str.append("<string>").append(png_name.c_str()).append("</string>\n");
					plist_str.append("</dict>\n");

					plist_str.append("<key>frames</key>\n");
					plist_str.append("<dict>\n");

 					FILE *file =NULL;
					int ret = fopen_s(&file,item_begin->path().filename().string().c_str(),"rb");
					char head [7] = {0};
					fread(head,1,7,file);
					printf("file %s ",item_begin->path().filename().string().c_str());
					unsigned char count = head[6];
					printf("fram count %d\r\n",count);
					int read_size = count *9;
					char frame[10240];
				
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
						

						char frame_name[256]={0};
						char sourceColorRect[256]={0};
						char sourceSize[256]={0};
						char key[256]={0};

						sprintf_s(frame_name,"{{%d,%d},{%d,%d}}",x,y,width,height);
						sprintf_s(sourceColorRect,"{{%d,%d},{%d,%d}}",0,0,width,height);
						sprintf_s(sourceSize,"{%d,%d}",width,height);
						sprintf_s(key,"%s_%d.png",name.c_str(),i);

						plist_str.append("<key>").append(key).append("</key>\n");
						plist_str.append("<dict>\n");
						plist_str.append("<key>frame</key>\n");
						plist_str.append("<string>").append(frame_name).append("</string>\n");
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
	
					FILE *new_file ;
					fopen_s(&new_file,name.append(".plist").c_str(),"wb");
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
		return "";
	}
	char size_buf[256]={0};
	sprintf_s(size_buf,"{%d,%d}",w, h);
	return std::string(size_buf);
}