#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "geometry.h"

#define VECTOR_INIT_SIZE 3

typedef struct Vector {
    uint64_t size;
    uint64_t internal_size;
    Triangle* data;
} Vector;

Vector* initVector() {
    Vector* result = (Vector*) malloc(sizeof(Vector));
    if (result == NULL) return NULL;
    result->size = 0;
    result->internal_size = VECTOR_INIT_SIZE;
    result->data = (Triangle*) malloc(sizeof(Triangle) * VECTOR_INIT_SIZE);
    return result;
}

uint64_t add(Vector* vector, Triangle value) {
    if (vector->size >= vector->internal_size) {
        uint64_t newInternalSize = vector->internal_size + vector->internal_size / 2;

        Triangle* newData = (Triangle*) malloc(sizeof(Triangle) * newInternalSize);
        if (newData == NULL) {
            fprintf(stderr, "Unable to allocate new data in add().\n");
            exit(1);
        }

        if (!memcpy(newData, vector->data, sizeof(Triangle) * vector->size)) {
            fprintf(stderr, "Unable to allocate copy old data to new in add().\n");
            exit(1);
        }
        free(vector->data);
        vector->data = newData;

        vector->internal_size = newInternalSize;
    }
    vector->data[(vector->size)++] = value;
    return vector->size;
}

Triangle *at(Vector* vector, uint64_t index) {
    if (index < 0 || index >= vector->size) {
        fprintf(stderr, "Index out of range.\n");
        exit(1);
    }
    return &(vector->data[index]);
}

void freeVector(Vector* vector) {
    free(vector->data);
    free(vector);
}