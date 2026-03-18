struct VSInput
{
    float2 position   : POSITION;
    float2 uv    : TEXCOORD0;
    float4 color   : COLOR;
};

struct PSInput
{
    float4 position   : SV_POSITION;
    float2 uv    : TEXCOORD0;
    float4 color   : COLOR;
};

cbuffer MVPConstantBuffer : register(b0) { 
    column_major float4x4 mvpMatrix; 
};
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(VSInput input)
{
    PSInput result;

    //float clip_x = (input.position.x / 800.0f) * 2.0f - 1.0f;
    //float clip_y = 1.0f - (input.position.y / 600.0f) * 2.0f;
    result.position = mul(mvpMatrix,float4(input.position,0.0f,1));
    //result.position = float4(clip_x,clip_y,0,1);
    //result.position = float4(input.position,0,1);
    result.color = input.color;
    result.uv = input.uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{   

    return g_texture.Sample(g_sampler,input.uv)*input.color; 
   // input.color;
}