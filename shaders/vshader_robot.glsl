#version 330 

in  vec4 vPosition;
in  vec4 vColor;
out vec4 color;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 DrawColor;

void main() 
{
    //color = vColor;
    color = DrawColor;
    gl_Position = Projection*ModelView*vPosition;
} 
