#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_renderer.h"

// Decls
void render_to_target(SDL_Renderer* renderer, SDL_Texture* target, struct nk_context* ctx, void* data, SDL_Vertex* vertices, int* indices, SDL_Texture* texture);
void screenshot(SDL_Renderer* renderer, void* data);


// Globals
enum { EASY, HARD };
static int op = EASY;
static float tex_color = 0.0f;
static int button_pressed = 0;

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return -1;
	}
	SDL_Log("SDL is active!");

	SDL_Window* window = SDL_CreateWindow("SDL is active!", 100, 100, 512, 512, 0);
	if (!window)
	{
		SDL_Log("Unable to create window: %s", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		SDL_Log("Unable to create renderer: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	int width;
	int height;
	int channels;
	unsigned char* pixels = stbi_load("menacing.png", &width, &height, &channels, 4);
	if (!pixels)
	{
		SDL_Log("Unable to open image");
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	SDL_Log("Image width: %d height: %d channels: %d", width, height, channels);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
	if (!texture)
	{
		SDL_Log("Unable to create texture: %s", SDL_GetError());
		free(pixels);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	SDL_Texture* render_target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_TARGET, 1024, 1024);
	if (!render_target)
	{
		SDL_Log("Unable to create texture: %s", SDL_GetError());
		free(pixels);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	SDL_UpdateTexture(texture, NULL, pixels, width * 4);
	SDL_SetTextureAlphaMod(texture, 255);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	free(pixels);

	uint64_t frequency = SDL_GetPerformanceFrequency();
	uint64_t prev_time = 0;

	// Nuklear init
	struct nk_context* ctx = nk_sdl_init(window, renderer);
	struct nk_font_atlas* atlas;
	struct nk_font_config config = nk_font_config(0);
	struct nk_font* font;
	nk_sdl_font_stash_begin(&atlas);
	font = nk_font_atlas_add_default(atlas, 13, &config);
	nk_sdl_font_stash_end();
	nk_style_set_font(ctx, &font->handle);

	unsigned char* data = SDL_malloc(1024 * 1024 * 4);

	// game loop
	int running = 1;
	while (running)
	{
		uint64_t current_time = SDL_GetPerformanceCounter();
		double delta_time = (current_time - prev_time) / (double)frequency;
		printf("%2f\n", delta_time);

		prev_time = current_time;

		SDL_Vertex vertices[4];
		vertices[0].position.x = 50;
		vertices[0].position.y = 50;
		vertices[0].color.r = 255;
		vertices[0].color.g = 0;
		vertices[0].color.b = 0;
		vertices[0].color.a = 255;
		vertices[0].tex_coord.x = 0;
		vertices[0].tex_coord.y = 0;
		vertices[1].position.x = 200;
		vertices[1].position.y = 50;
		vertices[1].color.r = 0;
		vertices[1].color.g = 255;
		vertices[1].color.b = 0;
		vertices[1].color.a = 255;
		vertices[1].tex_coord.x = 1;
		vertices[1].tex_coord.y = 0;
		vertices[2].position.x = 50;
		vertices[2].position.y = 200;
		vertices[2].color.r = 0;
		vertices[2].color.g = 0;
		vertices[2].color.b = 255;
		vertices[2].color.a = 255;
		vertices[2].tex_coord.x = 0;
		vertices[2].tex_coord.y = 1;
		vertices[3].position.x = 200;
		vertices[3].position.y = 200;
		vertices[3].color.r = 0;
		vertices[3].color.g = 255;
		vertices[3].color.b = 255;
		vertices[3].color.a = 255;
		vertices[3].tex_coord.x = 1;
		vertices[3].tex_coord.y = 1;

		int indices[6] = { 0, 1, 2, 2, 3, 1 };

		SDL_Event event;
		nk_input_begin(ctx);
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = 0;
			}
			nk_sdl_handle_event(&event);
		}
		nk_input_end(ctx);

		SDL_PumpEvents();

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		render_to_target(renderer, NULL, ctx, data, vertices, indices, texture);
		// FIXME: second render target disabled
		//render_to_target(renderer, render_target, ctx, data, vertices, indices, texture);

		if (button_pressed)
		{
			screenshot(renderer, data);
		}

		SDL_RenderPresent(renderer);

	}

	nk_sdl_shutdown();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void render_to_target(SDL_Renderer* renderer, SDL_Texture* target, struct nk_context* ctx, void* data, SDL_Vertex* vertices, int* indices, SDL_Texture* texture)
{
	SDL_SetRenderTarget(renderer, target);

	for (int i = 0; i < 4; i++) {
		vertices[i].color.r = tex_color * 255;
		vertices[i].color.g = tex_color * 255;
		vertices[i].color.b = tex_color * 255;
		vertices[i].color.a = 255;
	}

	if (nk_begin(ctx, "Show", nk_rect(50, 50, 220, 220),
		NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE)) {
		/* fixed widget pixel width */
		nk_layout_row_static(ctx, 30, 80, 1);
		if (nk_button_label(ctx, "button")) {
			button_pressed = 1;
		}
		else
		{
			button_pressed = 0;
		}

		nk_layout_row_dynamic(ctx, 30, 2);
		if (nk_option_label(ctx, "easy", op == EASY))
		{
			op = EASY;
		}
		if (nk_option_label(ctx, "easy", op == HARD))
		{
			op = HARD;
		}

		nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
		{
			nk_layout_row_push(ctx, 50);
			nk_label(ctx, "Volume:", NK_TEXT_LEFT);
			nk_layout_row_push(ctx, 110);
			nk_slider_float(ctx, 0, &tex_color, 1.0f, 0.1f);
		}
		nk_layout_row_end(ctx);

	}
	nk_end(ctx);

	SDL_RenderGeometry(
		renderer, texture,
		vertices, 4,
		indices, 6
	);
	nk_sdl_render(NK_ANTI_ALIASING_ON);
}

void screenshot(SDL_Renderer* renderer, void* data)
{
	int ret = SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, data, 1024 * 4);
	stbi_write_png("screenshot.png", 1024, 1024, 4, data, 1024 * 4);
}