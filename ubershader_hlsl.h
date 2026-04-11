#ifndef ubershader_hlsl_h
#define ubershader_hlsl_h

#if !defined(SLUGS_GRAPHICS_H)
#error "Please include slugs_graphics.h before including this file"
#endif
#define HLSL_SHADER_SOURCE_UBERSHADER "ubershader.hlsl"
#define VERTEX_SHADER_SOURCE_UBERSHADER "ubershader_vs.cso" 
#define FRAGMENT_SHADER_SOURCE_UBERSHADER "ubershader_ps.cso" 
#define BINDSLOT_UBERSHADER_joint_matrices 1
#define BINDSLOT_UBERSHADER_TransformBuffer 0
#define BINDSLOT_UBERSHADER_FeatureFlags 2
#define BINDSLOT_UBERSHADER_g_sampler 0
#define BINDSLOT_UBERSHADER_albedo 0
#define BINDSLOT_UBERSHADER_LightPositions 1

#define UBERSHADER_SHADER_DESC (slg_shader_desc){\
.filename = "ubershader.hlsl",\
.vert_shader_target = "vs_5_0",\
.vert_shader_name = "ubershader_vs.cso",\
.frag_shader_target = "ps_5_0",\
.frag_shader_name = "ubershader_ps.cso"\
}

typedef struct UBERSHADER_HLSL_UNIFORMS{
	slg_buffer joint_matrices;
	slg_buffer TransformBuffer;
	slg_buffer FeatureFlags;
	slg_texture albedo;
	slg_buffer LightPositions;
}UBERSHADER_HLSL_UNIFORMS;

slg_uniforms UBERSHADER_HLSL_MAKE_UNIFORMS(UBERSHADER_HLSL_UNIFORMS uniform_desc){
	slg_uniforms out_uniforms = {0};
	out_uniforms.srv_buffer[BINDSLOT_UBERSHADER_joint_matrices] = uniform_desc.joint_matrices;
	out_uniforms.cbv_buffer[BINDSLOT_UBERSHADER_TransformBuffer] = uniform_desc.TransformBuffer;
	out_uniforms.cbv_buffer[BINDSLOT_UBERSHADER_FeatureFlags] = uniform_desc.FeatureFlags;
	out_uniforms.samplers[BINDSLOT_UBERSHADER_g_sampler] = true;
	out_uniforms.srv_buffer[BINDSLOT_UBERSHADER_albedo] = uniform_desc.albedo;
	out_uniforms.cbv_buffer[BINDSLOT_UBERSHADER_LightPositions] = uniform_desc.LightPositions;
	return out_uniforms;
}
#endif