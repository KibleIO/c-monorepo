#if defined(__linux__) && defined(__arm__)

#ifndef NK_GLES_H_
#define NK_GLES_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <cassert>
#include "stb_image.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_DEFAULT_FONT

#ifdef __arm__
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#endif

#include "nuklear.h"

#define NK_SHADER_VERSION "#version 100\n"

#define MAX_VERTEX_MEMORY 128 * 1024
#define MAX_ELEMENT_MEMORY 32 * 1024

//don't ask my why this is here
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

struct nk_sdl_device {
    nk_buffer cmds;
    nk_draw_null_texture null;
    GLuint vbo, ebo;
    GLuint prog;
    GLuint vert_shdr;
    GLuint frag_shdr;
    GLint attrib_pos;
    GLint attrib_uv;
    GLint attrib_col;
    GLint uniform_tex;
    GLint uniform_proj;
    GLuint font_tex;
    GLsizei vs;
    size_t vp, vt, vc;
};

struct nk_sdl_vertex {
    GLfloat position[2];
    GLfloat uv[2];
    nk_byte col[4];
};

struct nk_sdl {
    SDL_Window*		win;
    nk_sdl_device	ogl;
    nk_context		ctx;
    nk_font_atlas	atlas;
};

struct NK_GLES {
	SDL_Window*		win;
	nk_context*		ctx;
    SDL_GLContext	glContext;
	nk_sdl*			sdl;
};

nk_context*	nk_sdl_init(nk_sdl*, SDL_Window*);
void		nk_sdl_font_stash_begin(nk_sdl*, nk_font_atlas**);
void		nk_sdl_font_stash_end(nk_sdl*);
void 		nk_sdl_render(nk_sdl*, nk_anti_aliasing, int, int);
void		nk_sdl_shutdown(nk_sdl*);
void		nk_sdl_device_destroy(nk_sdl*);
void		nk_sdl_device_create(nk_sdl*);
struct nk_image	nk_sdl_load_image(const char*);
void		nk_sdl_device_upload_atlas(nk_sdl*, const void*, int, int);

void Initialize_NK_GLES(NK_GLES*);
void Render_NK_GLES(NK_GLES*);
void Delete_NK_GLES(NK_GLES*);

#endif

#endif
