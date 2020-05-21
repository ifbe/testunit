float4x4 projectionMatrix;
float4x4 viewMatrix;
float4x4 worldMatrix;
// Add the texture coordinates to the vertex shader in- and output.
struct VS_INPUT {
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    output.position = float4(input.position, 1.0f);
    output.position = mul(output.position, worldMatrix);    
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.color = input.color;
    // Set the texcoord output to the input.
    output.texcoord = input.texcoord;

    return output;
}
