#ifdef __arm__
#define NK_IMPLEMENTATION
#include "NK_GLES2.h"

float Font_Get_Text_Width(
nk_handle handle, float height, const char* text, int length) {
	(void)handle;
	(void)height;
	(void)text;
	(void)length;
	return 2000;
}

nk_context* nk_sdl_init(nk_sdl* sdl, SDL_Window *win) {
	sdl->win = win;
	nk_init_default(&sdl->ctx, 0);
	nk_sdl_device_create(sdl);
	return &sdl->ctx;
}

void nk_sdl_font_stash_begin(nk_sdl* sdl, nk_font_atlas **atlas) {
	nk_font_atlas_init_default(&sdl->atlas);
	nk_font_atlas_begin(&sdl->atlas);
	*atlas = &sdl->atlas;
}

void nk_sdl_font_stash_end(nk_sdl* sdl) {
    struct nk_image* image;
    int w, h;
    image = (struct nk_image*) nk_font_atlas_bake(
	&sdl->atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

    nk_sdl_device_upload_atlas(sdl, image, w, h);
    nk_font_atlas_end(
	&sdl->atlas, nk_handle_id((int)sdl->ogl.font_tex), &sdl->ogl.null);

    if (sdl->atlas.default_font) {
		nk_style_set_font(&sdl->ctx, &sdl->atlas.default_font->handle);
	}
}

void nk_sdl_render(
nk_sdl* sdl, nk_anti_aliasing AA, int max_vertex_buffer,
int max_element_buffer) {
    nk_sdl_device* dev = &sdl->ogl;
    int width, height;
    int display_width, display_height;
    struct nk_vec2 scale;
    GLfloat ortho[4][4] = {
        {2.0f, 0.0f, 0.0f, 0.0f},
        {0.0f,-2.0f, 0.0f, 0.0f},
        {0.0f, 0.0f,-1.0f, 0.0f},
        {-1.0f,1.0f, 0.0f, 1.0f},
    };
    SDL_GetWindowSize(sdl->win, &width, &height);
    SDL_GL_GetDrawableSize(sdl->win, &display_width, &display_height);
    ortho[0][0] /= (GLfloat)width;
    ortho[1][1] /= (GLfloat)height;

    scale.x = (float)display_width/(float)width;
    scale.y = (float)display_height/(float)height;

    /* setup global state */
    glViewport(0,0,display_width,display_height);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    /* setup program */
    glUseProgram(dev->prog);
    glUniform1i(dev->uniform_tex, 0);
    glUniformMatrix4fv(dev->uniform_proj, 1, GL_FALSE, &ortho[0][0]);
    {
        /* convert from command queue into draw list and draw to screen */
        const nk_draw_command *cmd;
        void *vertices, *elements;
        const nk_draw_index *offset = NULL;

        /* Bind buffers */
        glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

        {
            /* buffer setup */
            glEnableVertexAttribArray((GLuint)dev->attrib_pos);
            glEnableVertexAttribArray((GLuint)dev->attrib_uv);
            glEnableVertexAttribArray((GLuint)dev->attrib_col);

            glVertexAttribPointer(
			(GLuint)dev->attrib_pos, 2, GL_FLOAT, GL_FALSE, dev->vs,
			(void*)dev->vp);

            glVertexAttribPointer(
			(GLuint)dev->attrib_uv, 2, GL_FLOAT, GL_FALSE, dev->vs,
			(void*)dev->vt);

            glVertexAttribPointer(
			(GLuint)dev->attrib_col, 4, GL_UNSIGNED_BYTE, GL_TRUE, dev->vs,
			(void*)dev->vc);
        }

        glBufferData(GL_ARRAY_BUFFER, max_vertex_buffer, NULL, GL_STREAM_DRAW);
        glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, max_element_buffer, NULL, GL_STREAM_DRAW);

        /* load vertices/elements directly into vertex/element buffer */
        vertices = malloc((size_t)max_vertex_buffer);
        elements = malloc((size_t)max_element_buffer);
        {
            /* fill convert configuration */
            nk_convert_config config;
            static const nk_draw_vertex_layout_element vertex_layout[] = {
                {NK_VERTEX_POSITION, NK_FORMAT_FLOAT,
				NK_OFFSETOF(nk_sdl_vertex, position)},
                {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT,
				NK_OFFSETOF(nk_sdl_vertex, uv)},
                {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8,
				NK_OFFSETOF(nk_sdl_vertex, col)},
                {NK_VERTEX_LAYOUT_END}
            };
            memset(&config, 0, sizeof(config));
            config.vertex_layout = vertex_layout;
            config.vertex_size = sizeof(nk_sdl_vertex);
            config.vertex_alignment = NK_ALIGNOF(nk_sdl_vertex);
            config.null = dev->null;
            config.circle_segment_count = 22;
            config.curve_segment_count = 22;
            config.arc_segment_count = 22;
            config.global_alpha = 1.0f;
            config.shape_AA = AA;
            config.line_AA = AA;

            /* setup buffers to load vertices and elements */
            {
				nk_buffer vbuf, ebuf;
	            nk_buffer_init_fixed(
				&vbuf, vertices, (nk_size)max_vertex_buffer);

	            nk_buffer_init_fixed(
				&ebuf, elements, (nk_size)max_element_buffer);

	            nk_convert(&sdl->ctx, &dev->cmds, &vbuf, &ebuf, &config);
			}
        }
        glBufferSubData(
		GL_ARRAY_BUFFER, 0, (size_t)max_vertex_buffer, vertices);

        glBufferSubData(
		GL_ELEMENT_ARRAY_BUFFER, 0, (size_t)max_element_buffer, elements);

        free(vertices);
        free(elements);

        /* iterate over and execute each draw command */
        nk_draw_foreach(cmd, &sdl->ctx, &dev->cmds) {
            if (!cmd->elem_count) {
				continue;
			}
            glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
            glScissor(
			(GLint)(cmd->clip_rect.x * scale.x),
			(GLint)((height - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h)) *
			scale.y), (GLint)(cmd->clip_rect.w * scale.x),
			(GLint)(cmd->clip_rect.h * scale.y));

            glDrawElements(
			GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, offset);

            offset += cmd->elem_count;
        }
        nk_clear(&sdl->ctx);
    }

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

void nk_sdl_shutdown(nk_sdl* sdl) {
    nk_font_atlas_clear(&sdl->atlas);
    nk_free(&sdl->ctx);
    nk_sdl_device_destroy(sdl);
    memset(&sdl, 0, sizeof(sdl));
}

void nk_sdl_device_destroy(nk_sdl* sdl) {
    nk_sdl_device *dev = &sdl->ogl;
    glDetachShader(dev->prog, dev->vert_shdr);
    glDetachShader(dev->prog, dev->frag_shdr);
    glDeleteShader(dev->vert_shdr);
    glDeleteShader(dev->frag_shdr);
    glDeleteProgram(dev->prog);
    glDeleteTextures(1, &dev->font_tex);
    glDeleteBuffers(1, &dev->vbo);
    glDeleteBuffers(1, &dev->ebo);
    nk_buffer_free(&dev->cmds);
}

void nk_sdl_device_create(nk_sdl* sdl) {
    GLint status;
    static const GLchar *vertex_shader =
        NK_SHADER_VERSION
        "uniform mat4 ProjMtx;\n"
        "attribute vec2 Position;\n"
        "attribute vec2 TexCoord;\n"
        "attribute vec4 Color;\n"
        "varying vec2 Frag_UV;\n"
        "varying vec4 Frag_Color;\n"
        "void main() {\n"
        "   Frag_UV = TexCoord;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";
    static const GLchar *fragment_shader =
        NK_SHADER_VERSION
        "precision mediump float;\n"
        "uniform sampler2D Texture;\n"
        "varying vec2 Frag_UV;\n"
        "varying vec4 Frag_Color;\n"
        "void main(){\n"
        "   gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV);\n"
        "}\n";

    nk_sdl_device *dev = &sdl->ogl;

    nk_buffer_init_default(&dev->cmds);
    dev->prog = glCreateProgram();
    dev->vert_shdr = glCreateShader(GL_VERTEX_SHADER);
    dev->frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(dev->vert_shdr, 1, &vertex_shader, 0);
    glShaderSource(dev->frag_shdr, 1, &fragment_shader, 0);
    glCompileShader(dev->vert_shdr);
    glCompileShader(dev->frag_shdr);
    glGetShaderiv(dev->vert_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glGetShaderiv(dev->frag_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glAttachShader(dev->prog, dev->vert_shdr);
    glAttachShader(dev->prog, dev->frag_shdr);
    glLinkProgram(dev->prog);
    glGetProgramiv(dev->prog, GL_LINK_STATUS, &status);
    assert(status == GL_TRUE);

    dev->uniform_tex = glGetUniformLocation(dev->prog, "Texture");
    dev->uniform_proj = glGetUniformLocation(dev->prog, "ProjMtx");
    dev->attrib_pos = glGetAttribLocation(dev->prog, "Position");
    dev->attrib_uv = glGetAttribLocation(dev->prog, "TexCoord");
    dev->attrib_col = glGetAttribLocation(dev->prog, "Color");
    {
        dev->vs = sizeof(nk_sdl_vertex);
        dev->vp = offsetof(nk_sdl_vertex, position);
        dev->vt = offsetof(nk_sdl_vertex, uv);
        dev->vc = offsetof(nk_sdl_vertex, col);

        /* Allocate buffers */
        glGenBuffers(1, &dev->vbo);
        glGenBuffers(1, &dev->ebo);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void nk_sdl_device_upload_atlas(
nk_sdl* sdl, const void *image, int width, int height) {
    nk_sdl_device *dev = &sdl->ogl;
    glGenTextures(1, &dev->font_tex);
    glBindTexture(GL_TEXTURE_2D, dev->font_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
	GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA,
	GL_UNSIGNED_BYTE, image);
}

struct nk_image Load_Image_NK_GEN(
string filename, uint32_t width, uint32_t height) {
	int32_t w;
	int32_t h;
	int32_t n;
	GLuint tex;
	uint8_t* orig_buffer;
	uint8_t* trans_buffer;
	avir::CImageResizer<> air(8);

	orig_buffer = stbi_load(filename.c_str(), &w, &h, &n, 0);
	if (!orig_buffer || n != 4) {
		cout << "failed to load texture " << filename << endl;
		return nk_image_id((int) NULL);
	}
	trans_buffer = new uint8_t[width * height * 4];

	//air.resizeImage(orig_buffer, w, h, 0, trans_buffer, width, height, 4, 0);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(
	GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexParameteri(
	GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(
	GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei) w, (GLsizei) h, 0, GL_RGBA,
	GL_UNSIGNED_BYTE, orig_buffer);

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(orig_buffer);
	delete trans_buffer;
	return nk_image_id((int)tex);
}

void Free_Image_NK_GEN(struct nk_image* image) {
	(void)image;	// Bypass compiler warning
}

void Render_NK_GEN(NK_GEN* nk_gles) {
    float bg[4];
    int win_width, win_height;
    nk_color_fv(bg, nk_rgb(28,48,62));
    SDL_GetWindowSize(nk_gles->win, &win_width, &win_height);
    glViewport(0, 0, win_width, win_height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(bg[0], bg[1], bg[2], bg[3]);
    nk_sdl_render(
	nk_gles->sdl, NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

    SDL_GL_SwapWindow(nk_gles->win);
}

void HideCursor() {
	Uint8 l_data[1];
	Uint8 l_mask[1];
	l_data[0] = 0;
	l_mask[0] = 0;
	SDL_SetCursor(SDL_CreateCursor(l_data, l_mask, 1, 1, 0, 0));
}

void Initialize_NK_GEN(
NK_GEN* nk_gles) {
	nk_gles->sdl				= new nk_sdl;
	nk_gles->fonts				= NULL;
	nk_gles->number_of_fonts		= 0;

	/*
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);

	nk_gles->width	= DM.w;
	nk_gles->height	= DM.h;
	*/

	nk_gles->width  = 1920;
        nk_gles->height = 1080;

    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_GL_SetAttribute(
	SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    SDL_GL_SetAttribute(
	SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    nk_gles->win = SDL_CreateWindow(
	"Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, nk_gles->width, nk_gles->height,
	SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);

	HideCursor();

    nk_gles->glContext = SDL_GL_CreateContext(nk_gles->win);
    glViewport(0, 0, nk_gles->width, nk_gles->height);
    nk_gles->NK_Context = nk_sdl_init(nk_gles->sdl, nk_gles->win);
}

uint32_t Get_Width_NK_GEN(NK_GEN* nk_gles) {
	return nk_gles->width;
}

uint32_t Get_Height_NK_GEN(NK_GEN* nk_gles) {
	return nk_gles->height;
}

void Load_Fonts_NK_GEN(
NK_GEN* nk_gles, string font_path, uint32_t* font_heights,
uint32_t total_font_heights) {
	if (nk_gles->fonts != NULL) {
		delete nk_gles->fonts;
	}
	nk_gles->fonts				= new struct nk_font*[total_font_heights];
	nk_gles->number_of_fonts	= total_font_heights;

	nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(nk_gles->sdl, &atlas);

	for (uint32_t i = 0; i < total_font_heights; i++) {
		nk_gles->fonts[i] = nk_font_atlas_add_from_file(
		atlas, font_path.c_str(), font_heights[i], 0);
	}

    nk_sdl_font_stash_end(nk_gles->sdl);

	nk_style_set_font(nk_gles->NK_Context, &nk_gles->fonts[0]->handle);
}

void Set_Font_NK_GEN(NK_GEN* nk_gles, uint32_t font_index) {
	if (font_index < nk_gles->number_of_fonts) {
		nk_style_set_font(nk_gles->NK_Context, &nk_gles->fonts[font_index]->handle);
	}
}

void Delete_NK_GEN(NK_GEN* nk_gles) {
	nk_sdl_shutdown(nk_gles->sdl);
    SDL_GL_DeleteContext(nk_gles->glContext);
    SDL_DestroyWindow(nk_gles->win);
    SDL_Quit();

	delete nk_gles->sdl;

	if (nk_gles->fonts != NULL) {
		delete nk_gles->fonts;
	}
}

#endif
