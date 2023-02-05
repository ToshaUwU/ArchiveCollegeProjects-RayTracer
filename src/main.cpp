#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cmath>
#include <cctype>
#include <SDL.h>
#include <3rdparty/opengl/gl.h>
#include <system/FileWork.h>

const float pi = 3.141592f;
const float speed = 0.05f;
const float rotationSpeed = 0.0008f;
struct Camera
{
	float x;
	float y;
	float z;
	float xAngle;
	float yAngle;
};

#pragma pack(push, 1)
struct Sphere
{
	GLfloat Coords[3];
	GLfloat r;
	GLfloat Color[3];
	GLfloat ReflectionRatio;
	GLfloat SpecularRatio;
	GLfloat SpecularScale;
	unsigned int :32;
	unsigned int :32;
};

struct Plane
{
	GLfloat Normal[3];
	GLfloat Distance;
	GLfloat Color[3];
	GLfloat ReflectionRatio;
	GLfloat SpecularRatio;
	GLfloat SpecularScale;
	unsigned int :32;
	unsigned int :32;
};

struct Cube
{
	GLfloat Coords[3];
	GLfloat EdgeHalfLength;
	GLfloat Color[3];
	GLfloat ReflectionRatio;
	GLfloat SpecularRatio;
	GLfloat SpecularScale;
	unsigned int :32;
	unsigned int :32;
};
#pragma pack(pop)

const size_t SpheresCount = 7;
const Sphere Spheres[SpheresCount] =
{
	{{3.3, -2.75, 5.8}, 0.3, {0.3, 0.23, 0.3}, 0.01, 1.0, 1.0},
	{{4.0, -2.45, 5.75}, 0.4, {0.9, 0.9, 0.8}, 0.899, 1.0, 1.0},
	{{-1.9, -2.0, 4.25}, 1.15, {0.5, 0.68, 0.49}, 0.989, 1.0, 1.0},
	{{2.1, 2.25, 6.25}, 1.35, {0.68, 0.09, 0.4}, 0.5, 1.0, 1.0},
	{{-3.15, 1.0, 3.25}, 1.25, {0.47, 0.78, 0.89}, 0.107, 16.0, 8.0},
	{{2.3, -1.5, 3.55}, 1.1, {0.43, 0.13, 0.13}, 1.7, 1.0, 1.0},
	{{0.1, -1.25, 2.75}, 0.275, {1.0, 1.0, 0.934}, 0.0, 1.0, 1.0}
};

const size_t PlanesCount = 6;
const Plane Planes[PlanesCount] =
{
	{{1.0, 0.0, 0.0}, 4.5, {0.12, 0.77, 0.12}, 0.25, 1.0, 0.65},
	{{-1.0, 0.0, 0.0}, 4.5, {0.77, 0.12, 0.12}, 0.25, 1.0, 0.65},
	{{0.0, 1.0, 0.0}, 4.0, {0.7, 0.65, 0.6}, 0.2, 1.0, 1.0},
	{{0.0, -1.0, 0.0}, 5.0, {0.7, 0.65, 0.6}, 0.2, 1.0, 1.0},
	{{0.0, 0.0, -1.0}, 8.0, {0.7, 0.65, 0.6}, 0.95, 1.0, 1.0},
	{{0.0, 0.0, 1.0}, 1.0, {0.0, 0.0, 0.0}, 0.0, 1.0, 0.0}
};

const size_t CubesCount = 1;
const Cube Cubes[CubesCount] =
{
	{{0.0, 0.0, 0.0}, 0.25, {1.0, 0.0, 1.0}, 0.0, 0.0, 0.0}
};

static GLuint InitRayTracer();
static void FillObjectsBuffer(GLuint * Buffer);
static bool FetchRenderingParameters(uint16_t & RenderWidth, uint16_t & RenderHeight, uint16_t & SamplesCount, bool & FullScreen);

bool HaveAnyIntersection(const Camera & CameraPosition, const Camera & CameraPreviousPosition);

int main(int argc, char * args[])
{
	SDL_Window * Window = nullptr;
	SDL_GLContext glContext;
	SDL_Event Event;
	GLuint RayTracerProgram;
	GLuint ObjectsBuffer;
	GLuint SSAAFBO, MainFBOTexture;
	GLuint TempFBO, TempFBOTexture;
	bool Processing = true;
	double Time = 0.0;
	float xMovement = 0.0, yMovement = 0.0, zMovement = 0.0;
	Camera CameraPosition = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	Camera TemporaryCameraPosition = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

	uint32_t Flags = SDL_WINDOW_OPENGL;

	uint16_t RenderWidth = 800;
	uint16_t RenderHeight = 600;
	uint16_t SamplesCount = 1;
	bool FullScreen = false;
	if(!FetchRenderingParameters(RenderWidth, RenderHeight, SamplesCount, FullScreen))
	{
		system("pause");
		return -1;
	}

	float AspectRatio = static_cast<float>(RenderWidth)/RenderHeight;
	uint16_t WindowWidth = RenderWidth;
	uint16_t WindowHeight = RenderHeight;
	if(FullScreen)
	{
		SDL_Rect DisplayRect;
		SDL_GetDisplayBounds(0, &DisplayRect);
		WindowWidth = DisplayRect.w;
		WindowHeight = DisplayRect.h;
		Flags |= SDL_WINDOW_FULLSCREEN;
	}

	Window = SDL_CreateWindow("RayCasting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, Flags);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	glContext = SDL_GL_CreateContext(Window);

	/*if(SDL_GL_SetSwapInterval(-1) == -1)
		SDL_GL_SetSwapInterval(1);*/

	RayTracerProgram = InitRayTracer();
	if(RayTracerProgram == 0)
	{
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(Window);
		SDL_Quit();
		system("pause");
		return -1;
	}
	glUseProgram(RayTracerProgram);
	FillObjectsBuffer(&ObjectsBuffer);

	glGenFramebuffers(1, &SSAAFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAAFBO);

	glBindTexture(GL_TEXTURE_2D, MainFBOTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, RenderWidth*SamplesCount, RenderHeight*SamplesCount);
	glTextureParameteri(MainFBOTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(MainFBOTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, MainFBOTexture, 0);
	glViewport(0, 0, RenderWidth*SamplesCount, RenderHeight*SamplesCount);

	glGenFramebuffers(1, &TempFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, TempFBO);

	glBindTexture(GL_TEXTURE_2D, TempFBOTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, RenderWidth, RenderHeight);
	glTextureParameteri(TempFBOTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(TempFBOTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TempFBOTexture, 0);

	while(Processing)
	{
		while(SDL_PollEvent(&Event))
			switch(Event.type)
			{
			case SDL_KEYUP:
				switch(Event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					Processing = false;
					break;

				case SDLK_PRINTSCREEN:
					{
						glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
						glReadBuffer(GL_FRONT);
						uint8_t * PixelData = new uint8_t[RenderWidth*RenderHeight*3];
						glReadPixels(0, 0, RenderWidth, RenderHeight, GL_BGR, GL_UNSIGNED_BYTE, PixelData);
						glReadBuffer(GL_BACK);
						glBindFramebuffer(GL_READ_FRAMEBUFFER, SSAAFBO);
						SaveImage("Image.bmp", RenderWidth, RenderHeight, PixelData);
					}
					break;

				case SDLK_d:
				case SDLK_a:
					xMovement = 0.0f;
					break;
				case SDLK_SPACE:
				case SDLK_c:
					yMovement = 0.0f;
					break;
				case SDLK_w:
				case SDLK_s:
					zMovement = 0.0f;
					break;
				}
				break;

			case SDL_KEYDOWN:
				switch(Event.key.keysym.sym)
				{
				case SDLK_d:
					xMovement = 1.0f;
					break;
				case SDLK_a:
					xMovement = -1.0f;
					break;
				case SDLK_SPACE:
					yMovement = 1.0f;
					break;
				case SDLK_c:
					yMovement = -1.0f;
					break;
				case SDLK_w:
					zMovement = 1.0f;
					break;
				case SDLK_s:
					zMovement = -1.0f;
					break;
				}
				break;

			case SDL_MOUSEMOTION:
				CameraPosition.yAngle -= Event.motion.xrel*rotationSpeed;
				if(std::abs(CameraPosition.xAngle - Event.motion.yrel*rotationSpeed) < 0.5*pi)
					CameraPosition.xAngle -= Event.motion.yrel*rotationSpeed;
				break;

			case SDL_QUIT:
				Processing = false;
				break;
			}
/*
		CameraPosition.x += speed*(xMovement*std::cos(CameraPosition.yAngle) + yMovement*std::sin(CameraPosition.xAngle)*std::sin(CameraPosition.yAngle) - zMovement*std::cos(CameraPosition.xAngle)*std::sin(CameraPosition.yAngle));
		CameraPosition.y += speed*(yMovement*std::cos(CameraPosition.xAngle) + zMovement*std::sin(CameraPosition.xAngle));
		CameraPosition.z += speed*(xMovement*std::sin(CameraPosition.yAngle) - yMovement*std::sin(CameraPosition.xAngle)*std::cos(CameraPosition.yAngle) + zMovement*std::cos(CameraPosition.xAngle)*std::cos(CameraPosition.yAngle));
*/
		TemporaryCameraPosition.x = CameraPosition.x + speed*(xMovement*std::cos(CameraPosition.yAngle) - zMovement*std::cos(CameraPosition.xAngle)*std::sin(CameraPosition.yAngle));
		TemporaryCameraPosition.y = CameraPosition.y + speed*(yMovement + zMovement*std::sin(CameraPosition.xAngle)*(1.0 - yMovement*yMovement));
		TemporaryCameraPosition.z = CameraPosition.z + speed*(xMovement*std::sin(CameraPosition.yAngle) + zMovement*std::cos(CameraPosition.xAngle)*std::cos(CameraPosition.yAngle));
		//if(!HaveAnyIntersection(TemporaryCameraPosition, CameraPosition))
		{
			CameraPosition.x = TemporaryCameraPosition.x;
			CameraPosition.y = TemporaryCameraPosition.y;
			CameraPosition.z = TemporaryCameraPosition.z;
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, SSAAFBO);

		glUseProgram(RayTracerProgram);
		glUniform1f(0, AspectRatio);
		glUniform1f(1, Time);
		glUniform3f(2, CameraPosition.x, CameraPosition.y, CameraPosition.z);
		glUniform2f(3, CameraPosition.xAngle, CameraPosition.yAngle);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, SSAAFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, TempFBO);
		glBlitFramebuffer(0, 0, RenderWidth*SamplesCount, RenderHeight*SamplesCount, 0, 0, RenderWidth, RenderHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, TempFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, RenderWidth, RenderHeight, 0, 0, WindowWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		SDL_GL_SwapWindow(Window);
		//SDL_Delay(10);
		Time += 0.001;
	}

	glDeleteFramebuffers(1, &TempFBO);
	glDeleteTextures(1, &TempFBOTexture);

	glDeleteFramebuffers(1, &SSAAFBO);
	glDeleteTextures(1, &MainFBOTexture);

	glDeleteProgram(RayTracerProgram);
	glDeleteBuffers(1, &ObjectsBuffer);

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(Window);
	SDL_Quit();

	return 0;
}

static GLuint InitRayTracer()
{
	GLuint VertexShader, FragmentShader, Program;

	VertexShader = LoadShader("RayTracer.vs", GL_VERTEX_SHADER);
	FragmentShader = LoadShader("RayTracer.fs", GL_FRAGMENT_SHADER);
	if(VertexShader == 0 || FragmentShader == 0)
		return 0;

	Program = glCreateProgram();
	glAttachShader(Program, VertexShader);
	glAttachShader(Program, FragmentShader);
	glLinkProgram(Program);

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	return Program;
}

static void FillObjectsBuffer(GLuint * Buffer)
{
	glGenBuffers(1, Buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, *Buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Sphere)*SpheresCount+sizeof(Plane)*PlanesCount+sizeof(Cube)*CubesCount, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Sphere)*SpheresCount, Spheres);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(Sphere)*SpheresCount, sizeof(Plane)*PlanesCount, Planes);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(Sphere)*SpheresCount+sizeof(Plane)*PlanesCount, sizeof(Cube)*CubesCount, Cubes);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, *Buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


const char * ConfigFileName = "Config.ini";
const size_t BufferSize = 64;
const size_t RenderParametersCount = 4;
const char * OptionsToRead[RenderParametersCount] = {
	"RenderWidth",
	"RenderHeight",
	"SamplesCount",
	"FullScreen"
};
static bool FetchRenderingParameters(uint16_t & RenderWidth, uint16_t & RenderHeight, uint16_t & SamplesCount, bool & FullScreen)
{
	std::ifstream ConfigFile(ConfigFileName, std::ios_base::in);
	if(!ConfigFile.is_open())
	{
		std::cout << "Can't find config file: " << ConfigFileName << std::endl;
		return false;
	}

	char BufferString[BufferSize];
	bool DataReaded[RenderParametersCount] = {};
	size_t PositionInBuffer = 0, OptionToProccess;
	while(!ConfigFile.eof() && iscntrl(ConfigFile.peek())) ConfigFile.get();
	while(!ConfigFile.eof())
	{
		if(!isalpha(ConfigFile.peek()))
		{
			std::cout << "Unexpected symbol: " << static_cast<char>(ConfigFile.peek()) << std::endl;
			ConfigFile.close();
			return false;
		}

		while(isalpha(ConfigFile.peek())) ConfigFile >> BufferString[PositionInBuffer++];
		BufferString[PositionInBuffer] = '\0';

		for(OptionToProccess = 0; OptionToProccess < RenderParametersCount; OptionToProccess++)
			if(!strcmp(BufferString, OptionsToRead[OptionToProccess]))
				break;

		if(OptionToProccess == RenderParametersCount)
		{
			std::cout << "Unknown option: " << BufferString << std::endl;
			ConfigFile.close();
			return false;
		}
		if(DataReaded[OptionToProccess])
		{
			std::cout << "Option \"" << BufferString << "\" not unique\n";
			ConfigFile.close();
			return false;
		}

		switch(OptionToProccess)
		{
		case 0:
			ConfigFile >> RenderWidth;
			break;
		case 1:
			ConfigFile >> RenderHeight;
			break;
		case 2:
			ConfigFile >> SamplesCount;
			break;
		case 3:
			ConfigFile >> FullScreen;
			break;
		}

		while(iscntrl(ConfigFile.peek())) ConfigFile.get();
		DataReaded[OptionToProccess] = true;
		PositionInBuffer = 0;
	}

	ConfigFile.close();

	return true;
}

bool SphereIntersection(const Sphere & SphereToCheck, const Camera & CameraPosition)
{
	float VectorCoordsX = CameraPosition.x - SphereToCheck.Coords[0];
	float VectorCoordsY = CameraPosition.y - SphereToCheck.Coords[1];
	float VectorCoordsZ = CameraPosition.z - SphereToCheck.Coords[2];
	return VectorCoordsX*VectorCoordsX + VectorCoordsY*VectorCoordsY + VectorCoordsZ*VectorCoordsZ < SphereToCheck.r*SphereToCheck.r;
}

bool PlaneIntersection(const Plane & PlaneToCheck, const Camera & CameraPosition, const Camera & CameraPreviousPosition)
{
	float FirstPostion = CameraPosition.x*PlaneToCheck.Normal[0] + CameraPosition.y*PlaneToCheck.Normal[1] + CameraPosition.z*PlaneToCheck.Normal[2] + PlaneToCheck.Distance;
	float SeconPosition = CameraPreviousPosition.x*PlaneToCheck.Normal[0] + CameraPreviousPosition.y*PlaneToCheck.Normal[1] + CameraPreviousPosition.z*PlaneToCheck.Normal[2] + PlaneToCheck.Distance;
	return FirstPostion*SeconPosition < 0.0f;
}

bool HaveAnyIntersection(const Camera & CameraPosition, const Camera & CameraPreviousPosition)
{
	for(const Sphere & CurrentSphere: Spheres)
		if(SphereIntersection(CurrentSphere, CameraPosition))
			return true;

	for(const Plane & CurrentPlane: Planes)
		if(PlaneIntersection(CurrentPlane, CameraPosition, CameraPreviousPosition))
			return true;

	return false;
}
