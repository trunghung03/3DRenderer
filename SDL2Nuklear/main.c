#include <float.h>
#include "header.h"

void setupSDL();
void setupNuklear();
int drawLines(Vertex *v1, Vertex *v2);
int drawPoints(int x, int y, struct nk_color *color);
struct nk_color getShade(struct nk_color* color, float shade);
struct nk_color getShadeSRGB(struct nk_color* color, float shade);
void cleanup();

// SDL
SDL_Window* win;
SDL_Renderer* renderer;
// Nuklear
struct nk_context* ctx;

void setupSDL() {
    /* SDL setup */
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_Init(SDL_INIT_EVERYTHING);

    win = SDL_CreateWindow("Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    if (win == NULL) {
        SDL_Log("Error SDL_CreateWindow %s", SDL_GetError());
        exit(-1);
    }

    int render_flags = 0;
    render_flags |= SDL_RENDERER_ACCELERATED;
    render_flags |= SDL_RENDERER_PRESENTVSYNC;

    renderer = SDL_CreateRenderer(win, -1, render_flags);

    if (renderer == NULL) {
        SDL_Log("Error SDL_CreateRenderer %s", SDL_GetError());
        exit(-1);
    }

    /* scale the renderer output for High-DPI displays */
    {
        int render_w, render_h;
        int window_w, window_h;
        float scale_x, scale_y;
        SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
        SDL_GetWindowSize(win, &window_w, &window_h);
        scale_x = (float)(render_w) / (float)(window_w);
        scale_y = (float)(render_h) / (float)(window_h);
        SDL_RenderSetScale(renderer, scale_x, scale_y);
    }
}

void setupNuklear() {
    /* GUI */
    ctx = nk_sdl_init(win, renderer);
    {
        struct nk_font_atlas* atlas;
        struct nk_font_config config = nk_font_config(0);
        struct nk_font* font;

        /* set up the font atlas and add desired font; note that font sizes are
         * multiplied by font_scale to produce better results at higher DPIs */
        nk_sdl_font_stash_begin(&atlas);
        font = nk_font_atlas_add_default(atlas, 13, &config);
        nk_sdl_font_stash_end();

        nk_style_set_font(ctx, &font->handle);
    }
}

int main(int argc, char *argv[]) {
    setupSDL();
    setupNuklear();

    int running = 1;
    /* GUI */
    struct nk_color bg = { 0, 0, 0, 0 };
    bg.r = 183, bg.g = 169, bg.b = 255, bg.a = 1;
    // Triangle stuff
    Triangle tris[4] = { 0 };

    // transparent bg
    
    ctx->style.window.fixed_background = nk_style_item_color(nk_rgba(0, 0, 0, 0));

    while (running)
    {
        /* Input */
        SDL_Event evt;
        nk_input_begin(ctx);
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) cleanup();
            nk_sdl_handle_event(&evt);
        }
        nk_input_end(ctx);

        // Sliders states
        static float slider_step = 1.0f;
        static float slider_max = 360.0f;
        static float yaw = 180.0f;
        static float pitch = 180.0f;

        /* GUI */

        if (nk_begin(ctx, "Pitch", nk_rect(PITCH_SLIDER_X_POSITION, 0, SLIDER_WIDTH, WINDOW_HEIGHT), 0)) {
            // Draw horizontal slider
            nk_layout_row_dynamic(ctx, PITCH_SLIDER_LENGTH, 1);

            nk_slider_floatv(ctx, 0, &pitch, slider_max, slider_step);
        }
        nk_end(ctx);

        if (nk_begin(ctx, "Yaw", nk_rect(0, YAW_SLIDER_Y_POSITION, YAW_SLIDER_LENGTH, SLIDER_WIDTH), 0)) {
            // Draw horizontal slider
            nk_layout_row_dynamic(ctx, SLIDER_WIDTH - 20, 1);

            nk_slider_float(ctx, 0, &yaw, slider_max, slider_step);
        }
        nk_end(ctx);

        /* End GUI */

        /* 3D */
        {
            // Draw triangle 1
            Triangle t1 = { 0, 0, 0, 0 };
            {
                Vertex v1 = createVertex(100, 100, 100);
                Vertex v2 = createVertex(-100, -100, 100);
                Vertex v3 = createVertex(-100, 100, -100);

                // white
                struct nk_color color = { 255, 255, 255, 255 };

                t1 = createTriangle(v1, v2, v3, color);
            }

            // Draw triangle 2
            Triangle t2 = { 0, 0, 0, 0 };
            {
                Vertex v1 = createVertex(100, 100, 100);
                Vertex v2 = createVertex(-100, -100, 100);
                Vertex v3 = createVertex(100, -100, -100);

                // red
                struct nk_color color = { 255, 0, 0, 255 };

                t2 = createTriangle(v1, v2, v3, color);
            }

            // Draw triangle 3
            Triangle t3 = { 0, 0, 0, 0 };
            {
                Vertex v1 = createVertex(-100, 100, -100);
                Vertex v2 = createVertex(100, -100, -100);
                Vertex v3 = createVertex(100, 100, 100);

                // green
                struct nk_color color = { 0, 255, 0, 255 };

                t3 = createTriangle(v1, v2, v3, color);
            }

            // Draw triangle 4
            Triangle t4 = { 0, 0, 0, 0 };
            {
                Vertex v1 = createVertex(-100, 100, -100);
                Vertex v2 = createVertex(100, -100, -100);
                Vertex v3 = createVertex(-100, -100, 100);

                // blue
                struct nk_color color = { 0, 0, 255, 255 };

                t4 = createTriangle(v1, v2, v3, color);
            }

            tris[0] = t1; tris[1] = t2; tris[2] = t3; tris[3] = t4;
        }

        /* End 3D */

        SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        // Draw triangles ortho
        {
            int i;
            Matrix3 yawTransform = yawTransformMatrix(yaw);
            Matrix3 pitchTransform = pitchTransformMatrix(pitch);
            Matrix3 transform = multiplyMatrix(&yawTransform, &pitchTransform);

            // init zBuffer with minimum z
            zBuffer = (float *) malloc(WINDOW_PIXEL_COUNT * sizeof(float));

            // Iterate through all triangles
            for (i = 0; i < 4; i++) {
                Vertex v1 = vertexTransform(&(tris[i].v1), &transform);
                Vertex v2 = vertexTransform(&(tris[i].v2), &transform);
                Vertex v3 = vertexTransform(&(tris[i].v3), &transform);

                // convert the origin to middle of the canvas
                convertOrigin(&v1);
                convertOrigin(&v2);
                convertOrigin(&v3);

                Vertex ab = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };
                Vertex ac = { v3.x - v1.x, v3.y - v1.y, v3.z - v1.z };
                Vertex norm = {
                    ab.y * ac.z - ab.z * ac.y,
                    ab.z * ac.x - ab.x * ac.z,
                    ab.x * ac.y - ab.y * ac.x
                };
                float normalLength = sqrtf(norm.x * norm.x + norm.y * norm.y + norm.z * norm.z);
                norm.x /= normalLength;
                norm.y /= normalLength;
                norm.z /= normalLength;

                float angleCos = absf(norm.z);

                // compute rectangular bounds for triangle
                int minX = (int)fmaxf(0, ceilf(fminf(v1.x, fminf(v2.x, v3.x))));
                int maxX = (int)fminf(WINDOW_WIDTH - 1, floorf(fmaxf(v1.x, fmaxf(v2.x, v3.x))));
                int minY = (int)fmaxf(0, ceilf(fminf(v1.y, fminf(v2.y, v3.y))));
                int maxY = (int)fminf(WINDOW_HEIGHT - 1, floorf(fmaxf(v1.y, fmaxf(v2.y, v3.y))));

                float triangleArea = (v1.y - v3.y) * (v2.x - v3.x) + (v2.y - v3.y) * (v3.x - v1.x);

                // Compute barycentric coordinate for each pixel 
                
                for (int y = minY; y <= maxY; y++) {
                    for (int x = minX; x <= maxX; x++) {
                        float b1 = ((y - v3.y) * (v2.x - v3.x) + (v2.y - v3.y) * (v3.x - x)) / triangleArea;
                        float b2 = ((y - v1.y) * (v3.x - v1.x) + (v3.y - v1.y) * (v1.x - x)) / triangleArea;
                        float b3 = ((y - v2.y) * (v1.x - v2.x) + (v1.y - v2.y) * (v2.x - x)) / triangleArea;
                        if (b1 >= 0 && b1 <= 1 && b2 >= 0 && b2 <= 1 && b3 >= 0 && b3 <= 1) {
                            
                            float depth = b1 * v1.z + b2 * v2.z + b3 * v3.z;
                            int zIndex = y * WINDOW_WIDTH + x;
                            if (zBuffer[zIndex] < depth) {
                                //getShade(&(tris[i].color), angleCos);
                                struct nk_color converted_color = getShadeSRGB(&(tris[i].color), angleCos);
                                //struct nk_color converted_color = getShade(&(tris[i].color), angleCos);
                                drawPoints(x, y, &converted_color);
                                zBuffer[zIndex] = depth;
                            }
                        }
                    }
                }
            }
        }

        nk_sdl_render(NK_ANTI_ALIASING_ON);

        SDL_RenderPresent(renderer);
    }

    return 1;
}

// Custom drawLine functions to translate points from top left origin to screen middle origin
int drawLines(Vertex *v1, Vertex *v2) {
    // Manually converting each argument is annoying, so pragma to suppress compiler
    // float to int conversion warning
#pragma warning(push)
#pragma warning(disable: 4244)
    return SDL_RenderDrawLine(renderer, v1->x, v1->y, v2->x, v2->y);
#pragma warning(pop)
}

int drawPoints(int x, int y, struct nk_color *color) {
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    return SDL_RenderDrawPoint(renderer, x, y);
}

inline struct nk_color getShade(struct nk_color* color, float shade) {
    struct nk_color result = {
        (nk_byte)(color->r * shade),
        (nk_byte)(color->g * shade),
        (nk_byte)(color->b * shade),
        255
    };
    return result;
}

inline struct nk_color getShadeSRGB(struct nk_color* color, float shade) {
    float rLinear = powf(color->r, 2.4f) * shade;
    float gLinear = powf(color->g, 2.4f) * shade;
    float bLinear = powf(color->b, 2.4f) * shade;

    struct nk_color result = {
        (nk_byte)powf(rLinear, 1 / 2.4f),
        (nk_byte)powf(gLinear, 1 / 2.4f),
        (nk_byte)powf(bLinear, 1 / 2.4f),
        255
    };
    return result;
}

void cleanup() {
    free(zBuffer);
    nk_sdl_shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    exit(0);
}

