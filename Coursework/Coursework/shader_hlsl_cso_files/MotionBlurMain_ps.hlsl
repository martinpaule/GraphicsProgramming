Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float screenWidth;
    float2 blurDirection;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float4 colour;
	// Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
   
    //calculate size of texel
    float texelSize = 1.0f / screenWidth;

 
    //blur strength depending on the mouse move vector from middle
    int blurStrength = length(blurDirection) * 2;
    

    //universal blur weight for all. has to add up to 1
    float weight_ = 1.0 / (blurStrength * 2 + 1);
    
    //if mouse got moved
    if (length(blurDirection) > 0)
    {

        //calculate stepX and Y. basically sets equation of blur direction
        float stepX, stepY;
        float2 dir__ = normalize(blurDirection);
        stepX = dir__.x;
        stepY = dir__.y;
        
        for (int i = -1 * blurStrength; i <= blurStrength; i++)
        {
            //add all the neighbpouring (affected by direction) pixel's colours
            colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * i*stepX*5, texelSize * i*stepY*5)) * weight_;

        }
    }
    else
    {//mouse didnt move so just render this pixel normally
        colour = shaderTexture.Sample(SampleType, input.tex);//saturate to not go over 1.0 in RGB
    }
    saturate(colour);
	// Set the alpha channel to one.
    colour.a = 1.0f;

    return colour;
}
