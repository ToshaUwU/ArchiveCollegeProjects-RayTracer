# Raytracer
Simple, suboptimal raytracer, made in two days for student course exam. Uploaded here for historical purposes.

## Requirments
Program depends on [SFML](https://github.com/SFML/SFML) library to create window and OpenGL context for rendering and handle input. Uses `window` and `system` modules.
- **CMake defines**:
 * `SFML_PATH` - path to SFML. Default value is `${THIRD_PARTY_DIRECTORY}/SFML/lib/cmake/SFML`.
 * `SFML_VERSION` - version of SFML. Default value is `2.5`, but may build with lower versions.

To run program GPU with support for OpenGL 4.5 is required. Any GPU that fulfills this requirments should be able to run project at least in 30FPS with SamplesCount set to 1 (about config settings see [Config](#Config)).

## Controls
- `Escape` - closes the program.
- `F12` - takes screenshot and saves it in root directiory in bmp format with name "screenshot" (overwriting old file).
- `Mouse` to look around.
- `WASD` to move forward, left, backward and right respective.
- `Space` to fly up and `C` to fly down.

All movement is made according to camera orientation.

## Config
Program expects to have `config.ini` file in exectuion directory with some minor settings.
- **Params description:**
 * `RenderWidth` - render framebuffer width. Default value is 1920.
 * `RenderHeight` - render framebuffer height. Default value is 1080.
 * `SamplesCount` - samples count to use for Antialiasing. Default value is 1.
   - **Warning:** algorithm used for antialiasing is SSAA, which leads to high impact on perfomance.
 * `FullScreen` - 1 to run in fullscreen mode, 0 to run in window. Default value is 1.
   - _Note:_ `RenderWidth` and `RenderHeight` parameters is used not just in windowed mode, but in fullscreen mode too.
   
Config file options should be kept like that, because parsing code is very meticulous to syntax and formatting.

## Screenshot from start position:
![screenshot](screenshot.bmp)

## Remastered version
TBD