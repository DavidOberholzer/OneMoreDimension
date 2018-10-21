# OneMoreDimension

A little project on figuring out a 3D software renderer :). Currently it just renders a rotating cube in the 3D space in 640x480 and the player can move the camera around in the 3D space. All rotations (including the camera rotations) are handled using Quarternions, avoiding gimbal lock. The fundementals of OpenGL were researched and the resulting engine was created using the ideologies found behind the OpenGL specification. 

## Running it!

Compile the C++ code with the make command. (NOTE: SDL2 library is required.)

`make compile`

Then run it!

`make run`

or to compile and run:

`make crun`

## Controls

W - Forwards\
S - Backwards\
A - Strafe Left\
D - Strafe Right\
G - Yaw camera left\
J - Yaw camera right\
Y - Pitch camera up\
H - Pitch camera down\
T - Roll camera left\
U - Roll camera right

## Object/Model File Structure

VERTEX
------
`vertex _x_ _y_ _z_`

**x, y, z:** An object vertex position in Model space.

TRIANGLE
--------
`triangle _vertices_`

**vertices:** A list of 3 vertex number references.

CUBE.TXT EXAMPLE
----------------

```
vertex 0.5 0.5 -0.5
vertex 0.5 -0.5 -0.5
vertex -0.5 -0.5 -0.5
vertex -0.5 0.5 -0.5
vertex 0.5 0.5 0.5
vertex 0.5 -0.5 0.5
vertex -0.5 -0.5 0.5
vertex -0.5 0.5 0.5

triangle 1 2 3
triangle 1 4 3
triangle 5 6 7
triangle 5 8 7

triangle 2 1 5
triangle 2 6 5
triangle 3 4 8
triangle 3 7 8

triangle 1 5 8
triangle 1 4 8
triangle 2 6 7
triangle 2 3 7
```

## TODOs

- Add Perspective Correct Texture Mapping to cube
- Mouse Support
- Some Lighting effects (Diffuse / Distance)

