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
        if(strstr(buffer,"Depth Stencil")){
            section_start = ftell(fileptr);
            break;
        }
    }

    depth_desc->depth_enable = get_bool_member("Depth_Enable",fileptr);
    depth_desc->stencil_enable = get_bool_member("Stencil_Enable",fileptr);
    depth_desc->write_mask = get_int_member("Depth_Write_Mask",fileptr);
    depth_desc->compare_func = get_int_member("Depth_Compare_Func",fileptr);


    /*  field_char = strstr(buffer,"Depth_Enable:");
        if(field_char){
            field_char += strlen("Depth_Enable:");
            while(*field_char != ';' && field_char != '\n'){
                field_value[field_value_index] = *field_char;
                field_char++;
                field_value_index++;
                assert(field_value_index < MAX_PATH);
            }
            field_value[field_value_index] = '\0';
            char* endptr;
            
            if(strstr(field_value,"true")){
                depth_desc->depth_enable = true;
            }
            else{
                depth_desc->depth_enable = false;
            }
            memset(field_value,0,MAX_PATH);
            field_value_index = 0;
            //fseek(fileptr,section_start,SEEK_SET);
        }
    */
    /*
        field_char = strstr(buffer,"Stencil_Enable:");
        if(field_char){
            field_char += strlen("Stencil_Enable:");
            while(*field_char != ';' && field_char != '\n'){
                field_value[field_value_index] = *field_char;
                field_char++;
                field_value_index++;
                assert(field_value_index < MAX_PATH);
            }
            field_value[field_value_index] = '\0';
            char* endptr;
           
            if(strstr(field_value,"true")){
                depth_desc->stencil_enable = true;
            }
            else{
                depth_desc->stencil_enable = false;
            }
            memset(field_value,0,MAX_PATH);
            field_value_index = 0;
            //fseek(fileptr,section_start,SEEK_SET);
        }
    */  
}



//if i dont make the asset file standardized then I have to check for each parameter
void make_asset_pipeline(slg_pipeline* pip,FILE* fileptr){
    //char buffer[MAX_PATH];

   /* while(fgets(buffer,MAX_PATH,)){
        char* sub_start = strstr(buffer,"Pipeline Desc:");
    }*/


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

    slg_pipeline_desc pip_desc = {0};

    pull_depth_stencil(&pip_desc.depth_stencil_desc,fileptr);

    //pull_member(&asset_paths,"Albedo",fileptr);
    fclose(fileptr);
    //return asset_paths;
    

}



#ifdef ASSET_MANAGER_IMPLEMENTATION
    




    

#endif



#endif