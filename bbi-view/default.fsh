#version 150
//
// Default fragment shader
//
out vec4 fColor;
in vec4  vColor;

void main (void)
{
    fColor = vColor;
}