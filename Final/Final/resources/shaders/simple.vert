#version 330 core

layout ( location = 0 ) in vec3 in_Position;
layout ( location = 1 ) in vec3 in_Color;

out vec3 vert_Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 objectColor;

void main()
{
    gl_Position = projection * view * model * vec4(in_Position, 1.0f);
    vert_Color = objectColor;
    
}
