#define POCKETLZMA_LZMA_C_DEFINE
#include "pocketlzma.hpp"

//Program to decompress header data in a jpeg
//Run: g++ Decode.cpp -o Decode.o && ./Decode.o
int main()
{
	const std::string fileName = "sample.jpg.middleclass";
	std::string outputPath = "sample(decoded).jpg";

	//Get file bytes
	std::vector<uint8_t> data;
	plz::FileStatus fileStatus = plz::File::FromFile(fileName, data);
	
	//Ensure valid Middleclass file
	if(data[0] != 'M' && data[1] != 'C'){printf("Invalid Middleclass file\n");return -1;}
	
	//Read compressed header size
	int compressedHeaderSize = (data[2] << 8) + data[3];

	//Split file into compressed header and huffman data	
	std::vector<uint8_t> compressedHeader(data.begin()+4, data.begin() + 4 + compressedHeaderSize);
	std::vector<uint8_t> headerData;
	
	//Decompress Header
	plz::PocketLzma p;
        plz::StatusCode status = p.decompress(compressedHeader, headerData);
        if(status == plz::StatusCode::Ok)
        {
        	//Ensure valid jpeg file
        	if(headerData[0] != 0xFF && headerData[1] != 0xD8){printf("Unable to Decompress file\n");return -1;}
        	
        	//Combine uncompressed header and huffman data
        	headerData.insert(headerData.end(), data.begin() + 4 + compressedHeaderSize, data.end());
        	
        	//Write jpeg to file
		plz::FileStatus writeStatus = plz::File::ToFile(outputPath, headerData);
		if(writeStatus.status() == plz::FileStatus::Code::Ok)
		{
			//Success
       	}
	}
	return 0;
}
