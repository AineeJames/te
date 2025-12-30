#ifndef RENDERER_H_
#define RENDERER_H_

#include "engine.h"
#include <raylib.h>
#include <stddef.h>

typedef struct {
  Texture texture;
  size_t glyph_w;
  size_t glyph_h;
} GlyphAtlas;

typedef struct {
  Shader shader;
  int glyphAtlasTextureLoc;
  int gridTextureLoc;
  int cellSizeLoc;
  int gridSizeLoc;
} GridShader;

struct Renderer {
  GlyphAtlas atlas;
  GridShader grid_shader;
  Texture dummy;
  Texture grid_texture;
};

Renderer *renderer_init(Engine *engine);
void renderer_free(Renderer *renderer);

#endif
