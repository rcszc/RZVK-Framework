#version 460 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform UniformBufferObject {
    vec2 position;
	vec4 color;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

void main() {
	FragColor = texture(texSampler, uv) * 0.5; 
}