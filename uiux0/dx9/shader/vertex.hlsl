// 3 4x4 float matrices representing the matrices we set in the fixed-function
// pipeline by using the SetTransform() method.
float4x4 projectionMatrix;
float4x4 viewMatrix;
float4x4 worldMatrix;
// The input struct to the vertex shader.
// It holds a 3d float vector for the position and a 4d float vector
// for the color.
struct VS_INPUT {
    float3 position : POSITION;
    float4 color : COLOR;
};
// The output struct of the vertex shader, that is passed to the pixel shader.
struct VS_OUTPUT {
    float4 position : POSITION;
    float4 color : COLOR;
};
// The main function of the vertex shader returns the output it sends to the
// pixel shader and receives it's input as a parameter.
VS_OUTPUT main(VS_INPUT input) {
    // Declare a empty struct, that the vertex shader returns.
    VS_OUTPUT output;
    // Set the output position to the input position and set
    // the w-component to 1, as the input position is a 3d vector and
    // the output position a 4d vector.
    output.position = float4(input.position, 1.0f);
    // Multiply the output position step by step with the world, view and
    // projection matrices.
    output.position = mul(output.position, worldMatrix);    
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    // Pass the input color unchanged to the pixel shader.
    output.color = input.color;
    // Return the output struct to the pixel shader.
    // The position value is automatically used as the vertex position.
    return output;
}
