#version 450

// first output is mapped to the framebuffer's colour index by default
out vec4 OutputColor;

in vec4 FragmentColor;
in vec3 WorldNormal;
in vec3 WorldPosition;
in float IsHidden;
in float DistanceFromCenter;

uniform vec3 viewPosition[2];

uniform vec3 lightPos = vec3(-1, 1, 1);

uniform float alpha = 20.0;
uniform float ks = 0.4;
uniform float kd = 0.4;
uniform float ka = 0.4;
uniform float bandwidth = 0.0008;

#define M_PI 3.1415926535897932384626433832795

float blinnPhongLighting(vec3 normal, vec3 position, vec3 viewPosition)
{
	vec3 viewer = normalize(viewPosition - position);
	vec3 light = normalize(lightPos - position);
	float attenuation = 1.f;	//320.f/(length(position - lightPos)*length(position-lightPos));

	vec3 h = normalize(viewer + light);
	//Formula found here: http://www.farbrausch.de/~fg/stuff/phong.pdf
	float normalizationFactor = (alpha+2)*(alpha+4)/(8*M_PI*(pow(sqrt(2), -alpha)+alpha));

	return max(dot(normal, light), 0)* (ks*normalizationFactor * pow(clamp(dot(normal, h), 0.0, 1.5), alpha)
			+ kd*clamp(dot(normal, light), 0.0, 1.0))*attenuation + ka;

}

vec3 getContrastingColorOfSimilarHue(vec3 color){
	float contrastLevel = 0.4;
	float maxComponent = max(color.r, max(color.g, color.b));
	if(maxComponent > 0.5)
		return (1-contrastLevel)*color;
	else if (maxComponent > 0.01)
		return color + contrastLevel/maxComponent*color;
	else
		return vec3(1)*contrastLevel;
}

vec3 getContrastingColorOfSimilarHue2(vec3 color){
	float contrastLevel = 0.4;
	float maxComponent = max(color.r, max(color.g, color.b));
	if(maxComponent > 0.5)
		return (1-contrastLevel)*color;
	else if (maxComponent > 0.01)
		return color + contrastLevel/maxComponent*color;
	else
		return vec3(1)*contrastLevel;
}

void main(void)
{

	vec3 color = FragmentColor.rgb;

	if(IsHidden > 0.1f){
		vec3 alternateColor = getContrastingColorOfSimilarHue(color);	//(length(color) > length(vec3(1,1, 1))*0.3) ? vec3(0) : vec3(1); 
		float blendFactor = pow(max(sin(DistanceFromCenter/bandwidth), 0), 2);
		color = (1-blendFactor)*color + blendFactor*alternateColor;		
	}

	color = color*blinnPhongLighting(normalize(WorldNormal), WorldPosition, viewPosition[gl_ViewportIndex]);


    // write colour output without modification
    OutputColor = vec4(color, 1.0);
}
