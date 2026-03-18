#ifndef imgui_hlsl_h
#define imgui_hlsl_h

#if !defined(SLUGS_GRAPHICS_H)
#error "Please include slugs_graphics.h before including this file"
#endif
#define HLSL_SHADER_SOURCE_IMGUI "imgui.hlsl"
#define VERTEX_SHADER_SOURCE_IMGUI "imgui_vs.cso" 
#define FRAGMENT_SHADER_SOURCE_IMGUI "imgui_ps.cso" 
#define BINDSLOT_IMGUI_MVPConstantBuffer 0
#define BINDSLOT_IMGUI_g_sampler 0
#define BINDSLOT_IMGUI_g_texture 0

#define IMGUI_SHADER_DESC (slg_shader_desc){\
.filename = "imgui.hlsl",\
.vert_shader_target = "vs_5_0",\
.vert_shader_name = "imgui_vs.cso",\
.frag_shader_target = "ps_5_0",\
.frag_shader_name = "imgui_ps.cso"\
}

typedef struct IMGUI_HLSL_UNIFORMS{
	slg_buffer MVPConstantBuffer;
	slg_texture g_texture;
}IMGUI_HLSL_UNIFORMS;

slg_uniforms IMGUI_HLSL_MAKE_UNIFORMS(IMGUI_HLSL_UNIFORMS uniform_desc){
	slg_uniforms out_uniforms = {0};
	out_uniforms.cbv_buffer[BINDSLOT_IMGUI_MVPConstantBuffer] = uniform_desc.MVPConstantBuffer;
	out_uniforms.samplers[BINDSLOT_IMGUI_g_sampler] = true;
	out_uniforms.srv_buffer[BINDSLOT_IMGUI_g_texture] = uniform_desc.g_texture;
	return out_uniforms;
}
#endif