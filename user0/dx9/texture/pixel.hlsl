// Create  a sampler called "sam0" using sampler register 0, which is equal
// to the texture stage 0, to which we passed the texture.
sampler sam0 : register(s0);

struct PS_INPUT {
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

float4 main(PS_INPUT input) : COLOR{
    // Do a linear interpolation between the texture color and the input color
    // using 75% of the input color.
    // tex2D returns the texture data at the specified texture coordinate.
    return lerp(tex2D(sam0, input.texcoord), input.color, 0.75f);
}
