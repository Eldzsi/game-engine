#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D image;
uniform vec4 spriteColor;
uniform bool useTexture;

void main() {
    if (useTexture) {
        FragColor = spriteColor * texture(image, TexCoords);
    } else {
        FragColor = spriteColor;
    }
}
