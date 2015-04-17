
const char* cc3D_Particle_vert = STRINGIFY(

varying vec2 TextureCoordOut;
varying vec4 ColorOut;
void main()
{
    ColorOut = a_color;
    TextureCoordOut = a_texCoord.xy;
    TextureCoordOut.y = 1.0 - TextureCoordOut.y;
    gl_Position = CC_PMatrix * a_position;
}

);