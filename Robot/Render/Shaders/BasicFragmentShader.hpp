#pragma once

inline constexpr char BASIC_FRAGMENT_SHADER[] = R"glsl(
#version 460 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)glsl";
