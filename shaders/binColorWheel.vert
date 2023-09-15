#version 450
#extension GL_NV_viewport_array2 : enable
#extension GL_NV_stereo_view_rendering : enable

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in int VertexColorIndex;

uniform mat4 view_projection_matrix[2];
uniform mat4 model_matrix;
uniform vec3 center;

out vec4 FragmentColor;
out vec3 WorldPosition;
out vec3 WorldNormal;
out float IsHidden;
out float DistanceFromCenter;

//uniform vec3 colorA = vec3(1, 0, 0);
//uniform vec3 colorB = vec3(0, 0, 1);
uniform vec3 colors [MAX_COLOR_NUM];
uniform uint visibility [MAX_COLOR_NUM/32 + 1];
const vec3 otherColors[2] = vec3[2](vec3(1, 0, 0), vec3(0, 1, 0));

void main()
{
	DistanceFromCenter = length(VertexPosition - center);

	WorldNormal = (model_matrix*vec4(VertexNormal, 0.0)).xyz;
	WorldPosition = (model_matrix*vec4(VertexPosition, 1.0)).xyz;

	float alpha = 1.0;
	if((visibility[0] & (uint(1) << VertexColorIndex)) > 0)
		IsHidden = 1.f;
	else
		IsHidden = 0.f;

	FragmentColor = vec4(colors[VertexColorIndex], alpha);
    // assign vertex position without modification
    gl_Position = view_projection_matrix[0]*vec4(WorldPosition, 1.0);
    gl_SecondaryPositionNV = view_projection_matrix[1]*vec4(WorldPosition, 1.0);

    gl_ViewportMask[0] = 1;
    gl_SecondaryViewportMaskNV[0] = 2;
}
