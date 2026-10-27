#pragma once

#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>

inline float smoothstep(float edge0, float edge1, float t) {
	// On contraint t entre 0 et 1
	t = std::clamp((t - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return t * t * (3.0f - 2.0f * t);
}

inline float easeOutSine(float t) {
	return std::sin((t * 3.1415926f) / 2.0f);
}

inline float easeInOutCirc(float t) {
	// On contraint t entre 0 et 1 pour éviter les erreurs de racine carrée
	t = glm::clamp(t, 0.0f, 1.0f);

	if (t < 0.5f) {
		// Formule pour la première moitié (accélération)
		return (1.0f - glm::sqrt(1.0f - glm::pow(2.0f * t, 2.0f))) / 2.0f;
	} else {
		// Formule pour la seconde moitié (décélération)
		return (glm::sqrt(1.0f - glm::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
	}
}
