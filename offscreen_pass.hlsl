#define MAX_LIGHT_COUNT 16


struct VSInput
{
    float3 position   : POSITION;
    float3 normal : TEXCOORD0;
    float2 uv    : TEXCOORD1;
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
    float3 direction;
    int light_type;
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

Texture2D albedo : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(VSInput input)
{
    PSInput result;

    result.position = mul(mvpMatrix,float4(input.position.xyz,1.0f));
    result.world_pos = mul(modelMatrix, float4(input.position, 1.0f)).xyz;
    result.normal = mul((float3x3)normalMatrix,input.normal);
    result.uv = input.uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{   

    float3 normal = input.normal; //Normal value for the pixel
    float3 position = input.position.xyz; //Postion value for the pixel
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
    
    return color * float4(total_lighting,1.0f); //+ float4(normal.xyz,1.0f);
    
     
    
    //return color;
    
}