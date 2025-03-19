# FluidX3D Vortex Generator

## Folders
- skybox: background images. you can put any image file you want
- export: path for exporting videos and configuration
- property: fluid property csv file
- src: source codes
- stl: 3d model of rotating fan
- bin: executable files. Update when simulation runs

## Source Code Editing Tips
1.  Open "src/defines.hpp"

2. Select  headless or GUI mode

    -  #define INTERACTIVE_GRAPHICS : GUI mode

    - #define GRAPHICS : Headless mode

3. Select stir mode or decay mode

    - #define STIR_MODE : stir forever
    - #define DECAY_MODE: stir until specified time (#define STOP_ROTOR)

4. Set simulation/export parameters
    
    - #define NUM_DATA : How many videos to export
    - #define RPM_RANGE : random range of rotor RPM
    - #define CONFIG_OPTION : config file export option. "json", "csv", "txt" 3 types available
    - #define START_CAPTURE : when to start image export
    - #define STOP_ROTOR : when to stop rotor rotation
    - #define STOP_CAPTURE : when to stop image export
    - #define FPS : simulation FPS
    - #define GRID : grid resolusion (one-side) 