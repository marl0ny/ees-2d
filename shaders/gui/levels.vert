#if __VERSION__ <= 120
attribute vec4 position;
varying vec2 UV;
#else
in vec4 position;
out vec2 UV;
#endif

#if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
#define texture2D texture
#else
#define texture texture2D
#endif

#if (__VERSION__ > 120) || defined(GL_ES)
precision highp float;
#endif

uniform sampler2D vertTex; // It's green!
uniform float expectationValue;
uniform float minLevel;
uniform float maxLevel;

void main() {
    UV = position.xy;
    float level = texture2D(vertTex, vec2(UV[1], 0.5))[0];
    float yPos = 0.5;
    if (position[3] < 1.0)
        yPos = (level - minLevel)/(maxLevel - minLevel);
    else
        yPos = (expectationValue - minLevel)
                / (maxLevel - minLevel);
    vec2 r = 0.9*vec2(UV[0], yPos) + 0.05;
    // vec2 r = vec2(UV[0], UV[1]);
    gl_Position = vec4(2.0*(r - vec2(0.5)), 0.0, 1.0);
}