#ifndef ubshader_debug_hlsl_h
#define ubshader_debug_hlsl_h

#if !defined(SLUGS_GRAPHICS_H)
#error "Please include slugs_graphics.h before including this file"
#endif
#define HLSL_SHADER_SOURCE_UBSHADER_DEBUG "ubshader_debug.hlsl"
#define VERTEX_SHADER_SOURCE_UBSHADER_DEBUG "ubshader_debug_vs.cso" 
#define FRAGMENT_SHADER_SOURCE_UBSHADER_DEBUG "ubshader_debug_ps.cso" 
#define BINDSLOT_UBSHADER_DEBUG_TransformBuffer 0
#define BINDSLOT_UBSHADER_DEBUG_g_sampler 0
#define BINDSLOT_UBSHADER_DEBUG_albedo 0
#define BINDSLOT_UBSHADER_DEBUG_LightPositions 1

#define UBSHADER_DEBUG_SHADER_DESC (slg_shader_desc){\
.filename = "ubshader_debug.hlsl",\
.vert_shader_target = "vs_5_0",\
.vert_shader_name = "ubshader_debug_vs.cso",\
.frag_shader_target = "ps_5_0",\
.frag_shader_name = "ubshader_debug_ps.cso"\
}

typedef struct UBSHADER_DEBUG_HLSL_UNIFORMS{
	slg_buffer TransformBuffer;
	slg_texture albedo;
	slg_buffer LightPositions;
}UBSHADER_DEBUG_HLSL_UNIFORMS;

slg_uniforms UBSHADER_DEBUG_HLSL_MAKE_UNIFORMS(UBSHADER_DEBUG_HLSL_UNIFORMS uniform_desc){
	slg_uniforms out_uniforms = {0};
	out_uniforms.cbv_buffer[BINDSLOT_UBSHADER_DEBUG_TransformBuffer] = uniform_desc.TransformBuffer;
	out_uniforms.samplers[BINDSLOT_UBSHADER_DEBUG_g_sampler] = true;
	out_uniforms.srv_buffer[BINDSLOT_UBSHADER_DEBUG_albedo] = uniform_desc.albedo;
	out_uniforms.cbv_buffer[BINDSLOT_UBSHADER_DEBUG_LightPositions] = uniform_desc.LightPositions;
	return out_uniforms;
}
#endif