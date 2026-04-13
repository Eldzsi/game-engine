#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D image;
uniform vec4 color;
uniform bool useTexture;

void main() {
    if (useTexture) {
        FragColor = color * texture(image, TexCoords);
    } else {
        FragColor = color;
    }
}
