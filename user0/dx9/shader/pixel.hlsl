// The pixel shader input struct must be the same as the vertex shader output!
struct PS_INPUT {
    float4 position : POSITION;
    float4 color : COLOR;
};
// The pixel shader simply returns a 4d vector representing the vertex color.
// It receives it's input as a parameter just like the vertex shader.
// We have to declare the output semantic as color to it gets interpreted
// correctly.
float4 main(PS_INPUT input) : COLOR {
    return input.color;
}
