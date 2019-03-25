#version 330 core

layout (location = 0) in vec3 inPos; // circle pos from opengl code

void main()
{
    gl_Position = vec4(inPos, 1.0);
}