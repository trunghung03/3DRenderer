#pragma once

NK_LIB void
nk_draw_sliderv(struct nk_command_buffer* out, nk_flags state,
    const struct nk_style_slider* style, const struct nk_rect* bounds,
    const struct nk_rect* visual_cursor, float min, float value, float max) {
    struct nk_rect fill;
    struct nk_rect bar;
    const struct nk_style_item* background;

    /* select correct slider images/colors */
    struct nk_color bar_color;
    const struct nk_style_item* cursor;

    NK_UNUSED(min);
    NK_UNUSED(max);
    NK_UNUSED(value);

    if (state & NK_WIDGET_STATE_ACTIVED) {
        background = &style->active;
        bar_color = style->bar_active;
        cursor = &style->cursor_active;
    }
    else if (state & NK_WIDGET_STATE_HOVER) {
        background = &style->hover;
        bar_color = style->bar_hover;
        cursor = &style->cursor_hover;
    }
    else {
        background = &style->normal;
        bar_color = style->bar_normal;
        cursor = &style->cursor_normal;
    }

    /* calculate slider background bar */
    bar.x = (visual_cursor->x + visual_cursor->w / 2) - bounds->w / 12;
    bar.y = bounds->y;
    bar.w = bounds->w / 6;
    bar.h = bounds->h;

    /* filled background bar style */
    fill.h = (visual_cursor->y + (visual_cursor->h / 2.0f)) - bar.y;
    fill.x = bar.x;
    fill.y = bar.y;
    fill.w = bar.w;

    /* draw background */
    switch (background->type) {
    case NK_STYLE_ITEM_IMAGE:
        nk_draw_image(out, *bounds, &background->data.image, nk_rgb_factor(nk_white, style->color_factor));
        break;
    case NK_STYLE_ITEM_NINE_SLICE:
        nk_draw_nine_slice(out, *bounds, &background->data.slice, nk_rgb_factor(nk_white, style->color_factor));
        break;
    case NK_STYLE_ITEM_COLOR:
        nk_fill_rect(out, *bounds, style->rounding, nk_rgb_factor(background->data.color, style->color_factor));
        nk_stroke_rect(out, *bounds, style->rounding, style->border, nk_rgb_factor(style->border_color, style->color_factor));
        break;
    }

    /* draw slider bar */
    nk_fill_rect(out, bar, style->rounding, nk_rgb_factor(bar_color, style->color_factor));
    nk_fill_rect(out, fill, style->rounding, nk_rgb_factor(style->bar_filled, style->color_factor));

    /* draw cursor */
    if (cursor->type == NK_STYLE_ITEM_IMAGE)
        nk_draw_image(out, *visual_cursor, &cursor->data.image, nk_rgb_factor(nk_white, style->color_factor));
    else
        nk_fill_circle(out, *visual_cursor, nk_rgb_factor(cursor->data.color, style->color_factor));
}

NK_LIB float
nk_sliderv_behavior(nk_flags* state, struct nk_rect* logical_cursor,
    struct nk_rect* visual_cursor, struct nk_input* in,
    struct nk_rect bounds, float slider_min, float slider_max, float slider_value,
    float slider_step, float slider_steps) {
    int left_mouse_down;
    int left_mouse_click_in_cursor;

    /* check if visual cursor is being dragged */
    nk_widget_state_reset(state);
    left_mouse_down = in && in->mouse.buttons[NK_BUTTON_LEFT].down;
    left_mouse_click_in_cursor = in && nk_input_has_mouse_click_down_in_rect(in,
        NK_BUTTON_LEFT, *visual_cursor, nk_true);

    if (left_mouse_down && left_mouse_click_in_cursor) {
        float ratio = 0;
        const float d = in->mouse.pos.y - (visual_cursor->y + visual_cursor->h * 0.5f);
        const float pxstep = bounds.h / slider_steps;

        /* only update value if the next slider step is reached */
        *state = NK_WIDGET_STATE_ACTIVE;
        if (NK_ABS(d) >= pxstep) {
            const float steps = (float)((int)(NK_ABS(d) / pxstep));
            slider_value += (d > 0) ? (slider_step * steps) : -(slider_step * steps);
            slider_value = NK_CLAMP(slider_min, slider_value, slider_max);
            ratio = (slider_value - slider_min) / slider_step;
            logical_cursor->y = bounds.y + (logical_cursor->h * ratio);
            in->mouse.buttons[NK_BUTTON_LEFT].clicked_pos.y = logical_cursor->y;
        }
    }

    /* slider widget state */
    if (nk_input_is_mouse_hovering_rect(in, bounds))
        *state = NK_WIDGET_STATE_HOVERED;
    if (*state & NK_WIDGET_STATE_HOVER &&
        !nk_input_is_mouse_prev_hovering_rect(in, bounds))
        *state |= NK_WIDGET_STATE_ENTERED;
    else if (nk_input_is_mouse_prev_hovering_rect(in, bounds))
        *state |= NK_WIDGET_STATE_LEFT;
    return slider_value;
}

NK_LIB float
nk_do_sliderv(nk_flags* state,
    struct nk_command_buffer* out, struct nk_rect bounds,
    float min, float val, float max, float step,
    const struct nk_style_slider* style, struct nk_input* in,
    const struct nk_user_font* font) {
    float slider_range;
    float slider_min;
    float slider_max;
    float slider_value;
    float slider_steps;
    float cursor_offset;

    struct nk_rect visual_cursor;
    struct nk_rect logical_cursor;

    NK_ASSERT(style);
    NK_ASSERT(out);
    if (!out || !style)
        return 0;

    /* remove padding from slider bounds */
    bounds.x = bounds.x + style->padding.x;
    bounds.y = bounds.y + style->padding.y;

    bounds.w = NK_MAX(bounds.w, 2 * style->padding.x);
    bounds.h = NK_MAX(bounds.h, 2 * style->padding.y + style->cursor_size.y);

    bounds.w -= 2 * style->padding.x;
    bounds.h -= 2 * style->padding.y;

    /* optional buttons */
    if (style->show_buttons) {
        nk_flags ws;
        struct nk_rect button;
        button.x = bounds.x;
        button.w = bounds.h;
        button.h = bounds.h;

        /* decrement button */
        button.y = bounds.y;
        if (nk_do_button_symbol(&ws, out, button, style->dec_symbol, NK_BUTTON_DEFAULT,
            &style->dec_button, in, font))
            val -= step;

        /* increment button */
        button.y = (bounds.y + bounds.h) - button.h;
        if (nk_do_button_symbol(&ws, out, button, style->inc_symbol, NK_BUTTON_DEFAULT,
            &style->inc_button, in, font))
            val += step;

        bounds.y = bounds.y + button.h + style->spacing.y;
        bounds.h = bounds.h - (2 * button.h + 2 * style->spacing.y);
    }

    /* remove one cursor size to support visual cursor */
    bounds.y += style->cursor_size.y * 0.5f;
    bounds.h -= style->cursor_size.y;

    /* make sure the provided values are correct */
    slider_max = NK_MAX(min, max);
    slider_min = NK_MIN(min, max);
    slider_value = NK_CLAMP(slider_min, val, slider_max);
    slider_range = slider_max - slider_min;
    slider_steps = slider_range / step;
    cursor_offset = (slider_value - slider_min) / step;

    /* calculate cursor
    Basically you have two cursors. One for visual representation and interaction
    and one for updating the actual cursor value. */
    logical_cursor.h = bounds.h / slider_steps;
    logical_cursor.w = bounds.w;
    logical_cursor.x = bounds.x;
    logical_cursor.y = bounds.y + (logical_cursor.h * cursor_offset);

    visual_cursor.h = style->cursor_size.y;
    visual_cursor.w = style->cursor_size.x;
    // For some reason, with vertical slider, 0.5f (from hor. slider) will make the cursor off center
    visual_cursor.x = (bounds.x + bounds.w * 0.7f) - visual_cursor.w * 0.7f;
    visual_cursor.y = logical_cursor.y - visual_cursor.h * 0.5f;

    slider_value = nk_sliderv_behavior(state, &logical_cursor, &visual_cursor,
        in, bounds, slider_min, slider_max, slider_value, step, slider_steps);
    visual_cursor.y = logical_cursor.y - visual_cursor.h * 0.5f;

    /* draw slider */
    if (style->draw_begin) style->draw_begin(out, style->userdata);
    nk_draw_sliderv(out, *state, style, &bounds, &visual_cursor, slider_min, slider_value, slider_max);
    if (style->draw_end) style->draw_end(out, style->userdata);
    return slider_value;
}

NK_API nk_bool
nk_slider_floatv(struct nk_context* ctx, float min_value, float* value, float max_value,
    float value_step) {
    struct nk_window* win;
    struct nk_panel* layout;
    struct nk_input* in;
    const struct nk_style* style;

    int ret = 0;
    float old_value;
    struct nk_rect bounds;
    enum nk_widget_layout_states state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    NK_ASSERT(value);
    if (!ctx || !ctx->current || !ctx->current->layout || !value)
        return ret;

    win = ctx->current;
    style = &ctx->style;
    layout = win->layout;

    state = nk_widget(&bounds, ctx);
    if (!state) return ret;
    in = (/*state == NK_WIDGET_ROM || */ state == NK_WIDGET_DISABLED || layout->flags & NK_WINDOW_ROM) ? 0 : &ctx->input;

    old_value = *value;
    *value = nk_do_sliderv(&ctx->last_widget_state, &win->buffer, bounds, min_value,
        old_value, max_value, value_step, &style->slider, in, style->font);
    return (old_value > *value || old_value < *value);
}

