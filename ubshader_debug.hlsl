#define MAX_LIGHT_COUNT 16


struct VSInput
{
    float3 position   : POSITION;
    float3 normal : TEXCOORD0;
    float2 uv    : TEXCOORD1;
    float4 joint_indices : TEXCOORD2;
    float4 joint_weights : TEXCOORD3;
};

struct PSInput
{
    float4 position   : SV_POSITION;
    float3 normal    : TEXCOORD0;
    float2 uv        : TEXCOORD1;
    float3 world_pos : TEXCOORD2;  // needed for lighting
};
struct Light{
    float3 position; // 12 bytes
    float  intensity; // 4 bytes
    float3 color; // 12 bytes
    float  radius; // 4 bytes
};

cbuffer TransformBuffer : register(b0) { 
    column_major float4x4 mvpMatrix;
    column_major float4x4 modelMatrix;
    column_major float4x4 normalMatrix; 
};

cbuffer LightPositions : register(b1) {
    Light lights[MAX_LIGHT_COUNT]; //32 * light count
    int num_lights;  //4 bytes
    float3 aligned_buffer; //12 bytes to align buffer
};
cbuffer jointMat : register(b2){
    column_major float4x4 joint_mat[63]; //64 is maximum joints
};

cbuffer FeatureFlags : register(b3){
    int has_skinning;
    int has_tangents;
    int has_albedo;
    int has_normal_map;
    int has_emissive;
    int has_specular;
    int has_metallic_roughness;
    int _pad; // pad to 32 bytes
};
Texture2D albedo : register(t0);
Texture2D normal_map : register(t1);
Texture2D tangent_map : register(t2);
Texture2D metallic_roughness : register(t3);
Texture2D emissive : register(t4);
Texture2D occlusion : register(t5);

SamplerState g_sampler : register(s0);


void calc_skin_pos(in float3 pos, in float4 joint_weight, in float4 joint_index, out float3 skin_pos){
    float4x4 skin_mat = (float4x4)0;
    if(joint_weight.x>0){
        skin_mat += joint_weight.x*joint_mat[int(joint_index.x)];
    }
    if(joint_weight.y>0){
        skin_mat += joint_weight.y*joint_mat[int(joint_index.y)];
    }
    if(joint_weight.z>0){
        skin_mat += joint_weight.z*joint_mat[int(joint_index.z)];
    }
    if(joint_weight.w>0){
        skin_mat += joint_weight.w*joint_mat[int(joint_index.w)];
    }
    if(dot(joint_weight,float4(1,1,1,1))==0){
        skin_mat = float4x4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );//if there is no joint weighting. The skinning mat should be identity matrix
    }
    skin_pos = mul(skin_mat,float4(pos,1.0f)).xyz;
}

PSInput VSMain(VSInput input)
{
    PSInput result;
    if(has_skinning){
        float3 skin_pos;
        calc_skin_pos(input.position,input.joint_weights,input.joint_indices,skin_pos);
        result.position  = mul(mvpMatrix, float4(skin_pos, 1.0f));
        result.world_pos = mul(modelMatrix, float4(skin_pos, 1.0f)).xyz;
    }
    else{
        result.position  = mul(mvpMatrix, float4(input.position, 1.0f));
        result.world_pos = mul(modelMatrix, float4(input.position, 1.0f)).xyz;
    }
    float3 skin_pos;
    //calc_skin_pos(input.position,input.joint_weights,input.joint_indices,skin_pos);
    //result.position  = mul(mvpMatrix, float4(skin_pos, 1.0f));
    //result.world_pos = mul(modelMatrix, float4(skin_pos, 1.0f)).xyz;
    //result.position  = mul(mvpMatrix, float4(input.position, 1.0f));
    //result.world_pos = mul(modelMatrix, float4(input.position, 1.0f)).xyz;
    result.normal = mul((float3x3)normalMatrix,input.normal);
    result.uv = input.uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{   

    float3 normal = input.normal; //Normal value for the pixel
    //float3 position = input.position.xyz; //Postion value for the pixel
    float4 color = albedo.Sample(g_sampler,input.uv);
    //this is going to be used sum up the effect of all the lights for each light ON THE PIXEL
    //this is a color, so it will be effected by specular color, diffuse color, light color, etc
    float3 total_lighting = float3(0,0,0); 
    
    //In here we calculate lighting value
    for(int i = 0;i<num_lights;i++){
        float3 light_dir = normalize(lights[i].position - input.world_pos); //get the position of the light relative to the point we are operating on
        // LAMBERTIAN LIGHTING MODEL
        float  diffuse   = max(dot(normal, light_dir), 0.0f); 
        float  dist      = length(lights[i].position - input.world_pos); //don't have world pos yet
        float  atten     = 1.0f / (1.0f + dist / lights[i].radius); //

        //total_lighting += (lights[i].color * diffuse + specular) * lights[i].intensity * atten;
        //specular not implemented yet
        total_lighting += (lights[i].color * diffuse) * lights[i].intensity * atten;
        //total_lighting = light_dir;
    }
    
    //return (color * float4(total_lighting,1.0f))*0.01f + float4(1.0f,0.0f,0.0f,1.0f) ; //+ float4(normal.xyz,1.0f);
    return (color * float4(total_lighting,1.0f));
     
    
    //return color;
    
}