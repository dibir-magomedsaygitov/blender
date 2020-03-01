/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright 2016, Blender Foundation.
 */

/** \file
 * \ingroup draw_engine
 */

#ifndef __WORKBENCH_PRIVATE_H__
#define __WORKBENCH_PRIVATE_H__

#include "BKE_studiolight.h"

#include "DNA_image_types.h"
#include "DNA_view3d_types.h"
#include "DNA_world_types.h"
#include "DNA_userdef_types.h"

#include "DRW_render.h"

#include "workbench_engine.h"

#define WORKBENCH_ENGINE "BLENDER_WORKBENCH"
/* TODO put them in workbench_shader.c */
#define MAX_COMPOSITE_SHADERS 3
#define MAX_PREPASS_SHADERS (1 << 8)
#define MAX_ACCUM_SHADERS (1 << 8)
#define MAX_CAVITY_SHADERS (1 << 3)
#define MAX_MATERIAL (1 << 12)

#define TEXTURE_DRAWING_ENABLED(wpd) (wpd->shading.color_type == V3D_SHADING_TEXTURE_COLOR)
#define VERTEX_COLORS_ENABLED(wpd) (wpd->shading.color_type == V3D_SHADING_VERTEX_COLOR)
#define MATERIAL_COLORS_ENABLED(wpd) (wpd->shading.color_type == V3D_SHADING_MATERIAL_COLOR)
#define FLAT_ENABLED(wpd) (wpd->shading.light == V3D_LIGHTING_FLAT)
#define STUDIOLIGHT_ENABLED(wpd) (wpd->shading.light == V3D_LIGHTING_STUDIO)
#define MATCAP_ENABLED(wpd) (wpd->shading.light == V3D_LIGHTING_MATCAP)
#define USE_WORLD_ORIENTATION(wpd) ((wpd->shading.flag & V3D_SHADING_WORLD_ORIENTATION) != 0)
#define STUDIOLIGHT_TYPE_WORLD_ENABLED(wpd) \
  (STUDIOLIGHT_ENABLED(wpd) && (wpd->studio_light->flag & STUDIOLIGHT_TYPE_WORLD))
#define STUDIOLIGHT_TYPE_STUDIO_ENABLED(wpd) \
  (STUDIOLIGHT_ENABLED(wpd) && (wpd->studio_light->flag & STUDIOLIGHT_TYPE_STUDIO))
#define STUDIOLIGHT_TYPE_MATCAP_ENABLED(wpd) \
  (MATCAP_ENABLED(wpd) && (wpd->studio_light->flag & STUDIOLIGHT_TYPE_MATCAP))
#define SSAO_ENABLED(wpd) \
  ((wpd->shading.flag & V3D_SHADING_CAVITY) && \
   ((wpd->shading.cavity_type == V3D_SHADING_CAVITY_SSAO) || \
    (wpd->shading.cavity_type == V3D_SHADING_CAVITY_BOTH)))
#define CURVATURE_ENABLED(wpd) \
  ((wpd->shading.flag & V3D_SHADING_CAVITY) && \
   ((wpd->shading.cavity_type == V3D_SHADING_CAVITY_CURVATURE) || \
    (wpd->shading.cavity_type == V3D_SHADING_CAVITY_BOTH)))
#define CAVITY_ENABLED(wpd) (CURVATURE_ENABLED(wpd) || SSAO_ENABLED(wpd))
#define SHADOW_ENABLED(wpd) (wpd->shading.flag & V3D_SHADING_SHADOW)
#define GHOST_ENABLED(psl) \
  (!DRW_pass_is_empty(psl->ghost_prepass_pass) || !DRW_pass_is_empty(psl->ghost_prepass_hair_pass))
#define CULL_BACKFACE_ENABLED(wpd) ((wpd->shading.flag & V3D_SHADING_BACKFACE_CULLING) != 0)
#define OIT_ENABLED(wpd) \
  (ELEM(wpd->shading.color_type, \
        V3D_SHADING_MATERIAL_COLOR, \
        V3D_SHADING_OBJECT_COLOR, \
        V3D_SHADING_TEXTURE_COLOR, \
        V3D_SHADING_VERTEX_COLOR))

#define IS_NAVIGATING(wpd) \
  ((DRW_context_state_get()->rv3d) && \
   (DRW_context_state_get()->rv3d->rflag & (RV3D_NAVIGATING | RV3D_PAINTING)))

#define OBJECT_OUTLINE_ENABLED(wpd) (wpd->shading.flag & V3D_SHADING_OBJECT_OUTLINE)
#define OBJECT_ID_PASS_ENABLED(wpd) (OBJECT_OUTLINE_ENABLED(wpd) || CURVATURE_ENABLED(wpd))
#define NORMAL_VIEWPORT_COMP_PASS_ENABLED(wpd) \
  (MATCAP_ENABLED(wpd) || STUDIOLIGHT_ENABLED(wpd) || SHADOW_ENABLED(wpd))
#define NORMAL_VIEWPORT_PASS_ENABLED(wpd) \
  (NORMAL_VIEWPORT_COMP_PASS_ENABLED(wpd) || SSAO_ENABLED(wpd) || CURVATURE_ENABLED(wpd))
#define NORMAL_ENCODING_ENABLED() (true)
/* TODO remove */
#define WORLD_CLIPPING_ENABLED(wpd) (false)

struct RenderEngine;
struct RenderLayer;
struct rcti;

typedef struct WORKBENCH_FramebufferList {
  /* Deferred render buffers */
  struct GPUFrameBuffer *opaque_fb;
  struct GPUFrameBuffer *opaque_infront_fb;
  struct GPUFrameBuffer *ghost_prepass_fb;
  struct GPUFrameBuffer *cavity_fb;
  struct GPUFrameBuffer *composite_fb;
  struct GPUFrameBuffer *id_clear_fb;

  struct GPUFrameBuffer *effect_fb;
  struct GPUFrameBuffer *effect_taa_fb;
  struct GPUFrameBuffer *depth_buffer_fb;
  struct GPUFrameBuffer *color_only_fb;

  struct GPUFrameBuffer *dof_downsample_fb;
  struct GPUFrameBuffer *dof_coc_tile_h_fb;
  struct GPUFrameBuffer *dof_coc_tile_v_fb;
  struct GPUFrameBuffer *dof_coc_dilate_fb;
  struct GPUFrameBuffer *dof_blur1_fb;
  struct GPUFrameBuffer *dof_blur2_fb;

  /* Forward render buffers */
  struct GPUFrameBuffer *object_outline_fb;
  struct GPUFrameBuffer *transparent_accum_fb;
  struct GPUFrameBuffer *transparent_revealage_fb;

  struct GPUFrameBuffer *transp_accum_fb;
  struct GPUFrameBuffer *transp_accum_infront_fb;
  struct GPUFrameBuffer *transp_reveal_fb;
} WORKBENCH_FramebufferList;

typedef struct WORKBENCH_TextureList {
  struct GPUTexture *dof_source_tx;
  struct GPUTexture *coc_halfres_tx;
  struct GPUTexture *history_buffer_tx;
  struct GPUTexture *depth_buffer_tx;
  struct GPUTexture *dummy_image_tx;
} WORKBENCH_TextureList;

typedef struct WORKBENCH_StorageList {
  struct WORKBENCH_PrivateData *g_data;
  struct WORKBENCH_PrivateData *wpd;
  struct WORKBENCH_EffectInfo *effects;
  float *dof_ubo_data;
} WORKBENCH_StorageList;

typedef struct WORKBENCH_PassList {
  /* deferred rendering */
  struct DRWPass *opaque_pass;
  struct DRWPass *opaque_infront_pass;
  struct DRWPass *merge_infront_pass;
  struct DRWPass *prepass_pass;
  struct DRWPass *prepass_hair_pass;
  struct DRWPass *ghost_prepass_pass;
  struct DRWPass *ghost_prepass_hair_pass;
  struct DRWPass *cavity_pass;
  struct DRWPass *shadow_depth_pass_pass;
  struct DRWPass *shadow_depth_pass_mani_pass;
  struct DRWPass *shadow_depth_fail_pass;
  struct DRWPass *shadow_depth_fail_mani_pass;
  struct DRWPass *shadow_depth_fail_caps_pass;
  struct DRWPass *shadow_depth_fail_caps_mani_pass;
  struct DRWPass *composite_pass;
  struct DRWPass *composite_shadow_pass;
  struct DRWPass *oit_composite_pass;
  struct DRWPass *background_pass;
  struct DRWPass *background_pass_clip;
  struct DRWPass *ghost_resolve_pass;
  struct DRWPass *effect_aa_pass;
  struct DRWPass *dof_down_ps;
  struct DRWPass *dof_down2_ps;
  struct DRWPass *dof_flatten_v_ps;
  struct DRWPass *dof_flatten_h_ps;
  struct DRWPass *dof_dilate_h_ps;
  struct DRWPass *dof_dilate_v_ps;
  struct DRWPass *dof_blur1_ps;
  struct DRWPass *dof_blur2_ps;
  struct DRWPass *dof_resolve_ps;
  struct DRWPass *volume_pass;

  /* forward rendering */
  struct DRWPass *transp_resolve_pass;
  struct DRWPass *transp_accum_pass;
  struct DRWPass *transp_accum_infront_pass;
  struct DRWPass *transparent_accum_pass;
  struct DRWPass *object_outline_pass;
  struct DRWPass *depth_pass;
} WORKBENCH_PassList;

typedef struct WORKBENCH_Data {
  void *engine_type;
  WORKBENCH_FramebufferList *fbl;
  WORKBENCH_TextureList *txl;
  WORKBENCH_PassList *psl;
  WORKBENCH_StorageList *stl;
} WORKBENCH_Data;

typedef struct WORKBENCH_UBO_Light {
  float light_direction[4];
  float specular_color[3], pad;
  float diffuse_color[3], wrapped;
} WORKBENCH_UBO_Light;

typedef struct WORKBENCH_UBO_Material {
  float base_color[3];
  /* Packed data into a int. Decoded in the shader. */
  uint32_t packed_data;
} WORKBENCH_UBO_Material;

typedef struct WORKBENCH_UBO_World {
  float viewvecs[3][4];
  float viewport_size[2], viewport_size_inv[2];
  float object_outline_color[4];
  float shadow_direction_vs[4];
  WORKBENCH_UBO_Light lights[4];
  float ambient_color[4];
  int matcap_orientation;
  float curvature_ridge;
  float curvature_valley;
  int _pad0;
} WORKBENCH_UBO_World;

BLI_STATIC_ASSERT_ALIGN(WORKBENCH_UBO_World, 16)
BLI_STATIC_ASSERT_ALIGN(WORKBENCH_UBO_Light, 16)
BLI_STATIC_ASSERT_ALIGN(WORKBENCH_UBO_Material, 16)

typedef struct WORKBENCH_PrivateData {
  struct GHash *material_transp_hash;
  struct GPUShader *prepass_sh;
  struct GPUShader *prepass_hair_sh;
  struct GPUShader *prepass_uniform_sh;
  struct GPUShader *prepass_uniform_hair_sh;
  struct GPUShader *prepass_textured_sh;
  struct GPUShader *prepass_textured_array_sh;
  struct GPUShader *prepass_vertex_sh;
  struct GPUShader *composite_sh;
  struct GPUShader *background_sh;
  struct GPUShader *transparent_accum_sh;
  struct GPUShader *transparent_accum_hair_sh;
  struct GPUShader *transparent_accum_uniform_sh;
  struct GPUShader *transparent_accum_uniform_hair_sh;
  struct GPUShader *transparent_accum_textured_sh;
  struct GPUShader *transparent_accum_textured_array_sh;
  struct GPUShader *transparent_accum_vertex_sh;
  View3DShading shading;
  eContextObjectMode ctx_mode;
  StudioLight *studio_light;
  const UserDef *preferences;
  /* Does this instance owns the `world_ubo` field.
   * Normally the field is borrowed from `WORKBENCH_WorldData`. In case that
   * there is no World attached to the scene the UBO cannot be cached and should
   * be freed after using. */
  bool is_world_ubo_owner;
  struct GPUUniformBuffer *world_ubo;
  struct DRWShadingGroup *shadow_shgrp;
  struct DRWShadingGroup *depth_shgrp;
  WORKBENCH_UBO_World world_data;
  float shadow_multiplier;
  float shadow_shift;
  float shadow_focus;
  float cached_shadow_direction[3];
  float shadow_mat[4][4];
  float shadow_inv[4][4];
  /* Far plane of the view frustum. */
  float shadow_far_plane[4];
  /* Near plane corners in shadow space. */
  float shadow_near_corners[4][3];
  /* min and max of shadow_near_corners. allow fast test */
  float shadow_near_min[3];
  float shadow_near_max[3];
  /* This is a parallelogram, so only 2 normal and distance to the edges. */
  float shadow_near_sides[2][4];
  bool shadow_changed;
  bool is_playback;

  /* Opaque pipeline */
  struct GPUTexture *object_id_tx;
  struct GPUTexture *material_buffer_tx;
  struct GPUTexture *composite_buffer_tx;
  struct GPUTexture *normal_buffer_tx;

  /* Transparent pipeline */
  struct GPUTexture *accum_buffer_tx;
  struct GPUTexture *reveal_buffer_tx;

  struct {
    struct DRWShadingGroup *common_shgrp;
    struct DRWShadingGroup *vcol_shgrp;
    struct DRWShadingGroup *image_shgrp;
    struct DRWShadingGroup *image_tiled_shgrp;
  } prepass[2][2];

  /* Materials */
  struct GHash *material_hash;
  struct GHash *texture_hash;
  struct BLI_memblock *material_ubo;
  struct BLI_memblock *material_ubo_data;
  WORKBENCH_UBO_Material *material_ubo_data_curr;
  struct GPUUniformBuffer *material_ubo_curr;
  struct GPUTexture *dummy_image_tx;
  int material_chunk_count;
  int material_chunk_curr;
  int material_index;

  /* Volumes */
  bool volumes_do;
  ListBase smoke_domains;

  /* Ssao */
  float winmat[4][4];
  float viewvecs[3][4];
  float ssao_params[4];
  float ssao_settings[4];

  /* Dof */
  struct GPUTexture *dof_blur_tx;
  struct GPUTexture *coc_temp_tx;
  struct GPUTexture *coc_tiles_tx[2];
  struct GPUUniformBuffer *dof_ubo;
  float dof_aperturesize;
  float dof_distance;
  float dof_invsensorsize;
  float dof_near_far[2];
  float dof_blades;
  float dof_rotation;
  float dof_ratio;
  bool dof_enabled;

  eGPUShaderConfig sh_cfg;
} WORKBENCH_PrivateData; /* Transient data */

typedef struct WORKBENCH_EffectInfo {
  /** View */
  struct DRWView *view;
  /** Last projection matrix to see if view is still valid. */
  float last_mat[4][4];
  int jitter_index;
  float taa_mix_factor;
  bool view_updated;
} WORKBENCH_EffectInfo;

typedef struct WORKBENCH_MaterialData {
  float base_color[3], metallic;
  float roughness, alpha;
  int color_type;
  int interp;
  Image *ima;
  ImageUser *iuser;

  /* Linked shgroup for drawing */
  DRWShadingGroup *shgrp;
  /* forward rendering */
  DRWShadingGroup *shgrp_object_outline;
} WORKBENCH_MaterialData;

typedef struct WORKBENCH_ObjectData {
  DrawData dd;

  /* Shadow direction in local object space. */
  float shadow_dir[3], shadow_depth;
  /* Min, max in shadow space */
  float shadow_min[3], shadow_max[3];
  BoundBox shadow_bbox;
  bool shadow_bbox_dirty;
} WORKBENCH_ObjectData;

typedef struct WORKBENCH_ViewLayerData {
  struct GPUUniformBuffer *world_ubo;
  struct BLI_memblock *material_ubo;
  struct BLI_memblock *material_ubo_data;
} WORKBENCH_ViewLayerData;

/* Enumeration containing override options for base color rendering.
 * This is used to during painting to force the base color to show what you are
 * painting using the selected lighting model. */
typedef enum WORKBENCH_ColorOverride {
  WORKBENCH_COLOR_OVERRIDE_OFF = 0,
  WORKBENCH_COLOR_OVERRIDE_TEXTURE = CTX_MODE_PAINT_TEXTURE,
  WORKBENCH_COLOR_OVERRIDE_VERTEX = CTX_MODE_PAINT_VERTEX,
} WORKBENCH_ColorOverride;

/* inline helper functions */
BLI_INLINE bool workbench_is_specular_highlight_enabled(WORKBENCH_PrivateData *wpd)
{
  if ((wpd->shading.flag & V3D_SHADING_SPECULAR_HIGHLIGHT)) {
    if (STUDIOLIGHT_ENABLED(wpd) || MATCAP_ENABLED(wpd)) {
      return (wpd->studio_light->flag & STUDIOLIGHT_SPECULAR_HIGHLIGHT_PASS) != 0;
    }
  }
  return false;
}

BLI_INLINE bool workbench_is_taa_enabled(WORKBENCH_PrivateData *wpd)
{
  if (DRW_state_is_image_render()) {
    const DRWContextState *draw_ctx = DRW_context_state_get();
    if (draw_ctx->v3d) {
      return draw_ctx->scene->display.viewport_aa > SCE_DISPLAY_AA_FXAA;
    }
    else {
      return draw_ctx->scene->display.render_aa > SCE_DISPLAY_AA_FXAA;
    }
  }
  else {
    return !(IS_NAVIGATING(wpd) || wpd->is_playback) &&
           wpd->preferences->viewport_aa > SCE_DISPLAY_AA_FXAA;
  }
}

BLI_INLINE bool workbench_is_fxaa_enabled(WORKBENCH_PrivateData *wpd)
{
  if (DRW_state_is_image_render()) {
    const DRWContextState *draw_ctx = DRW_context_state_get();
    if (draw_ctx->v3d) {
      return draw_ctx->scene->display.viewport_aa == SCE_DISPLAY_AA_FXAA;
    }
    else {
      return draw_ctx->scene->display.render_aa == SCE_DISPLAY_AA_FXAA;
    }
  }
  else {
    if (wpd->preferences->viewport_aa == SCE_DISPLAY_AA_FXAA) {
      return true;
    }

    /* when navigating or animation playback use FXAA if scene uses TAA. */
    return (IS_NAVIGATING(wpd) || wpd->is_playback) &&
           wpd->preferences->viewport_aa > SCE_DISPLAY_AA_FXAA;
  }
}

/** Is texture paint mode enabled (globally) */
BLI_INLINE bool workbench_is_in_texture_paint_mode(void)
{
  const DRWContextState *draw_ctx = DRW_context_state_get();
  return draw_ctx->object_mode == OB_MODE_TEXTURE_PAINT;
}

/** Is vertex paint mode enabled (globally) */
BLI_INLINE bool workbench_is_in_vertex_paint_mode(void)
{
  const DRWContextState *draw_ctx = DRW_context_state_get();
  return draw_ctx->object_mode == OB_MODE_VERTEX_PAINT;
}

/* Must the `View3DShading.color_type` be overriden for the given object. */
BLI_INLINE WORKBENCH_ColorOverride workbench_object_color_override_get(Object *ob)
{
  const DRWContextState *draw_ctx = DRW_context_state_get();
  if (ob->type == OB_MESH && (draw_ctx->obact == ob)) {
    const enum eContextObjectMode mode = CTX_data_mode_enum_ex(
        draw_ctx->object_edit, draw_ctx->obact, draw_ctx->object_mode);
    if (mode == CTX_MODE_PAINT_TEXTURE) {
      return WORKBENCH_COLOR_OVERRIDE_TEXTURE;
    }
    else if (mode == CTX_MODE_PAINT_VERTEX) {
      return WORKBENCH_COLOR_OVERRIDE_VERTEX;
    }
  }

  return WORKBENCH_COLOR_OVERRIDE_OFF;
}

BLI_INLINE bool workbench_is_matdata_pass_enabled(WORKBENCH_PrivateData *wpd)
{
  return (wpd->shading.color_type != V3D_SHADING_SINGLE_COLOR || MATCAP_ENABLED(wpd)) ||
         workbench_is_in_texture_paint_mode() || workbench_is_in_vertex_paint_mode();
}

/**
 * Get the default texture format to be used by the color and history buffers.
 *
 * Use GPU_RGBA16F for final renderings and for drawing textures. This
 * allows displaying HDRI textures. Vertex Colors uses GPU_RGBA16 to resolve
 * color banding issues (T66100). All other modes use GPU_RGBA8 to reduce
 * bandwidth and gpu memory.
 */
BLI_INLINE eGPUTextureFormat workbench_color_texture_format(const WORKBENCH_PrivateData *wpd)
{
  eGPUTextureFormat result;
  if (DRW_state_is_image_render() || workbench_is_in_texture_paint_mode() ||
      TEXTURE_DRAWING_ENABLED(wpd)) {
    result = GPU_RGBA16F;
  }
  else {
    result = GPU_RGBA16;
  }
  return result;
}

/* workbench_opaque.c */
void workbench_opaque_engine_init(WORKBENCH_Data *data);
void workbench_opaque_cache_init(WORKBENCH_Data *data);

/* workbench_transparent.c */
void workbench_transparent_engine_init(WORKBENCH_Data *data);
void workbench_transparent_cache_init(WORKBENCH_Data *data);

/* workbench_shader.c */
GPUShader *workbench_shader_opaque_get(WORKBENCH_PrivateData *wpd);
GPUShader *workbench_shader_opaque_image_get(WORKBENCH_PrivateData *wpd, bool tiled);
GPUShader *workbench_shader_composite_get(WORKBENCH_PrivateData *wpd);
GPUShader *workbench_shader_merge_infront_get(WORKBENCH_PrivateData *wpd);

GPUShader *workbench_shader_transparent_get(WORKBENCH_PrivateData *wpd);
GPUShader *workbench_shader_transparent_image_get(WORKBENCH_PrivateData *wpd, bool tiled);
GPUShader *workbench_shader_transparent_resolve_get(WORKBENCH_PrivateData *wpd);

void workbench_shader_library_ensure(void);
void workbench_shader_free(void);

/* workbench_deferred.c */
void workbench_deferred_engine_init(WORKBENCH_Data *vedata);
void workbench_deferred_engine_free(void);
void workbench_deferred_draw_scene(WORKBENCH_Data *vedata);
void workbench_deferred_draw_finish(WORKBENCH_Data *vedata);
void workbench_deferred_cache_init(WORKBENCH_Data *vedata);
void workbench_deferred_solid_cache_populate(WORKBENCH_Data *vedata, Object *ob);
void workbench_deferred_cache_finish(WORKBENCH_Data *vedata);

/* workbench_forward.c */
void workbench_forward_engine_init(WORKBENCH_Data *vedata);
void workbench_forward_engine_free(void);
void workbench_forward_draw_scene(WORKBENCH_Data *vedata);
void workbench_forward_draw_finish(WORKBENCH_Data *vedata);
void workbench_forward_cache_init(WORKBENCH_Data *vedata);
void workbench_forward_cache_populate(WORKBENCH_Data *vedata, Object *ob);
void workbench_forward_cache_finish(WORKBENCH_Data *vedata);

/* For OIT in deferred */
void workbench_forward_outline_shaders_ensure(WORKBENCH_PrivateData *wpd, eGPUShaderConfig sh_cfg);
void workbench_forward_choose_shaders(WORKBENCH_PrivateData *wpd, eGPUShaderConfig sh_cfg);
WORKBENCH_MaterialData *workbench_forward_get_or_create_material_data(WORKBENCH_Data *vedata,
                                                                      Object *ob,
                                                                      Material *mat,
                                                                      Image *ima,
                                                                      ImageUser *iuser,
                                                                      int color_type,
                                                                      int interp);

/* workbench_effect_aa.c */
void workbench_aa_create_pass(WORKBENCH_Data *vedata, GPUTexture **tx);
void workbench_aa_draw_pass(WORKBENCH_Data *vedata, GPUTexture *tx);

/* workbench_effect_fxaa.c */
void workbench_fxaa_engine_init(void);
void workbench_fxaa_engine_free(void);
DRWPass *workbench_fxaa_create_pass(GPUTexture **color_buffer_tx);

/* workbench_effect_taa.c */
void workbench_taa_engine_init(WORKBENCH_Data *vedata);
void workbench_taa_engine_free(void);
DRWPass *workbench_taa_create_pass(WORKBENCH_Data *vedata, GPUTexture **color_buffer_tx);
void workbench_taa_draw_scene_start(WORKBENCH_Data *vedata);
void workbench_taa_draw_scene_end(WORKBENCH_Data *vedata);
void workbench_taa_view_updated(WORKBENCH_Data *vedata);
int workbench_taa_calculate_num_iterations(WORKBENCH_Data *vedata);
int workbench_num_viewport_rendering_iterations(WORKBENCH_Data *vedata);

/* workbench_effect_dof.c */
void workbench_dof_engine_init(WORKBENCH_Data *vedata, Object *camera);
void workbench_dof_engine_free(void);
void workbench_dof_create_pass(WORKBENCH_Data *vedata,
                               GPUTexture **dof_input,
                               GPUTexture *noise_tex);
void workbench_dof_draw_pass(WORKBENCH_Data *vedata);

/* workbench_materials.c */
int workbench_material_determine_color_type(WORKBENCH_PrivateData *wpd,
                                            Image *ima,
                                            Object *ob,
                                            bool use_sculpt_pbvh);
void workbench_material_get_image_and_mat(
    Object *ob, int mat_nr, Image **r_image, ImageUser **r_iuser, int *r_interp, Material **r_mat);
char *workbench_material_build_defines(WORKBENCH_PrivateData *wpd,
                                       bool is_uniform_color,
                                       bool is_hair,
                                       bool is_tiled,
                                       const WORKBENCH_ColorOverride color_override);
void workbench_material_update_data(WORKBENCH_PrivateData *wpd,
                                    Object *ob,
                                    Material *mat,
                                    WORKBENCH_MaterialData *data,
                                    int color_type);
uint workbench_material_get_hash(WORKBENCH_MaterialData *material_template, bool is_ghost);
int workbench_material_get_composite_shader_index(WORKBENCH_PrivateData *wpd);
int workbench_material_get_prepass_shader_index(WORKBENCH_PrivateData *wpd,
                                                bool is_uniform_color,
                                                bool is_hair,
                                                bool is_tiled,
                                                const WORKBENCH_ColorOverride color_override);
int workbench_material_get_accum_shader_index(WORKBENCH_PrivateData *wpd,
                                              bool is_uniform_color,
                                              bool is_hair,
                                              bool is_tiled,
                                              const WORKBENCH_ColorOverride color_override);
void workbench_material_shgroup_uniform(WORKBENCH_PrivateData *wpd,
                                        DRWShadingGroup *grp,
                                        WORKBENCH_MaterialData *material,
                                        Object *ob,
                                        const bool deferred,
                                        const bool is_tiled,
                                        const int interp);
void workbench_material_copy(WORKBENCH_MaterialData *dest_material,
                             const WORKBENCH_MaterialData *source_material);

void workbench_material_ubo_data(WORKBENCH_PrivateData *wpd,
                                 Object *ob,
                                 Material *mat,
                                 WORKBENCH_UBO_Material *data,
                                 int color_type);

DRWShadingGroup *workbench_material_setup(WORKBENCH_PrivateData *wpd,
                                          Object *ob,
                                          int mat_nr,
                                          int color_type);
DRWShadingGroup *workbench_image_setup(
    WORKBENCH_PrivateData *wpd, Object *ob, int mat_nr, Image *ima, ImageUser *iuser, int interp);

/* workbench_studiolight.c */
void studiolight_update_world(WORKBENCH_PrivateData *wpd,
                              StudioLight *sl,
                              WORKBENCH_UBO_World *wd);
void studiolight_update_light(WORKBENCH_PrivateData *wpd, const float light_direction[3]);
bool studiolight_object_cast_visible_shadow(WORKBENCH_PrivateData *wpd,
                                            Object *ob,
                                            WORKBENCH_ObjectData *oed);
float studiolight_object_shadow_distance(WORKBENCH_PrivateData *wpd,
                                         Object *ob,
                                         WORKBENCH_ObjectData *oed);
bool studiolight_camera_in_object_shadow(WORKBENCH_PrivateData *wpd,
                                         Object *ob,
                                         WORKBENCH_ObjectData *oed);

/* workbench_data.c */
void workbench_effect_info_init(WORKBENCH_EffectInfo *effect_info);
void workbench_private_data_init(WORKBENCH_PrivateData *wpd);
void workbench_private_data_free(WORKBENCH_PrivateData *wpd);
void workbench_private_data_get_light_direction(float r_light_direction[3]);
void workbench_clear_color_get(float color[4]);
void workbench_update_material_ubos(WORKBENCH_PrivateData *wpd);
struct GPUUniformBuffer *workbench_material_ubo_alloc(WORKBENCH_PrivateData *wpd);

/* workbench_volume.c */
void workbench_volume_engine_init(void);
void workbench_volume_engine_free(void);
void workbench_volume_cache_init(WORKBENCH_Data *vedata);
void workbench_volume_cache_populate(WORKBENCH_Data *vedata,
                                     Scene *scene,
                                     Object *ob,
                                     struct ModifierData *md);
void workbench_volume_smoke_textures_free(WORKBENCH_PrivateData *wpd);

/* workbench_render.c */
void workbench_render(WORKBENCH_Data *vedata,
                      struct RenderEngine *engine,
                      struct RenderLayer *render_layer,
                      const struct rcti *rect);
void workbench_render_update_passes(struct RenderEngine *engine,
                                    struct Scene *scene,
                                    struct ViewLayer *view_layer);

#endif
