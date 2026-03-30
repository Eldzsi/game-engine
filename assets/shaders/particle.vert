#version 330 core

layout (location = 0) in vec2 aQuadVert; 
layout (location = 1) in vec3 aPos;      
layout (location = 2) in vec4 aColor;    
layout (location = 3) in float aSize;    

out vec2 TexCoord;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

void main() {
    TexCoord = aQuadVert + vec2(0.5); 
    ParticleColor = aColor;

    vec3 vertexPosition_worldspace = aPos + cameraRight * aQuadVert.x * aSize + cameraUp * aQuadVert.y * aSize;

    gl_Position = projection * view * vec4(vertexPosition_worldspace, 1.0);
}
