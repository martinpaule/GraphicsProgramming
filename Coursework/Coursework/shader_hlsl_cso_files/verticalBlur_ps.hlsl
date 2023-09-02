Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float screenHeight;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;  
};

float4 main(InputType input) : SV_TARGET
{
    float weight0, weight1, weight2, weight3, weight4, weight5, weight6, weight7, weight8, weight9;
    float4 colour;

	// Create the weights that each neighbor pixel will contribute to the blur.
    weight0 = 0.1;
    weight1 = 0.05;
    weight2 = 0.05;
    weight3 = 0.05;
    weight4 = 0.05;
    weight5 = 0.05;
    weight6 = 0.05;
    weight7 = 0.05;
    weight8 = 0.05;
    weight9 = 0.05;

	// Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float texelSize = 1.0f / screenHeight;
    // Add the horizontal pixels to the colour by the specific weight of each.
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -9.0f)) * weight9;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -8.0f)) * weight8;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -7.0f)) * weight7;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -6.0f)) * weight6;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -5.0f)) * weight5;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -4.0f)) * weight4;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -3.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -2.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -1.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex) * weight0;    
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 1.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 2.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 3.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 4.0f)) * weight4;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 5.0f)) * weight5;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 6.0f)) * weight6;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 7.0f)) * weight7;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 8.0f)) * weight8;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 9.0f)) * weight9;
 
    
    float highest = colour.r;
    
    if (colour.g > highest)
    {
        highest = colour.g;
    }
    if (colour.b > highest)
    {
        highest = colour.b;
    }
    
    if (highest > 1.0f)
    {
        colour.r = colour.r / highest;
        colour.g = colour.g /highest;
        colour.b = colour.b /highest;
    }
    
    // Set the alpha channel to one.
    colour.a = 1.0f;
    
    return colour;
}

