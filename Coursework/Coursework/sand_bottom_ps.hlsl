Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);

SamplerState diffuseSampler  : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 ambient;
    float4 diffuse;
    float3 direction;
    float padding;
};

cbuffer LightBuffer : register(b1)
{

    float normDisp;
    float3 padding_;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPos : TEXCOORD1;

};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
    // Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

    // Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float3 calculateNormalFromTexture(float2 textureUV)
{
    float3 out_normal;
    float pixStepDist = 0.01f;

    //create distance vectors from centre 
    float3 upVector = float3(0.0f, 0.0f, -pixStepDist);
    float3 downVector = float3(0.0f, 0.0f, pixStepDist);
    float3 leftVector = float3(-pixStepDist, 0.0f, 0.0f);
    float3 rightVector = float3(pixStepDist, 0.0f, 0.0f);

    //height of the pixel in question
    float outPixelHeight = shaderTexture.Sample(diffuseSampler, textureUV).r;



    //calculate vectors of current pixel vs surrounding 4 pixels
    upVector.y = shaderTexture.Sample(diffuseSampler, textureUV + float2(0, -pixStepDist)).r - outPixelHeight;
    downVector.y = shaderTexture.Sample(diffuseSampler, textureUV + float2(0, pixStepDist)).r - outPixelHeight;
    leftVector.y = shaderTexture.Sample(diffuseSampler, textureUV + float2(-pixStepDist, 0)).r - outPixelHeight;
    rightVector.y = shaderTexture.Sample(diffuseSampler, textureUV + float2(pixStepDist, 0)).r - outPixelHeight;

    //calculate 4 cross products   
    //pattern to follow:
         //RxU
         //UxL
         //LxD
         //DxR


    //make their average for this pixel's normal
    out_normal = (cross(rightVector, upVector) + cross(upVector, leftVector) + cross(leftVector, downVector) + cross(downVector, rightVector)) / 4.0f;

    return normalize(out_normal);
}


float4 main(InputType input) : SV_TARGET
{

    if (normDisp) {
        float3 normal = calculateNormalFromTexture(input.tex);
        return float4(normal, 1.0f);
    }

    float shadowMapBias = 0.005f;
    float4 colour = float4(0.f, 0.f, 0.f, 1.f);
    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);
    textureColour += float4(0.3f, 0.3f, 0.0f, 0.0f);

    // Calculate the projected texture coordinates.
    float2 pTexCoord = getProjectiveCoords(input.lightViewPos);

    colour += ambient;

    // Shadow test. Is or isn't in shadow
    if (hasDepthData(pTexCoord))
    {
        // Has depth map data
        if (!isInShadow(depthMapTexture, pTexCoord, input.lightViewPos, shadowMapBias))
        {
            // is NOT in shadow, therefore light
            float3 normal = calculateNormalFromTexture(input.tex);
            colour += calculateLighting(-direction, normal, diffuse);
            
        }
        else {
            float4 black = float4(0.f, 0.f, 0.f, 1.f);
            return black;
        }
    }

    
    

    return saturate(colour) * textureColour;
}