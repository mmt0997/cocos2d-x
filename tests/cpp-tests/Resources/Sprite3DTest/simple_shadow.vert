varying vec2 v_texture_coord;
varying vec4 v_position;
uniform mat4 u_model_matrix;
void main(void)
{
    gl_Position =  CC_PMatrix  * CC_MVMatrix * a_position;
    v_texture_coord = a_texCoord.xy;
    v_texture_coord.y = (1.0 - v_texture_coord.y);
	v_position = u_model_matrix * a_position;
}
