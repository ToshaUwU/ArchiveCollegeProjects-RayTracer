#include <fstream>
#include <iostream>
#include "FileWork.h"

#pragma pack(push, 1)
const static struct
{
	uint8_t bfType[2] = {'B', 'M'};
	mutable uint32_t bfSize = 0;
	uint16_t bfReserved1 = 0;
	uint16_t bfReserved2 = 0;
	mutable uint32_t bfOffBits = 0;
} BitmapHeader;

const static struct
{
	uint32_t bcSize = 12;
	mutable uint16_t bcWidth = 0;
	mutable uint16_t bcHeight = 0;
	uint16_t bcPlanes = 1;
	uint16_t bcBitCount = 24;
} BitmapFileInfo;
#pragma pack(pop)

void SaveImage(const char * FileName, uint16_t ImageWidth, uint16_t ImageHeight, uint8_t * PixelData)
{
	uint32_t PixelDataSize = ImageWidth*ImageHeight*3;
	BitmapHeader.bfSize = sizeof(BitmapHeader) + sizeof(BitmapFileInfo) + PixelDataSize;
	BitmapHeader.bfOffBits = sizeof(BitmapHeader) + sizeof(BitmapFileInfo);
	BitmapFileInfo.bcWidth = ImageWidth;
	BitmapFileInfo.bcHeight = ImageHeight;
	std::ofstream BmpFile(FileName, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
	BmpFile.write(reinterpret_cast<const char *>(&BitmapHeader), sizeof(BitmapHeader));
	BmpFile.write(reinterpret_cast<const char *>(&BitmapFileInfo), sizeof(BitmapFileInfo));
	BmpFile.write(reinterpret_cast<const char *>(PixelData), PixelDataSize);
	BmpFile.close();
}

GLuint LoadShader(const char * FileName, GLenum ShaderType)
{
	GLuint Shader;
	uint16_t SourceInputSize;
	Shader = glCreateShader(ShaderType);
	GLchar * ShaderSource;

	std::ifstream ShaderFile(FileName, std::ios_base::in | std::ios_base::binary);
	if(!ShaderFile.is_open())
	{
		std::cout << "Can't find shader file: " << FileName << std::endl;
		return 0;
	}

	ShaderFile.seekg(0, std::ios_base::end);
	SourceInputSize = static_cast<uint16_t>(ShaderFile.tellg()) + 1;
	ShaderFile.seekg(0);

	ShaderSource = new GLchar[SourceInputSize]{};
	ShaderFile.read(ShaderSource, SourceInputSize - 1);

	GLchar * ptrShaderSource[] = {ShaderSource};
	glShaderSource(Shader, 1, ptrShaderSource, nullptr);
	glCompileShader(Shader);
	delete [] ShaderSource;

	GLint CompileStatus;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);
	if(!CompileStatus)
	{
		GLchar Log[512];
		glGetShaderInfoLog(Shader, 512, nullptr, Log);
		std::cout << "Shader compilation error:\n";
		std::cout << Log;
		return 0;
	}

	return Shader;
}
