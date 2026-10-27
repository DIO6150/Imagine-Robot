#pragma once

inline constexpr char TILE_FRAGMENT_SHADER[] = R"glsl(
#version 460 core

uniform vec3 u_color;

layout(binding = 0) uniform sampler2D sprite;

in vec2 vUV;
out vec4 FragColor;


void main()
{
    FragColor = texture(sprite, vUV) * vec4(u_color, 1.0);
}
)glsl";
