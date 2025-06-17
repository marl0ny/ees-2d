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
varying vec3 NORMAL;
#define fragColor gl_FragColor
#else
in vec2 UV;
in vec3 NORMAL;
out vec4 fragColor;
#endif

#define complex vec2

uniform sampler2D tex;
uniform float brightness;
uniform vec3 color;

void main() {
    complex z = texture2D(tex, UV).xy;
    float ambient = 0.01;
    float actBrightness = brightness*length(z);
    vec3 actColor 
        = ambient + color*actBrightness;
    fragColor = vec4(actColor, 1.0);
}
