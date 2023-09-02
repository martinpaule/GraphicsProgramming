// Tessellation Hull Shader
// Prepares control points for tessellation
struct InputType
{
    float4 position : POSITION;
    float3 normal : NORMAL;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float4 position : POSITION;
    float3 normal : NORMAL;
};

cbuffer EdgesValues : register(b1)
{
    float3 camera_position;
    float tess_factor;
};

//tesselation function based on distance

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;
    
    for (int i = 0; i < 4; i++)
    {

        //calculate each edge's tesselation individually based on the distance there' - smoother because adjacent edges will have same/similar tess factors
        float lengthDist = length(camera_position - (inputPatch[(i + 1)%4].position + inputPatch[i].position) * 0.5f);
        float tessellationFact = 64.0f / (lengthDist + 1.0f);
        output.edges[i] = tessellationFact;
   
    }
    
    //centre will have the same tesselation factor
    float3 centrePosition = (inputPatch[0].position + inputPatch[1].position + inputPatch[2].position + inputPatch[3].position) / 4.0f;
    float distance = length(camera_position - centrePosition);
    float centreTess = 64.0f / (distance + 1.0f);
    output.inside[0] = centreTess;
    output.inside[1] = centreTess;

    
    return output;
}


[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

  
    
    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;
    // Set the input colour as the output colour.
    output.normal = patch[pointId].normal;
    
    //output.tex = patch[pointId].tex;
   
    return output;
}
