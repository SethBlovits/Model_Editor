#ifndef SHADER_REGISTRY_H
#define SHADER_REGISTRY_H


#include "slugs_graphics.h"
#include "imgui_hlsl.h"
#include "offscreen_pass_hlsl.h"
#include "ubershader_hlsl.h"
#include "ubshader_debug_hlsl.h"

typedef struct{
	const char* hlsl_filename;
	slg_shader_desc shader_desc;
	slg_shader shd;
}shader_registry_entry;
shader_registry_entry shader_registry[4];

void init_shader_registry();
slg_shader get_shader_from_registry(char* shader_name);
#ifdef SHADER_REGISTRY_IMPLEMENTATION

void init_shader_registry(){
    int shader_registry_count = 4;
	shader_registry[0] = (shader_registry_entry){"imgui_hlsl.h",IMGUI_SHADER_DESC};
	shader_registry[1] = (shader_registry_entry){"offscreen_pass_hlsl.h",OFFSCREEN_PASS_SHADER_DESC};
	shader_registry[2] = (shader_registry_entry){"ubershader_hlsl.h",UBERSHADER_SHADER_DESC};
	shader_registry[3] = (shader_registry_entry){"ubshader_debug_hlsl.h",UBSHADER_DEBUG_SHADER_DESC};
    for(int i = 0;i<shader_registry_count;i++){
    	shader_registry[i].shd = slg_make_shader(&shader_registry[i].shader_desc);
    }
}
slg_shader get_shader_from_registry(char* shader_name){
    int shader_registry_count = 0;
    for(int i = 0;i<shader_registry_count;i++){
        if(!strcmp(shader_registry[i].hlsl_filename,shader_name)){
            return shader_registry[i].shd;
        }
    }
    return (slg_shader){0};
}
#endif //SHADER_REGISTRY_IMPLEMENTATION
#endif //SHADER_REGISTRY_H
