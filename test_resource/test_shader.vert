#version 460 core
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 VertexUV;

layout(location = 0) out vec2 uv;

layout(binding = 0) uniform UniformBufferObject {
    vec2 position;
	vec4 color;
} ubo;

void main()
{
	vec2 VertexPos = VertexPosition.xy + ubo.position;
	// vertex position.
	gl_Position = vec4(vec3(VertexPos, VertexPosition.z), 1.0);

	uv = VertexUV;
}