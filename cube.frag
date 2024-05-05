#version 150

in vec3 pass_Color;
in vec3 normal;
in vec3 fragPos;

out vec4 out_Color;

uniform bool useLighting;
uniform bool useStaticColor;
uniform vec3 staticColor;

uniform int lightingMode;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos; 
uniform float ambientStrength;
uniform float specularStrength; 
uniform int shininess;

void main() {
    if (useStaticColor) {
        out_Color = vec4(staticColor, 1.0);
    } else if (useLighting) {
        vec3 norm = normalize(normal);
        vec3 lightDir = normalize(lightPos - fragPos);
        vec3 viewDir = normalize(viewPos - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);

        float diff = max(dot(norm, lightDir), 0.0);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

        vec3 ambient = ambientStrength * lightColor;
        vec3 diffuse = diff * lightColor; 
        vec3 specular = specularStrength * spec * lightColor;

        vec3 result;
        switch (lightingMode) {
            case 0: // Ambient
                result = ambient;
                break;
            case 1: // Diffuse
                result = diffuse;
                break;
            case 2: // Specular
                result = specular;
                break;
            case 3: // Combined
                result = ambient + diffuse + specular;
                break;
        }

        out_Color = vec4(result * pass_Color, 1.0);
    } else {
        out_Color = vec4(pass_Color, 1.0);
    }
}
