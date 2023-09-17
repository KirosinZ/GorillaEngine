#version 460

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragColor;
layout(location = 3) in vec3 camPos;

layout(location = 0) out vec4 fragColor1;

void main()
{
    vec3 V = normalize(camPos - fragPos);
    vec3 N = normalize(fragNormal);
    float alignment = dot(V, N);

    fragColor1 = vec4(alignment * fragColor, 1.0);
}