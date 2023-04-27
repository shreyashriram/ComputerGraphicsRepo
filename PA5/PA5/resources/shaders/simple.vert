#version 330 core

layout ( location = 0 ) in vec3 in_Position;

uniform vec3 line_color;

out vec3 vert_Color;

void main()
{
    gl_Position = vec4(in_Position, 1.0f);
    vert_Color = line_color;
    
}
