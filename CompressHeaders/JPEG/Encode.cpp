#define POCKETLZMA_LZMA_C_DEFINE
#include "pocketlzma.hpp"

//Program to find uncompressed header data in a jpeg
//NOTE : Change filename
//Run: g++ Encode.cpp -o Encode.o && ./Encode.o
int main()
{
	//Change file name here
	const std::string fileName = "sample.jpg";
	std::string outputPath = "sample.jpg.middleclass";
	
	//Get file bytes
	std::vector<uint8_t> data;
	plz::FileStatus fileStatus = plz::File::FromFile(fileName, data);
	int headerSize = 0;
	
	//Ensure valid JPEG file
	if(data[0] != 0xFF && data[1] != 0xD8){printf("Invalid JPEG file\n");return -1;}
	
	//Find uncompressed headerSize
	for(int i = 0; i < data.size(); i++, headerSize++){if(data[i] == 0xDA){break;}}
	
	
	//Split file into uncompressed header and huffman data
	std::vector<uint8_t> headerData(data.begin(), data.begin() + headerSize);
	std::vector<uint8_t> huffmanData(data.begin() + headerSize, data.end());

	//Compress Header
	std::vector<uint8_t> compressedHeader;
	plz::PocketLzma p;
        p.usePreset(plz::Preset::GoodCompression); 
        plz::StatusCode status = p.compress(headerData, compressedHeader);
        
        if(compressedHeader.size() > (1 << 16)){printf("Large Header\n");return -1;}
        
        compressedHeader.insert(compressedHeader.begin(), compressedHeader.size() & 0xFF); 
        compressedHeader.insert(compressedHeader.begin(), compressedHeader.size()>>8); 
        compressedHeader.insert(compressedHeader.begin(), (uint8_t) 'C'); 
        compressedHeader.insert(compressedHeader.begin(), (uint8_t) 'M'); 	
        
      	//Combine compressed header and huffman data
        compressedHeader.insert(compressedHeader.end(), huffmanData.begin(), huffmanData.end());
        
        //Write to file
	plz::FileStatus writeStatus = plz::File::ToFile(outputPath, compressedHeader);
	if(writeStatus.status() == plz::FileStatus::Code::Ok)
	{
		//Process completed successfully!
	}

	return 0;
}
