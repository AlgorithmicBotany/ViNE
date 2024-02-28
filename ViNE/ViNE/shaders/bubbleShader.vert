#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

uniform mat4 view_projection_matrix;
uniform mat4 model_matrix;

out vec3 FragmentNormal;
out vec3 ModelPosition;

void main()
{
	FragmentNormal = (model_matrix*vec4(VertexNormal, 0)).xyz;
	ModelPosition = (model_matrix*vec4(VertexPosition, 1.0)).xyz;
    // assign vertex position without modification
    gl_Position = view_projection_matrix*model_matrix*vec4(VertexPosition, 1.0);
}
