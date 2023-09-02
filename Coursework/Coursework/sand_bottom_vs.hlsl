
struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;

};

OutputType main(InputType input)
{
    OutputType output;

    // Pass the vertex position into the hull shader.
    output.position = input.position;

    output.normal = input.normal;
    output.tex = input.tex;
    return output;
}
