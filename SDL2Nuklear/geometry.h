#pragma once

#include <math.h>

#define MATRIX_ELEM_COUNT 3 * 3
#define WRAP_OVER(X) ((X + ANIMATION_SPEED) > CIRCLE_DEGREE ? ANIMATION_SPEED : X + ANIMATION_SPEED)
#define WRAP_UNDER(X) ((X - ANIMATION_SPEED) < 0.0f ? (CIRCLE_DEGREE - ANIMATION_SPEED) : X - ANIMATION_SPEED)

// TODO: Convert these functions to use reference instead of copy

// Utilities
inline float to_rad(float degree) {
	return degree * ((float)M_PI / 180);
}

inline float absf(float value) {
	return (value < 0) ? (-value) : value;
}

// Geometry

typedef struct Vertex {
	float x;
	float y;
	float z;
} Vertex;

Vertex createVertex(float x, float y, float z) {
	return (Vertex) { x, y, z };
}

inline void convertOrigin(Vertex *v) {
	v->x += WINDOW_WIDTH / 2;
	v->y +=  WINDOW_HEIGHT / 2;
}

typedef struct Triangle {
	Vertex v1;
	Vertex v2;
	Vertex v3;

	struct nk_color color;
} Triangle;

Triangle createTriangle(Vertex v1, Vertex v2, Vertex v3, struct nk_color color) {
	return (Triangle) { v1, v2, v3, color };
}


typedef struct Matrix3 {
	float values[MATRIX_ELEM_COUNT];
} Matrix3;

Matrix3 createMatrix(float values[]) {
	Matrix3 result = { 0 };
	for (int i = 0; i < MATRIX_ELEM_COUNT; i++) {
		result.values[i] = values[i];
	}
	return result;
}

Matrix3 multiplyMatrix(Matrix3 *m1, Matrix3 *m2) {
	float resultValues[MATRIX_ELEM_COUNT] = { 0 };
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			for (int i = 0; i < 3; i++) {
				resultValues[row * 3 + col] += m1->values[row * 3 + i] * m2->values[i * 3 + col];
			}
		}
	}
	return createMatrix(resultValues);
}

Matrix3 yawTransformMatrix(float yawValue) {
	float yaw = to_rad(yawValue);
	float resultValues[] = {
		(float) cos(yaw),	0,	(float) -sin(yaw),
		0,					1,	0,
		(float) sin(yaw),	0,	(float) cos(yaw),
	};
	return createMatrix(resultValues);
}

Matrix3 pitchTransformMatrix(float pitchValue) {
	float pitch = to_rad(pitchValue);
	float resultValues[] = {
		1, 0, 0,
		0, (float) cos(pitch), (float) sin(pitch),
		0, (float) -sin(pitch), (float) cos(pitch)
	};
	return createMatrix(resultValues);
}

Vertex vertexTransform(Vertex *input, Matrix3 *transform) {
	return (Vertex) {
		input->x * transform->values[0] + input->y * transform->values[3] + input->z * transform->values[6],
		input->x * transform->values[1] + input->y * transform->values[4] + input->z * transform->values[7],
		input->x * transform->values[2] + input->y * transform->values[5] + input->z * transform->values[8]
	};
}