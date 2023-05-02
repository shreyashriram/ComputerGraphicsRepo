#version 330 core

in vec3 vert_Color;
out vec4 frag_Color;

void main() {
    frag_Color = vec4( vert_Color, 1.0f );
}
