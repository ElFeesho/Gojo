# Building Gojo
Gojo can be built for Mac OS X by using CMake and brew.

## Dependencies
Install all dependencies using:
```bash
brew install cmake sdl sdl_ttf sdl_gfx sdl_mixer sdl_image sdl_net homebrew/versions/lua53
```
Once the dependencies are installed, run cmake:
```bash
cmake .
```
Then invoke `make`.

This will build the `gojo` executable, which can be given Gojo script files (`*.gj` files.)
