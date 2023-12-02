#include "header.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

void setupSDL();
void setupNuklear();
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

        static float value = 0.0f;
        static float horizontal = 0.0f;
        static float vertical = 0.0f;


        /* GUI */
        if (nk_begin(ctx, "Horizontal", nk_rect(0, 750, WINDOW_WIDTH, 50), 0)) {
            // Draw horizontal slider
            nk_layout_row_dynamic(ctx, 30, 2);

            nk_slider_float(ctx, 0, &horizontal, WINDOW_HEIGHT, 0.1f);
        }
        nk_end(ctx);

        if (nk_begin(ctx, "Vertical", nk_rect(0, 0, 50, WINDOW_HEIGHT), 0)) {
            // Draw horizontal slider
            nk_layout_row_dynamic(ctx, 30, 1);

            nk_slider_float(ctx, 0, &vertical, WINDOW_HEIGHT, 0.1f);
        }
        nk_end(ctx);

        if (nk_begin(ctx, "Test", nk_rect(0, WINDOW_HEIGHT/2, WINDOW_WIDTH, 50), 0)) {
            // Draw horizontal slider
            nk_layout_row_dynamic(ctx, 30, 1);

            nk_slider_float(ctx, 0, &value, WINDOW_HEIGHT, 0.1f);
        }
        nk_end(ctx);

        fprintf(stdout, "%f\n", value);

        SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderClear(renderer);

        nk_sdl_render(NK_ANTI_ALIASING_ON);

        SDL_RenderPresent(renderer);
    }
}

void cleanup() {
    nk_sdl_shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    exit(0);
}