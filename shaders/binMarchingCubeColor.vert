#version 450
#extension GL_NV_viewport_array2 : require
#extension GL_NV_stereo_view_rendering : require

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in int VertexColorIndex;

layout( secondary_view_offset=1 ) out highp int gl_Layer;

uniform mat4 view_projection_matrix[2];
uniform mat4 model_matrix;

out vec4 FragmentColor;
out vec3 WorldPosition;
out vec3 WorldNormal;

//uniform vec3 colorA = vec3(1, 0, 0);
//uniform vec3 colorB = vec3(0, 0, 1);
uniform vec3 colors [MAX_COLOR_NUM];
uniform uint visibility [MAX_COLOR_NUM/32 + 1];
const vec3 otherColors[2] = vec3[2](vec3(1, 0, 0), vec3(0, 1, 0));

uniform vec3 planeOrigin = vec3(0, 0, 0);
uniform vec3 planeNormal = vec3(0, 0, 0);

void main()
{
	WorldNormal = (model_matrix*vec4(VertexNormal, 0.0)).xyz;
	WorldPosition = (model_matrix*vec4(VertexPosition, 1.0)).xyz;

	float alpha;
	if((visibility[0] & (uint(1) << VertexColorIndex)) > 0 || 
		dot((VertexPosition - planeOrigin), planeNormal) > 0.0)
	{
		alpha = 0.0;
	}
	else
		alpha = 1.f;

	FragmentColor = vec4(colors[VertexColorIndex], alpha);
    // assign vertex position without modification
    gl_Position = view_projection_matrix[0]*vec4(WorldPosition, 1.0);
    gl_SecondaryPositionNV = view_projection_matrix[1]*vec4(WorldPosition, 1.0);

    gl_Layer=0;

    gl_ViewportMask[0] = 1;
    gl_SecondaryViewportMaskNV[0] = 2;
}
