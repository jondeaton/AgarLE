R"for_c++_include(
#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 projection_transform;
uniform mat4 view_transform;
uniform mat4 model_transform;

void main() {
    gl_Position = projection_transform * view_transform * model_transform * vec4(position, 1.0f);
})for_c++_include"