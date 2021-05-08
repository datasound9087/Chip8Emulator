#version 330 core

in vec2 tCoords;
out vec4 color;

uniform sampler2D tex;

void main()
{
    //color = vec4(1.0, 0.0, 1.0, 1.0);
    /*if(texture(tex, tCoords).r > 0)
    {
        color = vec4(1.0);
    }else
    {
        color = vec4(0.5);
    }*/
    color = vec4(texture(tex, tCoords).r * 255.0, 0.0, 0.0, 1.0);
    
}