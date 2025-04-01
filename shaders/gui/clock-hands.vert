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

uniform sampler2D vertTex;

#define complex vec2

vec2 rotate(vec2 r, float angle) {
    float c = cos(angle), s = sin(angle);
    return vec2(c*r.x - s*r.y, s*r.x + c*r.y);
}

void main() {
    UV = position.xy;
    float maxRad = position[2];
    // vec2 center = 2.0*(UV - vec2(0.5));
    vec2 r;
    complex z = texture2D(vertTex, UV).xy;
    float angle = atan(z.y, z.x);
    if (position[3] < 1.0) {
        r = UV + rotate(maxRad*vec2(0.0, 0.05), angle);
    } else if (position[3] < 2.0) {
        r = UV + rotate(maxRad*vec2(0.0, -0.05), angle);
    } else if (position[3] < 3.0) {
        r = UV + rotate(maxRad*vec2(length(z), 0.05), angle);
    } else if (position[3] < 4.0) {
        r = UV + rotate(maxRad*vec2(length(z), -0.05), angle);
    }
    gl_Position = vec4(2.0*(r - vec2(0.5)), 0.0, 1.0);

}
