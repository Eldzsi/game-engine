#version 330 core

out vec4 FragColor;

in vec2 texCoords;
in vec3 normal;
in vec3 fragPos;

uniform float u_time;
uniform vec2 u_uv_speed;
uniform sampler2D texture_diffuse;

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

uniform DirectionalLight directionalLight;

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS 20
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

#define MAX_SPOT_LIGHTS 20
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int spotLightCount;

uniform vec3 ambientColor;
uniform bool u_has_glow;
uniform vec3 u_glow_color;

uniform vec3 viewPos;
uniform vec3 specularColor;
uniform float shininess;

void main() {
    vec2 animatedTexCoords = texCoords + (u_uv_speed * u_time);

    vec4 texColor = texture(texture_diffuse, animatedTexCoords);

    if (texColor.a < 0.1) {
        discard;
    }

    if (u_has_glow) {
        FragColor = texColor * vec4(u_glow_color, 1.0);
    }
    else {
        vec3 norm = normalize(normal);

        vec3 viewDir = normalize(viewPos - fragPos);
        
        vec3 result = ambientColor * texColor.rgb;

        vec3 lightDir = normalize(-directionalLight.direction);
        float diff = max(dot(norm, lightDir), 0.0);

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = specularColor * spec * directionalLight.color;

        result += (directionalLight.color * diff * texColor.rgb) + specular;

        for (int i = 0; i < pointLightCount; i++) {
            vec3 pLightDir = normalize(pointLights[i].position - fragPos);
            float distance = length(pointLights[i].position - fragPos);
            float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));
            
            float pDiff = max(dot(norm, pLightDir), 0.0);

            vec3 pReflectDir = reflect(-pLightDir, norm);
            float pSpec = pow(max(dot(viewDir, pReflectDir), 0.0), shininess);
            vec3 pSpecular = specularColor * pSpec * pointLights[i].color;

            result += ((pointLights[i].color * pDiff * texColor.rgb) + pSpecular) * attenuation;
        }

        for (int i = 0; i < spotLightCount; i++) {
            vec3 sLightDir = normalize(spotLights[i].position - fragPos);
            
            float theta = dot(sLightDir, normalize(-spotLights[i].direction)); 
            float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
            float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
            
            if (intensity > 0.0) {
                float distance = length(spotLights[i].position - fragPos);
                float attenuation = 1.0 / (spotLights[i].constant + spotLights[i].linear * distance + spotLights[i].quadratic * (distance * distance));
                
                float sDiff = max(dot(norm, sLightDir), 0.0);
                vec3 sReflectDir = reflect(-sLightDir, norm);
                float sSpec = pow(max(dot(viewDir, sReflectDir), 0.0), shininess);
                vec3 sSpecular = specularColor * sSpec * spotLights[i].color;

                result += ((spotLights[i].color * sDiff * texColor.rgb) + sSpecular) * attenuation * intensity;
            }
        }

        FragColor = vec4(result, 1.0);
    }
}
