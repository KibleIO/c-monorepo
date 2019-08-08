#ifdef __arm__

#ifndef NK_GLES_H_
#define NK_GLES_H_

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <cassert>
#include "stb_image.h"
#include "avir.h"
#include "NK_BASE.h"

#define NK_SHADER_VERSION "#version 100\n"

#define MAX_VERTEX_MEMORY 128 * 1024
#define MAX_ELEMENT_MEMORY 32 * 1024

//don't ask my why this is here
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

using namespace std;

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

nk_context*	nk_sdl_init(nk_sdl*, SDL_Window*);
void		nk_sdl_font_stash_begin(nk_sdl*, nk_font_atlas**);
void		nk_sdl_font_stash_end(nk_sdl*);
void 		nk_sdl_render(nk_sdl*, nk_anti_aliasing, int, int);
void		nk_sdl_shutdown(nk_sdl*);
void		nk_sdl_device_destroy(nk_sdl*);
void		nk_sdl_device_create(nk_sdl*);
void		nk_sdl_device_upload_atlas(nk_sdl*, const void*, int, int);

struct NK_GEN {
	SDL_Window*		win;
    SDL_GLContext	glContext;
	nk_sdl*			sdl;
	struct nk_font**	fonts;
	uint32_t		number_of_fonts;
	nk_context* 	NK_Context;
	void*			userdata;
};

float	Font_Get_Text_Width(nk_handle, float, const char*, int);

#endif

#endif
