#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <stdio.h>
#include <string.h>
#include "mathUtil.h"
#include <assert.h>
#if !defined(SLUGS_GRAPHICS_H)
#error "Please include slugs_graphics.h before including this file"
#endif


typedef struct{
    char model_path[MAX_PATH];
    char albedo_tex_path[MAX_PATH];
    char shader_path[MAX_PATH];
}Asset_t;

typedef struct{
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
}asset_transform; 

typedef struct{
    GLTF_Data model_data;

    slg_pipeline pip;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
}Object_t;
#define ASSET_MANAGER_IMPLEMENTATION


//smaller function to get bools. Will be reused alot
bool get_bool_member(const char* field,FILE* fileptr){
    char buffer[MAX_PATH];
    long section_start = ftell(fileptr);
    char field_value[MAX_PATH];
    int field_value_index = 0;

    while(fgets(buffer,MAX_PATH,fileptr)){
        char* field_char = strstr(buffer,field);
        if(field_char){
            field_char += strlen(field) + 1;
            while(*field_char != ';' && *field_char != '\n'){
                field_value[field_value_index] = *field_char;
                field_char++;
                field_value_index++;
                assert(field_value_index < MAX_PATH);
            }
            field_value[field_value_index] = '\0';
            if(strstr(field_value,"true")){
                fseek(fileptr,section_start,SEEK_SET);
                return true;
            }
            else{
                fseek(fileptr,section_start,SEEK_SET);
                return false;
            }
        
        }
    }

    
    return false;

}

int get_int_member(const char* field,FILE* fileptr){
    char buffer[MAX_PATH];
    long section_start = ftell(fileptr);
    char field_value[MAX_PATH];
    int field_value_index = 0;

    while(fgets(buffer,MAX_PATH,fileptr)){
        char* field_char = strstr(buffer,field);
        if(field_char){
            field_char += strlen(field) + 1;
            while(*field_char != ';' && *field_char != '\n'){
                field_value[field_value_index] = *field_char;
                field_char++;
                field_value_index++;
                assert(field_value_index < MAX_PATH);
            }
            field_value[field_value_index] = '\0';
            fseek(fileptr,section_start,SEEK_SET);
            char* endptr;
            long num = strtol(field_value, &endptr, 10);
            return (int)num;
        }
    }

    
    return 0;



}
float get_float_member(const char* field,FILE* fileptr){
    char buffer[MAX_PATH];
    long section_start = ftell(fileptr);
    char field_value[MAX_PATH];
    int field_value_index = 0;

    while(fgets(buffer,MAX_PATH,fileptr)){
        char* field_char = strstr(buffer,field);
        if(field_char){
            field_char += strlen(field) + 1;
            while(*field_char != ';' && *field_char != '\n'){
                field_value[field_value_index] = *field_char;
                field_char++;
                field_value_index++;
                assert(field_value_index < MAX_PATH);
            }
            field_value[field_value_index] = '\0';
            fseek(fileptr,section_start,SEEK_SET);
            char* endptr;
            float num = strtof(field_value, &endptr);
            return num;
        }
    }
    return 0.0f;
}


void pull_member_filepath(Asset_t* asset,char* sub_string,FILE* fileptr){

    char buffer[MAX_PATH];
    
    while(fgets(buffer,MAX_PATH,fileptr)){
        char* sub_start = strstr(buffer,sub_string);
        //this returns a pointer to the start of the substring
        if(sub_start){
            sub_start += strlen(sub_string) + 1;
            //char* curr_char = (sub_start + strlen(sub_string)) - buffer - 1;
            int path_index = 0;
            while(*sub_start != ';' && *sub_start != '\n' && *sub_start != '\0'){
                asset->model_path[path_index] = *sub_start;
                sub_start++;
                path_index++;
                
            }
            asset->model_path[path_index+1] = '\0';
            
        }

    }
    fseek(fileptr, 0, SEEK_SET);
    return;

}

void pull_member_vector3(Vector3* object_vec,char* sub_string,FILE* fileptr){
    char buffer[MAX_PATH];

    while(fgets(buffer,MAX_PATH,fileptr)){
        char* sub_start = strstr(buffer,sub_string);
        //this returns a pointer to the start of the substring
        if(sub_start){
            sub_start += strlen(sub_string) + 1;
            //increment pointer until we reach the first number
            while(*sub_start != '('){
                sub_start++;
            }
            sub_start++;
            int val_index = 0;
            char val[MAX_PATH] = {0};
            while(*sub_start != ','){
                val[val_index] = *sub_start;
                val_index++;
                sub_start++;
            }
            sub_start++;
            val[val_index] = '\0';
            char* endptr = {0};
            object_vec->x = strtof(val,&endptr);
            memset(val,0,MAX_PATH);
            val_index = 0;
            while(*sub_start != ','){
                val[val_index] = *sub_start;
                val_index++;
                sub_start++;
            }
            sub_start++;
            val[val_index] = '\0';
            object_vec->y = strtof(val,&endptr);
            memset(val,0,MAX_PATH);
            val_index = 0;
            while(*sub_start != ')'){
                val[val_index] = *sub_start;
                val_index++;
                sub_start++;
            }
            sub_start++;
            val[val_index] = '\0';
            object_vec->z = strtof(val,&endptr);
            break;
            /*while(*sub_start != ';' && *sub_start != '\n' && *sub_start != '\0'){
                asset->model_path[path_index] = *sub_start;
                sub_start++;
                path_index++;
                
            }
            asset->model_path[path_index+1] = '\0';
            */
        }

    }
    fseek(fileptr, 0, SEEK_SET);
    return;
}

void pull_transform(asset_transform* transform,FILE* fileptr){
    pull_member_vector3(&transform->position,"Position",fileptr);
    pull_member_vector3(&transform->rotation,"Rotation",fileptr);
    pull_member_vector3(&transform->scale,"Scale",fileptr);
}
void pull_depth_stencil(slg_depth_stencil_desc* depth_desc,FILE* fileptr){

    char buffer[MAX_PATH];
    long section_start = ftell(fileptr);

    char* field_char = NULL;
    char field_value[MAX_PATH];
    int field_value_index = 0;

    while(fgets(buffer,MAX_PATH,fileptr)){
        if(strstr(buffer,"# Depth Stencil")){
            section_start = ftell(fileptr);
            break;
        }
    }

    depth_desc->depth_enable = get_bool_member("Depth_Enable",fileptr);
    depth_desc->stencil_enable = get_bool_member("Stencil_Enable",fileptr);
    depth_desc->write_mask = get_int_member("Depth_Write_Mask",fileptr);
    depth_desc->compare_func = get_int_member("Depth_Compare_Func",fileptr);

    fseek(fileptr,0,SEEK_SET);

}

void pull_rasterizer_desc(slg_rasterizer_desc* rasterizer_desc,FILE* fileptr){
    char buffer[MAX_PATH];
    long section_start = ftell(fileptr);

    char* field_char = NULL;
    char field_value[MAX_PATH];
    int field_value_index = 0;

    while(fgets(buffer,MAX_PATH,fileptr)){
        if(strstr(buffer,"# Rasterizer")){
            section_start = ftell(fileptr);
            break;
        }
    }

    rasterizer_desc->cull_mode = get_int_member("Cull_Mode", fileptr);
    rasterizer_desc->fill_mode = get_int_member("Fill_Mode", fileptr);
    rasterizer_desc->facewinding_mode = get_int_member("Facewinding_Mode", fileptr);
    rasterizer_desc->conservative_rasterization_mode = get_int_member("Conservative_Rasterization_Mode", fileptr);
    rasterizer_desc->depth_bias = get_int_member("Depth_Bias", fileptr);
    rasterizer_desc->depth_bias_clamp = get_float_member("Depth_Bias_Clamp", fileptr);
    rasterizer_desc->slope_scaled_depth_bias = get_float_member("Slope_Scaled_Depth_Bias", fileptr);
    rasterizer_desc->depth_clip_enable = get_bool_member("Depth_Clip_Enable", fileptr);
    rasterizer_desc->multisample_enable = get_bool_member("Multisample_Enable", fileptr);
    rasterizer_desc->antialiased_line_enable = get_bool_member("Antialiased_Line_Enable", fileptr);
    rasterizer_desc->forced_sample_count = get_int_member("Forced_Sample_Count", fileptr);

    fseek(fileptr,0,SEEK_SET);
}

void pull_blend_desc(slg_blend_desc* blend_desc,FILE* fileptr){
    char buffer[MAX_PATH];
    long section_start = ftell(fileptr);

    char* field_char = NULL;
    char field_value[MAX_PATH];
    int field_value_index = 0;

    while(fgets(buffer,MAX_PATH,fileptr)){
        if(strstr(buffer,"# Rasterizer")){
            section_start = ftell(fileptr);
            break;
        }
    }

    // Blend desc
    blend_desc->alpha_to_coverage_enable = get_bool_member("Alpha_To_Coverage_Enable", fileptr);
    blend_desc->independent_blend_enable = get_bool_member("Independent_Blend_Enable", fileptr);
    blend_desc->blend_enable = get_bool_member("Blend_Enable", fileptr);
    blend_desc->logic_op_enable = get_bool_member("Logic_Op_Enable", fileptr);
    blend_desc->src_blend = get_int_member("Src_Blend", fileptr);
    blend_desc->dest_blend = get_int_member("Dest_Blend", fileptr);
    blend_desc->blend_op = get_int_member("Blend_Op", fileptr);
    blend_desc->src_blend_alpha = get_int_member("Src_Blend_Alpha", fileptr);
    blend_desc->dest_blend_alpha = get_int_member("Dest_Blend_Alpha", fileptr);
    blend_desc->blend_op_alpha = get_int_member("Blend_Op_Alpha", fileptr);
    blend_desc->logic_op = get_int_member("Logic_Op", fileptr);
    blend_desc->color_write_enable.red_enable = get_int_member("Color_Write_Enable_Red", fileptr);
    blend_desc->color_write_enable.green_enable = get_int_member("Color_Write_Enable_Green", fileptr);
    blend_desc->color_write_enable.blue_enable = get_int_member("Color_Write_Enable_Blue", fileptr);
    blend_desc->color_write_enable.alpha_enable = get_int_member("Color_Write_Enable_Alpha", fileptr);

    fseek(fileptr,0,SEEK_SET);
}

Asset_t load_asset(char* asset_path,int path_size){
    Asset_t asset = {0};
    Object_t object = {0};
    FILE* fileptr = fopen(asset_path,"r");
    if(fileptr == NULL){
        printf("Couldn't open asset file");
    }

    pull_member_filepath(&asset,"Gltf_File",fileptr);
    asset_transform transform = {0};
    pull_transform(&transform,fileptr);
    //pull_member_vector3(&object.position,"Position",fileptr);
    //pull_member_vector3(&object.rotation,"Rotation",fileptr);
    //pull_member_vector3(&object.scale,"Scale",fileptr);

    //slg_pipeline_desc pip_desc = {0};

    //pull_depth_stencil(&pip_desc.depth_stencil_desc,fileptr);
    //pull_rasterizer_desc(&pip_desc.rasterizer_desc,fileptr);
    //pull_blend_desc(&pip_desc.blend_desc,fileptr);


    //pull_member(&asset_paths,"Albedo",fileptr);
    fclose(fileptr);
    //return asset_paths;
    //I could maybe read the shader helper file and search for the shader desc struct???
    //slg_shader shd = slg_make_shader();   
    //object.pip = slg_make_pipeline(&pip_desc);


    //I think the shader loading makes this kinda hard to do 
    //Since all the shader pipeline and binding information is runtime information
    return asset;
}



#ifdef ASSET_MANAGER_IMPLEMENTATION
    




    

#endif



#endif