#version 330 core

out vec4 FragColor;
uniform vec3 col; // set in opengl code

void main()
{
    FragColor = vec4(col, 1.0f);
}