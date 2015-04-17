#define STRINGIFY(A)  #A
static const char* shadertestvsh = STRINGIFY(
                                          
void main()
{
    gl_Position = CC_MVPMatrix * a_position;
}

);
