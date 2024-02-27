#version 410

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

in vec3 FragmentNormal;
in vec3 ModelPosition;

uniform vec4 color;
uniform vec3 view_position;

void main(void)
{
	vec3 normal = normalize(FragmentNormal);
	float intensity = 1.0 - abs(dot(normalize(view_position - ModelPosition), normal));
    // write colour output without modification
    FragmentColour = vec4(color.xyz, pow(intensity, 1.5));
}
 