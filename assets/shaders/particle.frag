#version 330 core

in vec2 TexCoord;
in vec4 ParticleColor;
out vec4 FragColor;

uniform sampler2D sprite;

void main() {
    vec4 texColor = texture(sprite, TexCoord);
    
    if (texColor.a < 0.05 || (texColor.r < 0.1 && texColor.g < 0.1 && texColor.b < 0.1)) {
        discard;
    }
    
    FragColor = texColor * ParticleColor;
}
