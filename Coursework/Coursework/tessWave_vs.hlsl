
struct InputType
{
    float4 position : POSITION;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : POSITION;
    float3 normal : NORMAL;
};

OutputType main(InputType input)
{
    OutputType output;

    // Pass the vertex position into the hull shader.
    output.position = input.position;

    // Pass the input color into the hull shader.
    output.normal = input.normal;
    return output;
}
