#version 460

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vColor;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragColor;
layout(location = 3) out vec3 camPos;

layout(set = 0, binding = 0) uniform ViewInfo
{
    mat4 view;
    mat4 projection;
    vec3 camPos;
} view_info;

layout(set = 1, binding = 0) uniform ModelInfo
{
    mat4 model;
} model_info;

void main()
{
    gl_Position = view_info.proj * view_info.view * model_info.model * vec4(vPos, 1.0);

    fragPos = (model_info.model * vec4(vPos, 1.0)).xyz;
    fragNormal = (model_info.model * vec4(vNormal, 1.0)).xyz;
    fragColor = vColor;
    camPos = view_info.camPos;
}
