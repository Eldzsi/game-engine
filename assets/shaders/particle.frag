#version 330 core

in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 FragColor;

uniform sampler2D sprite;

void main() {
    vec4 texColor = texture(sprite, TexCoords);
    
    if (texColor.a < 0.05 || (texColor.r < 0.1 && texColor.g < 0.1 && texColor.b < 0.1)) {
        discard;
    }
    
    FragColor = texColor * ParticleColor;
}
