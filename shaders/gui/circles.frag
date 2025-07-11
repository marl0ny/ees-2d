#if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
#define texture2D texture
#else
#define texture texture2D
#endif

#if (__VERSION__ > 120) || defined(GL_ES)
precision highp float;
#endif
    
#if __VERSION__ <= 120
varying vec2 UV;
#define fragColor gl_FragColor
#else
in vec2 UV;
out vec4 fragColor;
#endif


uniform vec4 color;
uniform ivec2 circlesGridSize;
uniform int inUseCount;

void main() {
    float horizontal = floor(UV.x*float(circlesGridSize[0]));
    float vertical = floor((1.0 - UV.y)*float(circlesGridSize[1]));
    if ((vertical*float(circlesGridSize[0]) + horizontal) 
        < (float(inUseCount)))
        fragColor = color;
}

