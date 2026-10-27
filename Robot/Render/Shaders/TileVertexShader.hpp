
#pragma once

inline constexpr char TILE_VERTEX_SHADER[] = R"glsl(
#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUV;

uniform mat4 u_transform;

out vec2 vUV;

void main()
{
    gl_Position = u_transform * vec4(aPosition, 1.0);
    vUV = aUV;
}
)glsl";
