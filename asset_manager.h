#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <stdio.h>
#include <string.h>
#if !defined(SLUGS_GRAPHICS_H)
#error "Please include slugs_graphics.h before including this file"
#endif


typedef struct{
    char model_path[MAX_PATH];
    char albedo_tex_path[MAX_PATH];
    char shader_path[MAX_PATH];
}pre_load_object_t;

typedef struct{
    GLTF_Data model_data;
}post_load_object_t;
#define ASSET_MANAGER_IMPLEMENTATION


void pull_member(pre_load_object_t* object,char* sub_string,FILE* fileptr){

    char buffer[MAX_PATH];

    while(fgets(buffer,MAX_PATH,fileptr)){
        char* sub_start = strstr(buffer,sub_string);
        //this returns a pointer to the start of the substring
        if(sub_start){
            int string_index = (sub_start + strlen(sub_string)) - buffer - 1;
            int path_index = 0;
            while(buffer[string_index] != ','){
                object->model_path[path_index] = buffer[string_index];
                path_index++;
                string_index++;
            }
            object->model_path[path_index+1] = '\0';
            
        }

    }
    fseek(fileptr, 0, SEEK_SET);

}

post_load_object_t load_asset(char* asset_path,int path_size){
    pre_load_object_t asset_paths = {0};

    FILE* fileptr = fopen(asset_path,"r");

    pull_member(&asset_paths,"Gltf_File = ",fileptr);
    pull_member(&asset_paths,"Albedo = ",fileptr);
    
    //return asset_paths;
    

}



#ifdef ASSET_MANAGER_IMPLEMENTATION
    




    

#endif



#endif