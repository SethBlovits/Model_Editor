
#include "slugs_graphics.h"
#include "imgui_hlsl.h"
#include "offscreen_pass_hlsl.h"
#include "ubershader_hlsl.h"
#include "ubshader_debug_hlsl.h"
typedef struct{
	const char* hlsl_filename;
	slg_shader_desc shader_desc;
}shader_registry_entry;
shader_registry_entry shader_registry[4];
int shader_registry_count = 4;
void init_shader_registry(){
	shader_registry[0] = (shader_registry_entry){"imgui_hlsl.h",IMGUI_SHADER_DESC};
	shader_registry[1] = (shader_registry_entry){"offscreen_pass_hlsl.h",OFFSCREEN_PASS_SHADER_DESC};
	shader_registry[2] = (shader_registry_entry){"ubershader_hlsl.h",UBERSHADER_SHADER_DESC};
	shader_registry[3] = (shader_registry_entry){"ubshader_debug_hlsl.h",UBSHADER_DEBUG_SHADER_DESC};
};

void get_shader_from_registry();