// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
Texture2D texture0 : register(t1);
SamplerState sampler0 : register(s1);



cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;

};


struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
    
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPos : TEXCOORD1;

};

[domain("tri")]
OutputType main(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    OutputType output;

    // Determine the position of the new vertex.
    // Invert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
    // Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).

    //calculate the new vertex position
    float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);    
    vertexPosition = lerp(v1, v2, uvwCoord.x);
    
    //calculate its texture coordinate
    float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    output.tex = lerp(t1, t2, uvwCoord.x);

    //get the pixel there's colour
    float textureColour = texture0.SampleLevel(sampler0, output.tex, 0) * 4;
    
    //raise the vertexz by how white the colour is
    vertexPosition.y += textureColour;
    
    // Calculate the position of the vertice as viewed by the light source.
    output.lightViewPos = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);

    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    
    return output;
}

