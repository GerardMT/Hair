# Hair
A simple CPU driven hair system with an OpenGL renderer. Used Qt 5 as the base framework.

The hair system is based on discrete particles simulation and virtual springs.

## Features
- Control the camera (`WASD` keys for movement, and `mouse left click` + `mouse movement` for orientation).
- Move the head/rope anchor points (`mouse right click` + `mouse movement`)
- Numerical solvers implemented:
	- Euler
	- Semi-implicit Euler
	- Verlet
- Collider types implemented:
	- Sphere
	- Plane
	- Triangle
- Force fields implemented:
	- Gravity
	- Drag
- Implemented spring force between particles.

## Known issues
- Some particles scape the box, or get into the sphere, on the edges/vertices. This is due to not checking for a collision again once the particle has been corrected. This can be easy fix at the cost of performance.

## Requirements
The software requires the following libraries to be installed:
- GLEW
- Eigen
- QT 5

## Build
Clone the repository:

	git clone https://github.com/GerardMT/Hair
	cd Hair

Compile the code:

    cd src
    qmake-qt5
    make

The builds can be found at `build/`.

## Run
Once build, run the project from the build directory:

	cd ../build/release/
	./hair

## Screenshots
<img src="docs/screenshots/rope.gif" alt="Rope" width="45%"> <img src="docs/screenshots/hair.gif" alt="Hair" width="45%">

## Other
Have a look at my other physics based projects:
- [Particles](https://github.com/GerardMT/Particles)
- [Cloth](https://github.com/GerardMT/Cloth)
- [Rigid-Bodies](https://github.com/GerardMT/Rigid-Bodies)

If you want to fork this project I recommend cloning [Cloth](https://github.com/GerardMT/Cloth), which includes all the code from [Particles](https://github.com/GerardMT/Particles) and [Hair](https://github.com/GerardMT/Hair), including some improvements.