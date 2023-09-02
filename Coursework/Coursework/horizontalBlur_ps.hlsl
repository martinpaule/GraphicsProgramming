Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float2 screenSize;
    float snorkelOn;
    float blurStrength;
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
    float texelSize = 1.0f / screenSize.x;


    //universal blur weight for all. has to add up to 1
    float weight_ = 1.0 / (blurStrength * 2 + 1);
    
        

    for (int i = -1 * blurStrength; i <= blurStrength; i++)
    {
        
        //add all the neighbpouring (affected by direction) pixel's colours
        colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * i, 0)) * weight_;

    }

    //if snorkel is on
    if (snorkelOn)
    {
        //left result refers to radius. circle equation is radius^2 = (x- midX)^2 + (y- midY)^2
        float circEqLeftResult = (input.tex.x - 0.5) * (input.tex.x - 0.5) + (input.tex.y - 0.5) * (input.tex.y - 0.5);

        if (circEqLeftResult < 0.11)//if pixel in question's distance from centre is smaller than 0.11^0.5
        {
            colour = float4(0.0f, 0.0f, 0.0f, 0.0f);//colour it black (snorkel outline)

            if (circEqLeftResult < 0.09)//unless it's smaller than 0.09 (inside snorkel glass), in which case just apply normal (clear) colour
            {
                colour = shaderTexture.Sample(SampleType, input.tex);
            }
        }
    }


    saturate(colour);
	// Set the alpha channel to one.
    colour.a = 1.0f;
    
    return colour;
}
