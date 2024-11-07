Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D depthMapTexture2 : register(t2);

SamplerState diffuseSampler : register(s0);
SamplerState shadowSampler : register(s1);

// Define an array of lights in the LightBuffer with direction as float4
cbuffer LightBuffer : register(b0)
{
    float4 ambient[2];
    float4 diffuse[2];
    float4 direction[2];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPos : TEXCOORD1;
    float4 lightViewPos2 : TEXCOORD2;
};

float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    return saturate(diffuse * intensity);
}

bool hasDepthData(float2 uv)
{
    return uv.x >= 0.f && uv.x <= 1.f && uv.y >= 0.f && uv.y <= 1.f;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    float depthValue = sMap.Sample(shadowSampler, uv).r;
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w - bias;
    return lightDepthValue >= depthValue;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex = projTex * float2(0.5, -0.5) + float2(0.5f, 0.5f);
    return projTex;
}

float4 main(InputType input) : SV_TARGET
{
    float shadowMapBias = 0.005f;
    float4 colour = float4(0.f, 0.f, 0.f, 1.f);
    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);

    float2 pTexCoord = getProjectiveCoords(input.lightViewPos);
    float2 pTexCoord2 = getProjectiveCoords(input.lightViewPos2);

    if (hasDepthData(pTexCoord) && !isInShadow(depthMapTexture, pTexCoord, input.lightViewPos, shadowMapBias))
    {
        colour += calculateLighting(normalize(-direction[0].xyz), normalize(input.normal), diffuse[0]);
    }

    if (hasDepthData(pTexCoord2) && !isInShadow(depthMapTexture2, pTexCoord2, input.lightViewPos2, shadowMapBias))
    {
        colour += calculateLighting(normalize(-direction[1].xyz), normalize(input.normal), diffuse[1]);
    }

    return saturate(ambient[0] * colour + ambient[1] * colour) * textureColour;
}