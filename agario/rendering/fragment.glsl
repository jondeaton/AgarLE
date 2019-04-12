#version 330 core

in vec3 theColor;
out vec4 color;

void main() {
    color = vec4(theColor, 1.0f);
}