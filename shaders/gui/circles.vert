#if __VERSION__ <= 120
attribute vec2 position;
varying vec2 UV;
#else
in vec2 position;
out vec2 UV;
#endif

// #if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
// #define texture2D texture
// #else
// #define texture texture2D
// #endif

#if (__VERSION__ > 120) || defined(GL_ES)
precision highp float;
#endif


void main() {
    UV = position;
    gl_Position = vec4(2.0*(position - vec2(0.5)), 0.0, 1.0);
}
