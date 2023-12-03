#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include <SDL2/SDL.h>

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500
#define WINDOW_PIXEL_COUNT (WINDOW_WIDTH * WINDOW_HEIGHT)

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#pragma warning(push, 0)
#include "nuklear.h"
#include "nuklear_sdl_renderer.h"
#pragma warning(pop)

#include "custom_nk_sliderv.h"
#include "geometry.h"

#define SLIDER_WIDTH 50

#define YAW_SLIDER_Y_POSITION (93.0f / 100.0f * WINDOW_HEIGHT)
#define YAW_SLIDER_LENGTH (98.0f / 100.0f * WINDOW_WIDTH)

#define PITCH_SLIDER_X_POSITION (93.0f / 100.0f * WINDOW_WIDTH)
#define PITCH_SLIDER_LENGTH (93.0f / 100.0f * WINDOW_HEIGHT)