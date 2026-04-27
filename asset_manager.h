#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <stdio.h>
#include <string.h>
#include "mathUtil.h"
#if !defined(SLUGS_GRAPHICS_H)
#error "Please include slugs_graphics.h before including this file"
#endif


typedef struct{
    char model_path[MAX_PATH];
    char albedo_tex_path[MAX_PATH];
    char shader_path[MAX_PATH];
}Asset_t;

typedef struct{
    GLTF_Data model_data;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
}Object_t;
#define ASSET_MANAGER_IMPLEMENTATION


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
            int path_index = 0;
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
            object_vec->y = strtof(val,endptr);
            memset(val,0,MAX_PATH);
            val_index = 0;
            while(*sub_start != ')'){
                val[val_index] = *sub_start;
                val_index++;
                sub_start++;
            }
            sub_start++;
            val[val_index] = '\0';
            object_vec->z = strtof(val,endptr);
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
Asset_t load_asset(char* asset_path,int path_size){
    Asset_t asset = {0};
    Object_t object = {0};
    FILE* fileptr = fopen(asset_path,"r");
    if(fileptr == NULL){
        printf("Couldn't open asset file");
    }

    pull_member_filepath(&asset,"Gltf_File",fileptr);
    pull_member_vector3(&object.position,"Position",fileptr);
    //pull_member(&asset_paths,"Albedo",fileptr);
    fclose(fileptr);
    //return asset_paths;
    

}



#ifdef ASSET_MANAGER_IMPLEMENTATION
    




    

#endif



#endif