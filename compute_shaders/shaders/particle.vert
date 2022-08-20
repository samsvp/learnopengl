#version 430 core
layout (location = 0) in vec2 aPos;

// TODO: Refactor into using a UBO
uniform mat4 model;
out vec2 mPos;

void main()
{
    mPos = aPos;
    gl_Position = model * vec4(aPos, 0.0, 1.0);
}