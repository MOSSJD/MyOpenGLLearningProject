#version 330 core
out vec4 fragColor;

in vec3 vertColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixRatio;

void main() {
	// fragColor = vec4(vertColor, 1);
	fragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), mixRatio);
}