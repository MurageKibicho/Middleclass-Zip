#define POCKETLZMA_LZMA_C_DEFINE
#include "pocketlzma.hpp"

//Program to find uncompressed header data in a jpeg
//NOTE : Change filename
//Run: g++ Encode.cpp -o Encode.o && ./Encode.o
int main()
{
	//Change file name here
	const std::string fileName = "build.jpg";
	std::string outputPath = "sample.jpg.middleclass";
	
	//Get file bytes
	std::vector<uint8_t> data;
	plz::FileStatus fileStatus = plz::File::FromFile(fileName, data);
	int headerSize = 0;
	
	//Split file into left and Right
	int i = 3;
	int j = 1024;
	for(int i = 0; i < 100; i++)
	{

	std::vector<uint8_t> headerData(data.begin() + (data.size() / 2)+(j*i), data.begin() + (data.size() / 2)+(j*(i+1)));

	//Compress Header
	std::vector<uint8_t> compressedHeader;
	plz::PocketLzma p;
        p.usePreset(plz::Preset::BestCompression); 
        plz::StatusCode status = p.compress(headerData, compressedHeader);
        
 	std::cout << compressedHeader.size()<< " "<< (data.size() / 2)+(j*i) << "\n";

	}
	

	return 0;
}
