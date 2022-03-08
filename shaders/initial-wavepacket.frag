#version 330 core

precision highp float;
#if __VERSION__ >= 300
#define texture2D texture
in vec2 fragTexCoord;
out vec4 fragColor;
#else
#define fragColor gl_FragColor
varying highp vec2 fragTexCoord;
#endif
uniform float dx;
uniform float dy;
uniform float bx;
uniform float by;
uniform float px;
uniform float py;
uniform float sx;
uniform float sy;
uniform float amp;
uniform float borderAlpha;
float sqrt2 = 1.4142135623730951; 
float sqrtpi = 1.7724538509055159;
float pi = 3.141592653589793;


void main () {
    if (fragTexCoord.x > dx && fragTexCoord.x < 1.0-dx &&
        fragTexCoord.y > dy && fragTexCoord.y < 1.0-dy) {
        float x = fragTexCoord.x;
        float y = fragTexCoord.y;
        float u = ((x - bx)/(sx*sqrt2));
        float v = ((y - by)/(sy*sqrt2));
        float re = amp*exp(- u*u - v*v)*cos(2.0*pi*(px*x + py*y));
        float im = amp*exp(- u*u - v*v)*sin(2.0*pi*(px*x + py*y));
        // float re = amp/cosh(- u*u - v*v)*cos(2.0*pi*(px*x + py*y));
        // float im = amp/cosh(- u*u - v*v)*sin(2.0*pi*(px*x + py*y));
        fragColor = vec4(re, im, 0.0, 1.0); 
    } else {
        fragColor = vec4(0.0, 0.0, 0.0, borderAlpha); 
    }
}
