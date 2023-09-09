#version 460

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 camPos;

layout(binding = 0) uniform Projection
{
    mat4 model;
    mat4 view;
    mat4 proj;

    vec3 camPos;
} proj_info;

void main()
{
    gl_Position = proj_info.proj * proj_info.view * proj_info.model * vec4(vPos, 1.0);

    fragPos = (proj_info.model * vec4(vPos, 1.0)).xyz;
    fragNormal = (proj_info.model * vec4(vNormal, 1.0)).xyz;
    camPos = proj_info.camPos;
}
