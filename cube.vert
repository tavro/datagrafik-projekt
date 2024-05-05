#version 150

in vec3 in_Position;
in vec3 in_Color;
in vec3 in_Normal; 

out vec3 pass_Color;
out vec3 normal;
out vec3 fragPos;

uniform mat4 mdlMatrix;
uniform mat4 projMatrix;
uniform mat4 camMatrix;

void main() {
    gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(in_Position, 1.0);
    fragPos = vec3(mdlMatrix * vec4(in_Position, 1.0));
    normal = normalize(mat3(mdlMatrix) * in_Normal);
    pass_Color = in_Color;
}
