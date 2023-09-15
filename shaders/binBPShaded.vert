#version 450

#extension GL_NV_viewport_array2 : enable
#extension GL_NV_stereo_view_rendering : enable

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

out vec3 ModelPosition;
out vec2 FragmentTexCoord;
out vec3 FragmentNormal;

uniform mat4 model_matrix;
uniform mat4 view_projection_matrix [2];

void main()
{
	FragmentTexCoord = VertexTexCoord;
	FragmentNormal = (model_matrix*vec4(VertexNormal, 0)).xyz;
	ModelPosition = (model_matrix*vec4(VertexPosition, 1)).xyz;

    // assign vertex position without modification
    gl_Position = view_projection_matrix[0]*vec4(ModelPosition, 1.0);
    gl_SecondaryPositionNV = view_projection_matrix[1]*vec4(ModelPosition, 1.0);

    gl_ViewportMask[0] = 1;
    gl_SecondaryViewportMaskNV[0] = 2;
}
