# FluidX3D Vortex Generator

## Turotrial (folders)
- skybox: background images. you can put any image file you want
- export: path for exporting videos and configuration
- property: fluid property csv file
- src: source codes
- stl: 3d model of rotating fan
- bin, temp: I don't know either

## Source Code Editing Tips
1.  GUI mode VS Headless mode 

Go to defines.hpp and comment / uncomment INTERACTIVE_GRAPHICS or GRAPHICS to select Headless mode or to use GUI. Currently set as headless mode.

2. Simulation Setup

Go to setup.cpp to edit geometries(fan.stl or fan2.stl), randomizing settings(rd), amount of data to export(num_data), 

3. Additional Source Codes

Please keep in mind there are 2 files (fluiddata.cpp, fluiddata.hpp) and many other variables added to the original code.
you should not use original make.sh code to execute this project.
