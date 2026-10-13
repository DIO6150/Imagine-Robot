#pragma once

inline constexpr char BASIC_VERTEX_SHADER[] = R"glsl(
#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUV;

void main()
{
    gl_Position = vec4(aPosition, 1.0);
}
)glsl";
