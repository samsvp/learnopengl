#version 430 core

// The color of the line
uniform vec4 u_color;

out vec4 FragColor;
in vec2 mPos;

void main()
{
    FragColor = vec4(abs(mPos), (mPos.x + mPos.y) / 2.0, 1.0);
}