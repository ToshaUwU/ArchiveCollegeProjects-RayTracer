#ifndef FILEWORK_H_INCLUDED
#define FILEWORK_H_INCLUDED

#include <GL/modgl.h>

void SaveImage(const char * FileName, uint16_t ImageWidth, uint16_t ImageHeight, uint8_t * PixelData);

GLuint LoadShader(const char * FileName, GLenum ShaderType);

#endif // FILEWORK_H_INCLUDED
