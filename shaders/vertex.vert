#version 330 core

#if __VERSION__ >= 300
in vec3 pos;
out highp vec2 fragTexCoord;
#else
attribute vec3 position;
varying highp vec2 fragTexCoord;
#endif

void main() {
    gl_Position = vec4(position.xyz, 1.0);
    fragTexCoord = vec2(0.5, 0.5) + position.xy/2.0;
}