
#include <windows.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define APP_UTILITY_IMPLEMENTATION
#include "app_utility.h"
//#include <d3d12.h>
//#include <dxgi1_6.h>
//#include <d3dcompiler.h>
#define _DEBUG
#define SLUGS_IMPLEMENTATION
#define SLG_HOT_RELOADING
#include "slugs_graphics.h"
#define SLUGS_IMGUI_IMPLEMENTATION
#include "slugs_imgui.h"
#include "mathUtil.h"
#include "modelData.h"
#define ANIMATION_UTIL_IMPLEMENTATION
#include "animationUtil.h"

#include "offscreen_pass_hlsl.h"
#include "ubershader_hlsl.h"
#include "ubshader_debug_hlsl.h"

#define APP_WIDTH 1920
#define APP_HEIGHT 1080

//struct to hold the spacing for all the options in the parameter panel

/* 
|This |
---------------------------------
|     |                         |
|     |                         |
|     |                         |
|     |                         | 
|     |                         |
---------------------------------
*/
struct{
    float panel_horiz;

    float name_vert;
    float tex_vert;
    ImVec2 tex_image_size;

    float properties_vert;
    float shader_vert;

}param_layout = {
    .name_vert = 0.1f,
    .panel_horiz = 0.1f,

    .tex_vert = 0.2f,
    .tex_image_size = {0.2f,0.2f}, 

    .properties_vert = 0.5f,

    .shader_vert = 0.2f

};

ImVec2 mouse_rotation = {0,0};

int selected_button_index = -1;

typedef struct Render_Window{
    int width;
    int height;
    const char* name;
    bool m_useWarpDevice;
}Render_Window;
const char g_szClassName[] = "myWindowClass";

typedef struct{
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
}Vertex;

typedef struct{
    Vector3 movevector;
    Vector3 position;
    Vector3 direction;
    Vector3 target;
    Vector3 up;
    Vector3 right;
    Mat4 view;
    Mat4 projection;
    float near_plane;
    float far_plane;
}Camera;
Camera offscreen_camera;

struct{
    Mat4 mvp_mat;
    Mat4 model_mat;
    Mat4 normal_mat;
}Transform_Matrices; 
slg_buffer transform_buffer;

typedef enum{
    LIGHT_TYPE_DIRECTIONAL = 0,
    LIGHT_TYPE_POINT = 1,
    LIGHT_TYPE_SPOT = 2
}LIGHT_TYPE;    
typedef struct{
    Vector3 position; // 12 bytes
    float  intensity; // 4 bytes
    Vector3 color; // 12 bytes
    float  radius; // 4 bytes
    Vector3 direction;// 12 bytes
    int light_type; // 4 bytes
}light;
typedef struct{
    light lights[16];
    int num_lights;
    float padding[3];
}light_sources;
typedef struct{
    int has_skinning;
    int has_tangents;
    int has_albedo;
    int has_specular;
    int has_normal_map;
    int has_emissive;
    int has_metallic_roughness;
    int pad;
}feature_flags;

light main_light;
slg_buffer light_buffer;
slg_pass offscreen_pass = {0};

struct{
    bool changed_resolution;
    int new_width;
    int new_height;
}pass_resize;

struct{
    GLTF_Data gltf_data;
    AABB bounding_box;
    slg_texture albedo;
    slg_render_texture normal;
}object_data;


slg_buffer ubershader_vert_buffer;
slg_buffer ubershader_index_buffer;
slg_bindings ubershader_bindings; //this so we can make the generic bindings as we swap in the new info
slg_pipeline ubershader_pip;

slg_pipeline debug_pipeline;
slg_bindings debug_bindings;

Arena gltf_load_arena;
uint8_t gltf_load_arena_backing_buffer[1048576];

Vertex* fox_vertex;
size_t fox_vertex_size;  
uint16_t* fox_index;
size_t fox_index_size;


void load_gltf(char* path, int path_size);
void init(){

    //uint8_t arena_backingBuffer[131072];
    //arena_init(&slg_arena,arena_backingBuffer,131072);

    arena_init(&gltf_load_arena,gltf_load_arena_backing_buffer,1048576);
    gltf_load_arena.name = "gltf_arena";
    //slg_arena.name = "main_arena";
    slg_d3d12_state.appdata.width = APP_WIDTH;
    slg_d3d12_state.appdata.height = APP_HEIGHT;
    slg_d3d12_state.appdata.name = "test app";
    slg_d3d12_state.appdata.hwnd = app_get_window_handle();
    slg_setup();
    slimgui_setup();

    //DEMO CUBE RESOURCES

    Vertex cubeVertices[] = {
       // Front face (-Z)  normal = 0, 0, -1
    { {-1,-1,-1}, { 0, 0,-1}, {0,1} },
    { { 1,-1,-1}, { 0, 0,-1}, {1,1} },
    { { 1, 1,-1}, { 0, 0,-1}, {1,0} },
    { {-1, 1,-1}, { 0, 0,-1}, {0,0} },
    // Back face (+Z)   normal = 0, 0, 1
    { {-1,-1, 1}, { 0, 0, 1}, {1,1} },
    { { 1,-1, 1}, { 0, 0, 1}, {0,1} },
    { { 1, 1, 1}, { 0, 0, 1}, {0,0} },
    { {-1, 1, 1}, { 0, 0, 1}, {1,0} },
    // Left face (-X)   normal = -1, 0, 0
    { {-1,-1,-1}, {-1, 0, 0}, {0,1} },
    { {-1, 1,-1}, {-1, 0, 0}, {0,0} },
    { {-1, 1, 1}, {-1, 0, 0}, {1,0} },
    { {-1,-1, 1}, {-1, 0, 0}, {1,1} },
    // Right face (+X)  normal = 1, 0, 0
    { { 1,-1,-1}, { 1, 0, 0}, {1,1} },
    { { 1, 1,-1}, { 1, 0, 0}, {1,0} },
    { { 1, 1, 1}, { 1, 0, 0}, {0,0} },
    { { 1,-1, 1}, { 1, 0, 0}, {0,1} },
    // Bottom face (-Y) normal = 0, -1, 0
    { {-1,-1,-1}, { 0,-1, 0}, {0,1} },
    { {-1,-1, 1}, { 0,-1, 0}, {0,0} },
    { { 1,-1, 1}, { 0,-1, 0}, {1,0} },
    { { 1,-1,-1}, { 0,-1, 0}, {1,1} },
    // Top face (+Y)    normal = 0, 1, 0
    { {-1, 1,-1}, { 0, 1, 0}, {0,0} },
    { {-1, 1, 1}, { 0, 1, 0}, {0,1} },
    { { 1, 1, 1}, { 0, 1, 0}, {1,1} },
    { { 1, 1,-1}, { 0, 1, 0}, {1,0} },
    };

    uint16_t cubeIndices[] = {
        0, 1, 2,  0, 2, 3,
        6, 5, 4,  7, 6, 4,
        8, 9, 10,  8, 10, 11,
        14, 13, 12,  15, 14, 12,
        16, 17, 18,  16, 18, 19,
        22, 21, 20,  23, 22, 20
    };
    
    slg_buffer index_buffer = slg_make_buffer(&(slg_buffer_desc){
        .buffer = cubeIndices,
        .buffer_size = sizeof(cubeIndices),
        .buffer_stride = sizeof(uint16_t),
    });
    slg_buffer vertex_buffer = slg_make_buffer(&(slg_buffer_desc){
        .buffer = cubeVertices,
        .buffer_size = sizeof(cubeVertices),
        .buffer_stride = sizeof(Vertex)
    });

    uint32_t pass_pixels[4*4] = { //test texture data
        0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000,
        0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000,
        0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF,
    };
    slg_texture texture = slg_make_texture(&(slg_texture_desc){
        .texture = (void*)pass_pixels,
        .width = 4,
        .height = 4,
        .pixel_size = 4
    });

    object_data.albedo = texture;

    Mat4 mvpMat4 = identityMat4();
    Mat4 m_model = identityMat4();
    //m_model = scaledMat4(m_model,(Vector3){0.1f,0.1f,0.1f});
    offscreen_camera.position = (Vector3){0.0f,0.0f,10.0f};
    offscreen_camera.target = (Vector3){0.0f,0.0f,0.0f};
    offscreen_camera.direction = normalizeVec3(subtractVec3(offscreen_camera.position,offscreen_camera.target));
    offscreen_camera.near_plane = 0.1f;
    offscreen_camera.far_plane = 100.0f;
    //mainCamera.direction = normalizeVec3(subtractVec3(mainCamera.position,mainCamera.target));
    Vector3 up = {0.0f,1.0f,0.0f};
    offscreen_camera.right = normalizeVec3(crossVec3(up,offscreen_camera.direction));
    offscreen_camera.up = crossVec3(offscreen_camera.direction,offscreen_camera.right);

    //mainCamera.view = LookAt_RH(mainCamera.right,mainCamera.up,mainCamera.direction,mainCamera.position);
    offscreen_camera.view = LookAt_RH_Version2(offscreen_camera.position,offscreen_camera.target,up);
    float aspect = APP_WIDTH/APP_HEIGHT;
    offscreen_camera.projection = perspectiveMat4_Z0(1.0472f,aspect,offscreen_camera.near_plane,offscreen_camera.far_plane);
    mvpMat4 = mulMat4(mulMat4(offscreen_camera.projection,offscreen_camera.view),m_model);

    
    //make offscreen pass here
    //I want it to fill in all of the necessary things for me
    //so I can just call offscreen pass later
    slg_render_texture color_render_target = slg_make_render_target((slg_rt_desc){
        .width = 656,
        .height = 565,
        .clear_color = {0.0f,0.0f,0.0f,0.5f}

    });
    slg_depth_texture depth_render_target = slg_make_depth_target(656,565);

    slg_pipeline offscreen_pip = slg_make_pipeline(&(slg_pipeline_desc){
        .shader = slg_make_shader(&OFFSCREEN_PASS_SHADER_DESC),
        .depth_stencil_desc.depth_enable = true,
        .depth_stencil_desc.write_mask = SLG_DEPTH_WRITE_MASK_ALL,
        .depth_stencil_desc.compare_func = SLG_COMPARISON_FUNC_LESS,
        //.rasterizer_desc.facewinding_mode = SLG_FACEWINDING_CLOCKWISE
    });

   Transform_Matrices.model_mat = m_model;
   Transform_Matrices.mvp_mat = mvpMat4;
   Transform_Matrices.normal_mat = identityMat4();

    transform_buffer = slg_make_buffer(&(slg_buffer_desc){
        .buffer = (void*)&Transform_Matrices,
        .buffer_size = sizeof(Transform_Matrices),
        .buffer_stride = sizeof(Transform_Matrices),
        .usage = SLG_BUFFER_USAGE_CONSTANT_BUFFER
    });

    light_sources light_data = {0};
    /*printf("offsetof position: %d\n", offsetof(light, position));
    printf("offsetof intensity: %d\n", offsetof(light, intensity));
    printf("offsetof color: %d\n", offsetof(light, color));
    printf("offsetof radius: %d\n", offsetof(light, radius));
    printf("offsetof direction: %d\n", offsetof(light, direction));
    printf("offsetof light_type: %d\n", offsetof(light, light_type));
    printf("size light: %d\n", sizeof(light));*/
    main_light = (light){
        .position  = {1.2f, 0.5f, 2.5f},
        .intensity = 1.0f,
        .color     = {1.0f, 1.0f, 1.0f},
        .radius    = 10.0f,
        .direction = {-0.2f,-0.1f,-0.5f},
        .light_type = LIGHT_TYPE_DIRECTIONAL
    };
    light_data.num_lights      = 1;
    light_data.lights[0] = main_light;
   // printf("size light_sources: %d\n", sizeof(light_sources));
    light_buffer = slg_make_buffer(&(slg_buffer_desc){
        .buffer = (void*)&light_data,
        .buffer_size = sizeof(light_sources),
        .buffer_stride = sizeof(light_sources),
        .usage = SLG_BUFFER_USAGE_CONSTANT_BUFFER
    });
   

   
    /*for(int i = 0; i < fox_vertex_size/sizeof(Vertex); i++){
        printf("vert %d: pos = %f %f %f\n", i, fox_vertex[i].position.x, fox_vertex[i].position.y, fox_vertex[i].position.z);
    }*/
    /*slg_buffer fox_index_buffer = slg_make_buffer(&(slg_buffer_desc){
        .buffer = fox_index,
        .buffer_size = fox_index_size,
        .buffer_stride = sizeof(uint16_t),
    });
    slg_buffer fox_vertex_buffer = slg_make_buffer(&(slg_buffer_desc){
        .buffer = fox_vertex,
        .buffer_size = fox_vertex_size,
        .buffer_stride = sizeof(Vertex)
    });*/
    slg_bindings offscreen_bindings = slg_make_bindings(&(slg_bindings_desc){
        .index_buffer = index_buffer,
        .vertex_buffer = vertex_buffer,
        .uniforms = OFFSCREEN_PASS_HLSL_MAKE_UNIFORMS((OFFSCREEN_PASS_HLSL_UNIFORMS){
            .albedo = texture,
            .TransformBuffer = transform_buffer,
            .LightPositions = light_buffer,
        })
    });


    //THESE ARE THE THINGS WE NEED TO RENDER INTO THE OFFSCREEN PASS
    offscreen_pass.color_target = color_render_target;
    offscreen_pass.depth_target = depth_render_target;
    offscreen_pass.bind = offscreen_bindings;
    offscreen_pass.pip = offscreen_pip; 
    
    //load_gltf("C:\\MaterialEditor\\test_gltf\\Fox.gltf",MAX_PATH);
    slg_close_setup();

    igStyleColorsDark(igGetStyle());

    ImGuiStyle* style = igGetStyle();
    style->Colors[ImGuiCol_WindowBg]    = (ImVec4){0.08f, 0.08f, 0.08f, 1.0f};
    style->Colors[ImGuiCol_ChildBg]     = (ImVec4){0.10f, 0.10f, 0.10f, 1.0f};
    style->Colors[ImGuiCol_SliderGrab]  = (ImVec4){0.78f, 0.66f, 0.43f, 1.0f};
    style->Colors[ImGuiCol_CheckMark]   = (ImVec4){0.78f, 0.66f, 0.43f, 1.0f};
    style->Colors[ImGuiCol_Header]      = (ImVec4){0.78f, 0.66f, 0.43f, 0.3f};
    style->Colors[ImGuiCol_HeaderHovered] = (ImVec4){0.78f, 0.66f, 0.43f, 0.5f};
    style->Colors[ImGuiCol_FrameBg]        = (ImVec4){0.12f, 0.12f, 0.12f, 1.0f};
    style->Colors[ImGuiCol_FrameBgHovered] = (ImVec4){0.16f, 0.16f, 0.16f, 1.0f};
    style->Colors[ImGuiCol_FrameBgActive]  = (ImVec4){0.20f, 0.20f, 0.20f, 1.0f};
    style->FrameRounding  = 3.0f;
    style->GrabRounding   = 3.0f;
    style->WindowRounding = 0.0f;

}

void check_button_index(int *current_index,int *selected_index,const char* button_name, ImVec2 region_size){
    
    bool selected_style = (*current_index == *selected_index);
    
    if(selected_style){
        igPushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        igPushStyleColorImVec4(ImGuiCol_Border, (ImVec4){0.78f, 0.66f, 0.43f, 1.0f});
        igPushStyleColorImVec4(ImGuiCol_Button, (ImVec4){0.78f, 0.66f, 0.43f, 0.1f});
        igPushStyleColorImVec4(ImGuiCol_Text, (ImVec4){1.0f, 1.0f, 1.0f, 1.0f});
    }
 
    if(igButtonEx(button_name,(ImVec2){region_size.x,region_size.y * 0.15f})){
        *selected_index = *current_index;
    }
    if(selected_style){
        igPopStyleColorEx(3);
        igPopStyleVar();
    }
    (*current_index)++;
}

bool is_mouse_inside_widget(ImVec2 start_pos, ImVec2 size){

    //ImGuiIO* io = igGetIO();
    ImVec2 mouse_pos = io->MousePos;

    ImVec2 end_pos = (ImVec2){
        .x = {start_pos.x + size.x},
        .y = {start_pos.y + size.y}
    };
    if(mouse_pos.x < start_pos.x || mouse_pos.x > (end_pos.x)){
        return false;
    }
    if(mouse_pos.y < start_pos.y || mouse_pos.y > (end_pos.y)){
        return false;
    }

    return true;
}

//breakout function for the material editor code;;
void load_gltf(char* path, int path_size){
    //load gltf arena
    SetCurrentDirectory("C:\\MaterialEditor\\test_gltf");
    //this function pulls model data from a gltf file it can get the buffers and the all the rest of the data
    GLTF_Data gltf_model = getDataFromGltf(path, path_size);
    object_data.gltf_data = gltf_model;
    AABB model_bounds = calcAABBFromVertexBuffer(gltf_model.model_buffers.combinedVertBuffer,gltf_model.model_buffers.vbuffer_size);
    object_data.bounding_box = model_bounds;
    //we need to reposition the camera based upon the bounding box
    Vector3 aabb_center = {
        .x = (model_bounds.xmin + model_bounds.xmax) * 0.5f,
        .y = (model_bounds.ymin + model_bounds.ymax) * 0.5f,
        .z = (model_bounds.zmin + model_bounds.zmax) * 0.5f
    };

    Vector3 aabb_size = {
        .x = model_bounds.xmax - model_bounds.xmin,
        .y = model_bounds.ymax - model_bounds.ymin,
        .z = model_bounds.zmax - model_bounds.zmin,
    };
    
    float max_size = fmaxf(aabb_size.x,fmaxf(aabb_size.y,aabb_size.z));
    float fov = 1.0472f;
    float distance = (max_size * 0.5f) / tanf(fov * 0.5f);

    offscreen_camera.near_plane = 0.01f * distance;
    //offscreen_camera.near_plane = 0.01f;
    offscreen_camera.far_plane = distance * 10.0f;
    offscreen_camera.position = (Vector3){
        aabb_center.x,
        aabb_center.y,
        aabb_center.z + distance * 1.5f  // pull back far enough
    };
  

    memcpy(offscreen_camera.target.Elements,aabb_center.Elements,sizeof(float)*3);

    float aspect = (float)pass_resize.new_width/(float)pass_resize.new_height;

    offscreen_camera.projection = perspectiveMat4_Z0(1.0472f,aspect,offscreen_camera.near_plane,offscreen_camera.far_plane);

    fox_vertex_size = (gltf_model.model_buffers.vbuffer_size*sizeof(Vertex));
    fox_index_size = gltf_model.model_buffers.ibuffer_size*sizeof(uint16_t);
    fox_vertex = arena_alloc(&gltf_load_arena,gltf_model.model_buffers.vbuffer_size * sizeof(Vertex));
    fox_index = arena_alloc(&gltf_load_arena,gltf_model.model_buffers.ibuffer_size * sizeof(uint16_t));

    

    for(int i = 0;i<gltf_model.model_buffers.vbuffer_size;i++){
        fox_vertex[i].position = gltf_model.model_buffers.combinedVertBuffer[i].position;
        fox_vertex[i].normal = gltf_model.model_buffers.combinedVertBuffer[i].normal;
        fox_vertex[i].uv = gltf_model.model_buffers.combinedVertBuffer[i].uv;
    }
    for(int i = 0; i < gltf_model.model_buffers.ibuffer_size;i++){
        fox_index[i] = gltf_model.model_buffers.combinedIndexBuffer[i];
    }
    slg_buffer fox_index_buffer = slg_make_buffer(&(slg_buffer_desc){
        .buffer = fox_index,
        .buffer_size = (UINT)fox_index_size,
        .buffer_stride = sizeof(uint16_t),
    });
    slg_buffer fox_vertex_buffer = slg_make_buffer(&(slg_buffer_desc){
        .buffer = fox_vertex,
        .buffer_size = (UINT)fox_vertex_size,
        .buffer_stride = sizeof(Vertex)
    });
    ubershader_vert_buffer = slg_make_buffer(&(slg_buffer_desc){
        .buffer = gltf_model.model_buffers.combinedVertBuffer,
        .buffer_size = gltf_model.model_buffers.vbuffer_size * sizeof(Vertex_t),
        .buffer_stride = sizeof(Vertex_t)
    });

    ubershader_index_buffer = slg_make_buffer(&(slg_buffer_desc){
        .buffer = gltf_model.model_buffers.combinedIndexBuffer,
        .buffer_size = gltf_model.model_buffers.ibuffer_size * sizeof(uint16_t),
        .buffer_stride = sizeof(uint16_t)
    });

    feature_flags flags = {0};
    Mat4* skin_matrix;
    slg_buffer skin_buffer;
    if(gltf_model.model.numberOfAnimations> 0){
        flags.has_skinning = 1;
        skin_matrix = arena_alloc(&gltf_load_arena,gltf_model.model.numberOfNodes * sizeof(Mat4)); 
        recalculateLocalTransformMatrix(gltf_model.model.nodes,gltf_model.model.numberOfNodes);
        recalculateSkinningMatrix(gltf_model.model.nodes,gltf_model.model.numberOfNodes,(skinMatrix_t*)skin_matrix);
        skin_buffer = slg_make_buffer(&(slg_buffer_desc){
            .buffer = (void*)skin_matrix,
            .buffer_size = gltf_model.model.numberOfNodes * sizeof(Mat4),
            .buffer_stride = sizeof(Mat4),
            .usage = SLG_BUFFER_USAGE_CONSTANT_BUFFER
        });
    }
    else{
        skin_matrix = arena_alloc(&gltf_load_arena,sizeof(Mat4));
        skin_matrix[0] = identityMat4();
        skin_buffer = slg_make_buffer(&(slg_buffer_desc){
            .buffer = (void*)skin_matrix,
            .buffer_size = sizeof(Mat4),
            .buffer_stride = sizeof(Mat4),
            .usage = SLG_BUFFER_USAGE_CONSTANT_BUFFER
        }); 
    }
    //for now set skinning to be 0
    flags.has_skinning = 0;

    //this is only a temporary fix for models that have only 1 material
    //support is needed for models with multiple meshes that have different materials per mesh
    if(gltf_model.model.numberOfMaterials > 0){
        flags.has_albedo = gltf_model.model.materials[0].hasBaseColor;
        flags.has_emissive = gltf_model.model.materials[0].hasEmissive;
        flags.has_metallic_roughness = gltf_model.model.materials[0].hasMetallic;
        flags.has_normal_map = gltf_model.model.materials[0].hasNormal;
        flags.has_specular = gltf_model.model.materials[0].hasSpecular;
    }
    
    slg_buffer flags_buffer = slg_make_buffer(&(slg_buffer_desc){
        .buffer = (void*)&flags,
        .buffer_size = sizeof(flags),
        .buffer_stride = sizeof(flags),
        .usage = SLG_BUFFER_USAGE_CONSTANT_BUFFER
    });
    uint32_t default_pixel = 0xFFFFFFFF;
    slg_texture default_texture = slg_make_texture(&(slg_texture_desc){
        .height = 1,
        .width = 1,
        .pixel_size = 4,
        .tex_type = SLG_TEXTURE_TYPE_2D,
        .texture = (void*)&default_pixel 
    });
    slg_texture albedo_used;
    stbi_uc* pixels_albedo;
    if(!flags.has_albedo){
        albedo_used = default_texture;
    } 
    else{
        int png_width, png_height, num_channels;
        const int desired_channels = 4;
        pixels_albedo = stbi_load(gltf_model.model.materials[0].baseColorUri,&png_width,&png_height,&num_channels,desired_channels);
        
        albedo_used = slg_make_texture(&(slg_texture_desc){
            .height = png_height,
            .width = png_width,
            .pixel_size = desired_channels,
            .tex_type = SLG_TEXTURE_TYPE_2D,
            .texture = pixels_albedo 
        });

        stbi_image_free(pixels_albedo);
    }
    object_data.albedo = albedo_used;
    //work around to fix incorrect directory issue
    app_reset_working_directory();
    DXGI_FORMAT overrides[] = {
        DXGI_FORMAT_R32G32B32_FLOAT,
        DXGI_FORMAT_R32G32B32_FLOAT,
        DXGI_FORMAT_R32G32_FLOAT,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_R32G32B32A32_FLOAT
    };    
    ubershader_pip = slg_make_pipeline(&(slg_pipeline_desc){
        .shader = slg_make_shader(&UBERSHADER_SHADER_DESC),
        .depth_stencil_desc.depth_enable = true,
        .depth_stencil_desc.write_mask = SLG_DEPTH_WRITE_MASK_ALL,
        .depth_stencil_desc.compare_func = SLG_COMPARISON_FUNC_LESS,
        .rasterizer_desc.cull_mode = SLG_FACEWINDING_CLOCKWISE
    });

    ubershader_bindings = slg_make_bindings(&(slg_bindings_desc){
        .index_buffer = ubershader_index_buffer,
        .vertex_buffer = ubershader_vert_buffer,
        .uniforms = UBERSHADER_HLSL_MAKE_UNIFORMS((UBERSHADER_HLSL_UNIFORMS){
            .FeatureFlags = flags_buffer,
            .TransformBuffer = transform_buffer,
            .LightPositions = light_buffer,
            .albedo = albedo_used,
            .jointMat = skin_buffer
        })
    });

    /*debug_pipeline = slg_make_pipeline(&(slg_pipeline_desc){
        .shader = slg_make_shader(&UBSHADER_DEBUG_SHADER_DESC),
        .depth_stencil_desc.depth_enable = true,
        .depth_stencil_desc.write_mask = SLG_DEPTH_WRITE_MASK_ALL,
        .depth_stencil_desc.compare_func = SLG_COMPARISON_FUNC_LESS,
        .rasterizer_desc.cull_mode = SLG_FACEWINDING_CLOCKWISE,
        .num_overrides = 5,
        .format_overrides = overrides
    });
    debug_bindings = slg_make_bindings(&(slg_bindings_desc){
        .index_buffer = ubershader_index_buffer,
        .vertex_buffer = ubershader_vert_buffer,
        .uniforms = UBSHADER_DEBUG_HLSL_MAKE_UNIFORMS((UBSHADER_DEBUG_HLSL_UNIFORMS){
            .FeatureFlags = flags_buffer,
            .TransformBuffer = transform_buffer,
            .LightPositions = light_buffer,
            .albedo = albedo_used,
            .jointMat = skin_buffer
        })
    });*/
    
    offscreen_pass.pip = ubershader_pip;
    offscreen_pass.bind = ubershader_bindings;

}
//I want the animation preview panel to appear 
// as a popup in the corner of the grid?????
// I want a list of the available animations
// a button to play the animations
// and a preview of the time variables
// such as animation length and current animation time

void animation_preview_panel(){

    if(igBeginPopupModal("my_popup", NULL, ImGuiWindowFlags_None)){
        igText("Hello from popup");
        
      
        igEndPopup();
    }      
}
void material_editor(){
    
    
    // Remove padding so child is seamless
    igPushStyleVarImVec2(ImGuiStyleVar_WindowPadding, (ImVec2){0, 0});
    
    
    igBeginChild("Parameter_Window",(ImVec2){igGetContentRegionAvail().x*param_layout.panel_horiz,0},false,ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    {
        ImVec2 panel_avail = igGetContentRegionAvail();
        igBeginChild("Asset_Name",(ImVec2){0,panel_avail.y*param_layout.name_vert},false,ImGuiWindowFlags_None);
        {
            igText("Asset Name:");
            igText("Default Name");
        }
        igEndChild();

        igBeginChild("Texture Maps",(ImVec2){0,panel_avail.y*param_layout.tex_vert},false,ImGuiWindowFlags_None);
        {   
            //igPushStyleColorImVec4(ImGuiCol_Text, (ImVec4){0.78f, 0.66f, 0.43f, 1.0f});
          
            igSeparatorText("Environment Settings");
            //igPopStyleColor();
            ImVec2 image_avail = igGetContentRegionAvail();
            ImVec2 image_size = {0};
            image_size.x = image_avail.x * param_layout.tex_image_size.x;
            image_size.y = image_avail.y * param_layout.tex_image_size.y;
            igImage((ImTextureID)&object_data.albedo,image_size);

            //igImage(NULL,image_size);
            //igImage(NULL,image_size);
            //igImage(NULL,image_size);
            //igImage((ImTextureID)&object_textures.albedo,image_size);
            //igImage((ImTextureID)&object_textures.albedo,image_size);



        }
        igEndChild();

        igBeginChild("Properties",(ImVec2){.x = 0,.y = panel_avail.y*param_layout.properties_vert},false,ImGuiWindowFlags_None);
        {  
            igSeparatorText("Properties");
            igPushStyleVarImVec2(ImGuiStyleVar_FramePadding, (ImVec2){4.0f, 0.0f});
            igPushStyleVar(ImGuiStyleVar_GrabMinSize, 8.0f);
            igSliderFloat("Light X", &main_light.position.Elements[0], -20.0f, 20.0f);
            igSliderFloat("Light Y", &main_light.position.Elements[1], -20.0f, 20.0f);
            igSliderFloat("Light Z", &main_light.position.Elements[2], -20.0f, 20.0f);

            igSliderFloat("Camera Pos X", &offscreen_camera.position.x,-50.0f,50.0f);
            igSliderFloat("Camera Pos Y", &offscreen_camera.position.y,-50.0f,50.0f);
            igSliderFloat("Camera Pos Z", &offscreen_camera.position.z,-50.0f,50.0f);

            igPopStyleVarEx(2);
            float delta_time = app_get_delta_time();
            igText("Left Mouse Down: %d",igIsMouseDown(ImGuiMouseButton_Left));
            ImVec2 mouse_delta = {0.0f,0.0f};
            if(igIsMouseDown(ImGuiMouseButton_Left)){
                mouse_delta.x = io->MouseDelta.x;
                mouse_delta.y = io->MouseDelta.y;
            }
            //ImVec2 mouse_delta = igGetMouseDragDelta(ImGuiMouseButton_Left,-1.0f);
            igText("Mouse Delta: (%0.2f,%0.2f)",mouse_delta.x,mouse_delta.y);
            mouse_rotation.x += mouse_delta.x*delta_time*0.1f;
            mouse_rotation.y += mouse_delta.y*delta_time*0.1f;
            igText("mouse/time: (%0.2f,%0.2f)", mouse_rotation.x,mouse_rotation.y);
            igText("Delta Time: %0.5f",delta_time);
            
            bool too_small_x = fabsf(mouse_rotation.x - delta_time) < delta_time;
            bool too_small_y = fabsf(mouse_rotation.y - delta_time) < delta_time;
            if( mouse_rotation.x > 0){
                
                mouse_rotation.x -= (too_small_x) ? mouse_rotation.x : delta_time;
            }
            else if(mouse_rotation.x < 0 ){
                mouse_rotation.x += (too_small_x) ? mouse_rotation.x : delta_time;
            }

            if( mouse_rotation.y > 0){
                mouse_rotation.y -= (too_small_y) ? mouse_rotation.y : delta_time;
            }
            else if(mouse_rotation.y < 0){
                mouse_rotation.y += (too_small_y) ? mouse_rotation.y : delta_time;
            }
 
        }
        igEndChild();

        igBeginChild("Shader Preview",(ImVec2){.x = 0,.y = panel_avail.y * param_layout.shader_vert},false,ImGuiWindowFlags_None);
        {
            igSeparatorText("Shaders");
            ImVec2 shader_size = igGetContentRegionAvail();
            
            igPushStyleColorImVec4(ImGuiCol_Button, (ImVec4){0.0f, 0.0f, 0.0f, 0.0f});
            igPushStyleColorImVec4(ImGuiCol_ButtonHovered, (ImVec4){0.78f, 0.66f, 0.43f, 0.1f});
            igPushStyleColorImVec4(ImGuiCol_ButtonActive, (ImVec4){0.78f, 0.66f, 0.43f, 0.2f});
            igPushStyleColorImVec4(ImGuiCol_Text, (ImVec4){0.4f, 0.4f, 0.4f, 1.0f});
            
            int current_button_index = 0; 
            check_button_index(&current_button_index,&selected_button_index,"Custom Shader",shader_size);
            check_button_index(&current_button_index,&selected_button_index,"Lembertian Shading",shader_size);

            igPopStyleColorEx(4);
        }
        igEndChild();
    }
    igEndChild();
    igSameLine();
    ImVec2 available_space =  igGetContentRegionAvail();
    igPushStyleColorImVec4(ImGuiCol_ChildBg,(ImVec4){0.0f,0.0f,0.0f,1.0f});
    igBeginChild(
        "Grid Window",
        available_space,   // fill remaining space
        false,            // no border
        ImGuiWindowFlags_None
    );

    //we want to draw a grid of lines here and then draw the rt on top of it with a semitransparent alpha

    ImDrawList* draw_list = igGetWindowDrawList();
    ImVec2 grid_space = igGetContentRegionAvail();
    ImVec2 cursor_pos = igGetCursorScreenPos();

    float dominant_region = (grid_space.x > grid_space.y) ? grid_space.x : grid_space.y;

    float min_y = cursor_pos.y;
    float max_y = cursor_pos.y + dominant_region;

    float min_x = cursor_pos.x;
    float max_x = cursor_pos.x + dominant_region;
    //we need a point from an x position from the lowest y to the highest y
    int num_lines = 30;
    float x_step = dominant_region / num_lines;
    float y_step = dominant_region / num_lines;
    for(int x = 0; x < num_lines; x++){
        
        float x_pos = cursor_pos.x + (x_step * x);
        
        ImDrawList_AddLine(draw_list,(ImVec2){.x = x_pos , .y = min_y},(ImVec2){.x = x_pos , .y = max_y},IM_COL32(200, 200, 200, 50));
    }
    for(int y = 0; y < num_lines; y++){
        float y_pos = cursor_pos.y + (y_step * y);
            
        ImDrawList_AddLine(draw_list,(ImVec2){.x = min_x , .y = y_pos},(ImVec2){.x = max_x , .y = y_pos},IM_COL32(200, 200, 200, 50));
    }

    //(ImVec2){(float)offscreen_pass.color_target.tex.width,(float)offscreen_pass.color_target.tex.height}
    if(available_space.x != offscreen_pass.color_target.tex->width || available_space.y != offscreen_pass.color_target.tex->height){
        pass_resize.changed_resolution = true;
        pass_resize.new_width = (int)available_space.x;
        pass_resize.new_height = (int)available_space.y;
    }
   
    igImage((ImTextureID)offscreen_pass.color_target.tex,available_space);
    animation_preview_panel();
    igEndChild();
    igPopStyleColor();
    igPopStyleVarEx(1);

    //code to check if mouse is inside the grid

    bool is_inside = is_mouse_inside_widget(cursor_pos,grid_space);
    
    if(is_inside)
    {
        ImVec2 mouse_delta = igGetMouseDragDelta(ImGuiMouseButton_Left,-1.0f);
        
    }

}

void frame(){
    app_get_cursor_pos(&slimgui_input_state.mouse_x,&slimgui_input_state.mouse_y);
    slg_begin_frame();


    //probably do my offscreen rendering pass here
    light_sources light_data = {0};
    light_data.num_lights      = 1;
    light_data.lights[0] = main_light;
    slg_update_buffer(light_buffer,&light_data,sizeof(light_data));

    ImVec2 norm_mouse_rot = {0,0};
    float mag = sqrtf(mouse_rotation.x * mouse_rotation.x + mouse_rotation.y * mouse_rotation.y);
    if(mag > 0.0f){
        norm_mouse_rot.x = mouse_rotation.x / mag;
        norm_mouse_rot.y = mouse_rotation.y / mag;
    }
    float delta_time = app_get_delta_time();
    float angle_x = norm_mouse_rot.y * mag * delta_time;
    float angle_y = norm_mouse_rot.x * mag * delta_time;

    Mat4 rot_x = rotateMat4Version2(angle_x,(Vector3){1.0f,0.0f,0.0f});
    Mat4 rot_y = rotateMat4Version2(angle_y,(Vector3){0.0f,1.0f,0.0f});
    //Transform_Matrices.model_mat = rotateMat4Version2(,);
    Transform_Matrices.model_mat = mulMat4(rot_x,Transform_Matrices.model_mat);
    Transform_Matrices.model_mat = mulMat4(rot_y,Transform_Matrices.model_mat);

    Transform_Matrices.normal_mat = inverseMat4(transposeMat4(Transform_Matrices.model_mat));
    if(pass_resize.changed_resolution){
        float new_aspect = (float)pass_resize.new_width/(float)pass_resize.new_height;
        Vector3 up = {0.0f,1.0f,0.0f};
        offscreen_camera.view = LookAt_RH_Version2(offscreen_camera.position,offscreen_camera.target,up);
        offscreen_camera.projection = perspectiveMat4_Z0(1.0472f,new_aspect,offscreen_camera.near_plane,offscreen_camera.far_plane);
        Transform_Matrices.mvp_mat = mulMat4(mulMat4(offscreen_camera.projection,offscreen_camera.view),Transform_Matrices.model_mat);
        slg_update_buffer(transform_buffer,(void*)&Transform_Matrices,sizeof(Transform_Matrices));
        slg_update_render_texture(&offscreen_pass.color_target,(UINT)pass_resize.new_width,(UINT)pass_resize.new_height);
        slg_update_depth_texture(&offscreen_pass.depth_target,(UINT)pass_resize.new_width,(UINT)pass_resize.new_height);
        offscreen_pass.initialized = false;
        pass_resize.changed_resolution = false;
    }
    else{
        Vector3 up = {0.0f,1.0f,0.0f};
        offscreen_camera.view = LookAt_RH_Version2(offscreen_camera.position,offscreen_camera.target,up);
        Transform_Matrices.mvp_mat = mulMat4(mulMat4(offscreen_camera.projection,offscreen_camera.view),Transform_Matrices.model_mat);
        slg_update_buffer(transform_buffer,(void*)&Transform_Matrices,sizeof(Transform_Matrices));
    }
    slg_begin_offscreen_pass(&offscreen_pass);
    slg_set_pipeline(&offscreen_pass.pip);
    slg_set_bindings(&offscreen_pass.bind);
    slg_draw((unsigned int)offscreen_pass.bind.data_ptr->index_buffer.size_in_bytes/offscreen_pass.bind.data_ptr->index_buffer.stride,1,0,0,0);
    slg_end_offscreen_pass(&offscreen_pass);
    
    //I think it would look something like this
    //then I could take the resulting output render texture
    // and give it to the imgui code to draw inside the gui
    slg_begin_pass();
    slimgui_frame();

    //ImVec2 pos = { 10.0f, 10.0f };
    //igSetNextWindowPos(pos, ImGuiCond_Once);
    igBeginMainMenuBar();
    if(igBeginMenu("File")){
        if(igMenuItem("Open")){
            char path[MAX_PATH];
            app_open_file_dialog(path,MAX_PATH);
            //implement more behavior once i decide on my file format
        }
        if(igMenuItem("Import")){
            char path[MAX_PATH];
            app_open_file_dialog(path,MAX_PATH);
            load_gltf(path,MAX_PATH);
            igOpenPopup("my_popup", 0); 
            
        }
        if(igMenuItem("Save")){
            //do nothing yet
        }
        if(igMenuItem("Export")){
            //do nothing yet
        }
        igEndMenu();
        
    }
    if(igBeginMenu("Create")){
        if(igMenuItem("New Object")){

        }
        igEndMenu();
    }
    igEndMainMenuBar();
    //igSetNextWindowPos((ImVec2){0, menu_bar_height}, ImGuiCond_Always, (ImVec2){0,0});
    //igSetNextWindowSize((ImVec2){io->DisplaySize.x, io->DisplaySize.y - menu_bar_height}, ImGuiCond_Always);
    ImGuiViewport* vp = igGetMainViewport();
    igSetNextWindowPos(vp->WorkPos,ImGuiCond_Always);
    igSetNextWindowSize(vp->WorkSize,ImGuiCond_Always);
    igBegin("Main", NULL,
        ImGuiWindowFlags_NoCollapse  |
        ImGuiWindowFlags_NoTitleBar  |
        ImGuiWindowFlags_NoResize    |
        ImGuiWindowFlags_NoMove
    );


    material_editor();
    //igShowDemoWindow(NULL);
    igEnd();

    slimgui_end_frame();
    slg_submit_draw();
}
void event(app_event_t* event){
    switch(event->event_code){
        case(APP_EVENT_MOUSE_LEFT_BUTTON_DOWN):{
            slimgui_input_state.left_mouse_down = true;
        }
        break;
        case(APP_EVENT_MOUSE_LEFT_BUTTON_UP):{
            slimgui_input_state.left_mouse_down = false;
        }
        break;
        case(APP_EVENT_MOUSE_RIGHT_BUTTON_DOWN):{
            slimgui_input_state.right_mouse_down = true;
        }
        break;
        case(APP_EVENT_MOUSE_RIGHT_BUTTON_UP):{
            slimgui_input_state.right_mouse_down = false;
        }
        break;
        case(APP_EVENT_SCROLL_WHEEL):{
            slimgui_input_state.mouse_scroll_delta = event->mouse_wheel_delta;
            event->mouse_wheel_delta = 0;
        }
        case(APP_EVENT_MOUSE_LEAVE):{
            slimgui_input_state.left_mouse_down = false;
            slimgui_input_state.right_mouse_down = false;
        }

    }
}
void cleanup(){
    slg_cleanup();
}
int main(){
    app_init((app_desc_t){
        .frame_func = frame,
        .init_func = init,
        .cleanup_func = cleanup,
        .event_func = event,
        .width = APP_WIDTH,
        .height = APP_HEIGHT
    });
    app_mainline();

}


