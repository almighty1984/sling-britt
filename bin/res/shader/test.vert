//#version 440
uniform vec2 light_pos;

void main()
{
    // transform the vertex position
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    // transform the texture coordinates
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    // forward the vertex color
    gl_FrontColor = gl_Color;
}

// attribute vec2 position;

// varying vec2 TextCoord;
// attribute vec2 textCoord;

// void main() {
//   gl_Position = vec4(position, 0.0, 1.0);
//   TextCoord = vec2(textCoord);
// }