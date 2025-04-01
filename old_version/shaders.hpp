#ifndef _SHADERS_H_
#define _SHADERS_H_
const char *vertex_shader_source = R"(

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
})";
const char *prob_current_shader_source = R"(

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
uniform float w;
uniform float h;
uniform float hbar;
uniform float m;
uniform sampler2D tex;


float realValueAt(sampler2D texPsi, vec2 location) {
    vec4 tmp = texture2D(texPsi, location);
    return tmp.r*tmp.a;
}

float imagValueAt(sampler2D texPsi, vec2 location) {
    vec4 tmp = texture2D(texPsi, location);
    return tmp.g*tmp.a;
}

vec2 getDivRePsi(sampler2D texPsi) {
    float u = realValueAt(texPsi, fragTexCoord + vec2(0.0, dy/h));
    float d = realValueAt(texPsi, fragTexCoord + vec2(0.0, -dy/h));
    float l = realValueAt(texPsi, fragTexCoord + vec2(-dx/w, 0.0));
    float r = realValueAt(texPsi, fragTexCoord + vec2(dx/w, 0.0));
    return vec2(0.5*(r - l)/dx, 0.5*(u - d)/dy);
}

vec2 getDivImPsi(sampler2D texPsi) {
    float u = imagValueAt(texPsi, fragTexCoord + vec2(0.0, dy/h));
    float d = imagValueAt(texPsi, fragTexCoord + vec2(0.0, -dy/h));
    float l = imagValueAt(texPsi, fragTexCoord + vec2(-dx/w, 0.0));
    float r = imagValueAt(texPsi, fragTexCoord + vec2(dx/w, 0.0));
    return vec2(0.5*(r - l)/dx, 0.5*(u - d)/dy);
}

void main() {
    float rePsi = texture2D(tex, fragTexCoord).r;
    float imPsi = texture2D(tex, fragTexCoord).g;
    vec2 divRePsi = getDivRePsi(tex);
    vec2 divImPsi = getDivImPsi(tex);
    vec2 probCurrent = (hbar/m)*(-imPsi*divRePsi + rePsi*divImPsi);
    fragColor = vec4(probCurrent.x, probCurrent.y, 0.0, 1.0);
}
)";
const char *initial_potential_shader_source = R"(precision highp float;
#if __VERSION__ == 300
#define texture2D texture
in vec2 fragTexCoord;
out vec4 fragColor;
#else
#define fragColor gl_FragColor
varying highp vec2 fragTexCoord;
#endif
uniform int potentialType;
uniform int dissipativePotentialType;

// Controls size of potential
uniform float a;

// For the double slit
uniform float y0;
uniform float w;
uniform float spacing;
uniform float x1;
uniform float x2;

uniform float aImag;

#define SHO 1
#define DOUBLE_SLIT 2
#define SINGLE_SLIT 3
#define STEP 4
#define INV_R 5
#define TRIPLE_SLIT 6
#define NEG_INV_R 7
#define CIRCLE 8

#define NO_DISSIPATION 0
#define BOUNDARY_DISSIPATION 1
#define UNIFORM_DISSIPATION 2


void main() {
    float x = fragTexCoord.x;
    float y = fragTexCoord.y;
    float imagVal = 0.0;
    if (dissipativePotentialType == BOUNDARY_DISSIPATION) {
        imagVal -= 30.0*exp(-0.5*y*y/(0.01*0.01));
        imagVal -= 30.0*exp(-0.5*(y-1.0)*(y-1.0)/(0.01*0.01));
        imagVal -= 30.0*exp(-0.5*x*x/(0.01*0.01));
        imagVal -= 30.0*exp(-0.5*(x-1.0)*(x-1.0)/(0.01*0.01));
    } else if (dissipativePotentialType == UNIFORM_DISSIPATION) {
        imagVal = -10.0;
    }
    if (potentialType == SHO) {
        fragColor = vec4(a*((x-0.5)*(x-0.5) + (y-0.5)*(y-0.5)), 0.0,
                         imagVal, 1.0); 
    } else if (potentialType == DOUBLE_SLIT) {
        if (y <= (y0 + w/2.0) &&
            y >= (y0 - w/2.0) &&
            (x <= x1 - spacing/2.0 ||
             (x >= x1 + spacing/2.0 &&
              x <= x2 - spacing/2.0
             ) || x >= x2 + spacing/2.0
            )) {
            fragColor = vec4(a, 0.0, imagVal, 1.0); 
        } else {
            fragColor = vec4(0.0, 0.0, imagVal, 1.0); 
        }
    } else if (potentialType == SINGLE_SLIT) {
         if (y <= (y0 + w/2.0) &&
            y >= (y0 - w/2.0) &&
            (x <= x1 - spacing/2.0 ||
             x >= x1 + spacing/2.0)) {
            fragColor = vec4(a, 0.0, imagVal, 1.0); 
        } else {
            fragColor = vec4(0.0, 0.0, imagVal, 1.0); 
        }
    } else if (potentialType == STEP) {
        if (y > y0) {
            fragColor = vec4(a, 0.0, imagVal, 1.0);
        } else {
            fragColor = vec4(0.0, 0.0, imagVal, 1.0);
        }
    } else if (potentialType == INV_R) {
        float u = 10.0*(x - 0.5);        
        float v = 10.0*(y - 0.5);
        float oneOverR = 1.0/sqrt(u*u + v*v);
        float val = (oneOverR < 50.0)? oneOverR: 50.0;
        fragColor = vec4(val, 0.0, imagVal, 1.0); 
    } else if (potentialType == TRIPLE_SLIT) {
        float val = 15.0;   
        if ((y <= 0.45 || y >= 0.48) || (x > 0.49 && x < 0.51)
            || (x > 0.43 && x < 0.45) || (x > 0.55 && x < 0.57)) {
            fragColor = vec4(0.0, 0.0, imagVal, 1.0);
        } else {
            fragColor = vec4(val, 0.0, imagVal, 1.0);
        }
    } else if (potentialType == NEG_INV_R) {
        float u = 2.0*(x - 0.5);        
        float v = 2.0*(y - 0.5);
        float oneOverR = -1.0/sqrt(u*u + v*v);
        float val = (oneOverR < -150.0)? -150.0: oneOverR;
        fragColor = vec4(val + 50.0, 0.0, imagVal, 1.0);
    } else if (potentialType == CIRCLE) {
        float u = x - 0.5;
        float v = y - 0.5;
        float r = sqrt(u*u + v*v);
        float val = 25.0*smoothstep(0.24, 0.26, r);
        fragColor = vec4(val, 0.0, imagVal, 1.0);
    } else {
        fragColor = vec4(0.0, 0.0, imagVal, 1.0); 
    }
}
)";
const char *initial_wavepacket_shader_source = R"(

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
)";
const char *real_imag_timestep_shader_source = R"(

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
uniform float dt;
uniform float w;
uniform float h;
uniform float m;
uniform float hbar;
uniform float rScaleV;
uniform sampler2D texPsi1;
uniform sampler2D texPsi2;
uniform sampler2D texV;
uniform int laplacePoints;


vec2 valueAt(sampler2D texPsi, vec2 coord) {
    vec4 psiFragment = texture2D(texPsi, coord);
    return psiFragment.xy*psiFragment.a;
}


vec2 div2Psi(sampler2D texPsi) {
    vec2 c = valueAt(texPsi, fragTexCoord);
    vec2 u = valueAt(texPsi, fragTexCoord + vec2(0.0, dy/h));
    vec2 d = valueAt(texPsi, fragTexCoord + vec2(0.0, -dy/h));
    vec2 l = valueAt(texPsi, fragTexCoord + vec2(-dx/w, 0.0));
    vec2 r = valueAt(texPsi, fragTexCoord + vec2(dx/w, 0.0));
    // Reference for different Laplacian stencil choices:
    // Wikipedia contributors. (2021, February 17)
    // Discrete Laplacian Operator 
    // 1.5.1 Implementation via operator discretization
    // https://en.wikipedia.org/wiki/Discrete_Laplace_operator
    // #Implementation_via_operator_discretization
    if (laplacePoints <= 5) {
        return (u + d + l + r - 4.0*c)/(dx*dx);
    } else {
        vec2 ul = valueAt(texPsi, fragTexCoord + vec2(-dx/w, dy/h));
        vec2 ur = valueAt(texPsi, fragTexCoord + vec2(dx/w, dy/h));
        vec2 dl = valueAt(texPsi, fragTexCoord + vec2(-dx/w, -dy/h));
        vec2 dr = valueAt(texPsi, fragTexCoord + vec2(dx/w, -dy/h));
        return (0.25*ur + 0.5*u + 0.25*ul + 0.5*l + 
                0.25*dl + 0.5*d + 0.25*dr + 0.5*r - 3.0*c)/(dx*dx);
    }
}


void main() {
    vec4 arrV = texture2D(texV, fragTexCoord);
    float V = (1.0 - rScaleV)*arrV[0] + rScaleV*arrV[1];
    float imV = arrV[2];
    float f1 = 1.0 - dt*imV/hbar;
    float f2 = 1.0 + dt*imV/hbar;
    vec4 psi1Fragment = texture2D(texPsi1, fragTexCoord);
    float alpha = psi1Fragment.a;
    vec2 psi1 = psi1Fragment.xy*alpha;
    vec2 psi2 = valueAt(texPsi2, fragTexCoord);
    vec2 hamiltonianPsi2 = -(0.5*hbar*hbar/m)*div2Psi(texPsi2) + V*psi2;
    fragColor = vec4(psi1.x*(f2/f1) + dt*hamiltonianPsi2.y/(f1*hbar),
                     psi1.y*(f2/f1) - dt*hamiltonianPsi2.x/(f1*hbar),
                     0.0, alpha);
}
)";
const char *copy_scale_shader_source = R"(

precision highp float;
#if __VERSION__ >= 300
#define texture2D texture
in vec2 fragTexCoord;
out vec4 fragColor;
#else
#define fragColor gl_FragColor
varying highp vec2 fragTexCoord;
#endif
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float scale1;
uniform float scale2;

void main () {
    vec4 col1 = texture2D(tex1, fragTexCoord);
    vec4 col2 = texture2D(tex2, fragTexCoord);
    vec3 col = scale1*col1.rgb + scale2*col2.rgb;
    fragColor = vec4(col, col1.a);
}
)";
const char *view_frame_shader_source = R"(

#define NAME viewFrameFragmentSource
precision highp float;
#if __VERSION__ >= 300
#define texture2D texture
in vec2 fragTexCoord;
out vec4 fragColor;
#else
#define fragColor gl_FragColor
varying highp vec2 fragTexCoord;
#endif
uniform float x0;
uniform float y0;
uniform float w;
uniform float h;
uniform float lineWidth;
uniform float brightness;
uniform float brightness2;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D texV;
uniform sampler2D vecTex;
uniform sampler2D textTex;
uniform sampler2D backgroundTex;
uniform int displayMode;
uniform int wavefunctionDisplayMode;
uniform int potentialDisplayMode;
uniform int vectorDisplayMode;
uniform int backgroundDisplayMode;
uniform vec3 probColour;
uniform vec3 potColour;

const float pi = 3.141592653589793;

#define DISPLAY_ONLY_PROB_DENSITY 0
#define DISPLAY_PHASE 1
#define DISPLAY_PROB_DENSITY_HEIGHT_MAP 2

#define DISPLAY_POTENTIAL_SINGLE_COLOUR 0
#define DISPLAY_POTENTIAL_COLOUR_MAP 1
#define DISPLAY_POTENTIAL_COLOUR 2

#define DISPLAY_NO_VECTOR 0
#define DISPLAY_VECTOR 1

#define DISPLAY_NO_BACKGROUND 0
#define DISPLAY_BACKGROUND 1


vec4 drawWindow(vec4 pix, float x, float y,
                float x0, float y0, float w, float h,
                float lineWidth) {
    y0 = (h < 0.0)? y0 + h: y0;
    h = (h < 0.0)? -h: h;
    x0 = (w < 0.0)? x0 + w: x0;
    w = (w < 0.0)? -w: w;
    if ((x >= x0 && x <= (x0 + w)) &&
        (
            (abs(y - y0) <= lineWidth/2.0) ||
            (abs(y - y0 - h) <= lineWidth/2.0)
        )
    ) {
        return vec4(1.0, 1.0, 1.0, 1.0);
    }
    if ((y > y0 && y < (y0 + h)) &&
        (
            (abs(x - x0) <= lineWidth/2.0) ||
            (abs(x - x0 - w) <= lineWidth/2.0)
        )
    ) {
        return vec4(1.0, 1.0, 1.0, 1.0);
    }
    return pix;
}


vec3 argumentToColour(float argVal) {
    float maxCol = 1.0;
    float minCol = 50.0/255.0;
    float colRange = maxCol - minCol;
    if (argVal <= pi/3.0 && argVal >= 0.0) {
        return vec3(maxCol,
                    minCol + colRange*argVal/(pi/3.0), minCol);
    } else if (argVal > pi/3.0 && argVal <= 2.0*pi/3.0){
        return vec3(maxCol - colRange*(argVal - pi/3.0)/(pi/3.0),
                    maxCol, minCol);
    } else if (argVal > 2.0*pi/3.0 && argVal <= pi){
        return vec3(minCol, maxCol,
                    minCol + colRange*(argVal - 2.0*pi/3.0)/(pi/3.0));
    } else if (argVal < 0.0 && argVal > -pi/3.0){
        return vec3(maxCol, minCol,
                    minCol - colRange*argVal/(pi/3.0));
    } else if (argVal <= -pi/3.0 && argVal > -2.0*pi/3.0){
        return vec3(maxCol + (colRange*(argVal + pi/3.0)/(pi/3.0)),
                    minCol, maxCol);
    } else if (argVal <= -2.0*pi/3.0 && argVal >= -pi){
        return vec3(minCol,
                    minCol - (colRange*(argVal + 2.0*pi/3.0)/(pi/3.0)), maxCol);
    }
    else {
        return vec3(minCol, maxCol, maxCol);
    }
}


vec3 complexToColour(float re, float im) {
    return argumentToColour(atan(im, re));
}


void main () {

    vec4 col1 = texture2D(tex1, fragTexCoord);
    vec4 col2 = texture2D(tex2, fragTexCoord);
    vec4 col3 = texture2D(tex3, fragTexCoord);
    float probDensity = (col1.g*col3.g + col2.r*col2.r);
    float re = col2.r;
    float im = (col3.g + col1.g)/2.0;
    vec3 wavefunction;
    /*vec3 colPotential = col4.r*brightness2*
            argumentToColour(2.0*3.14159*col4.r*brightness2 - 1.0)*
            exp(-brightness*probDensity/16.0);*/

    vec4 col4 = texture2D(texV, fragTexCoord)/(50.0*1.0);

    vec3 potential;
    if (potentialDisplayMode == DISPLAY_POTENTIAL_SINGLE_COLOUR) {
        potential = col4.r*brightness2*potColour;
    } else if (potentialDisplayMode == DISPLAY_POTENTIAL_COLOUR_MAP) {
        float val = -3.0*pi*col4.r*brightness2 - 2.0*pi/3.0;
        if (val < -pi) {
            val = 2.0*pi + val;
            if (val < -pi/4.0) {
                val = -pi/4.0;
            }
        }
        potential = argumentToColour(val); 
        // min(col4.r*(brightness2), 1.25)*argumentToColour(val);
    } else if (potentialDisplayMode == DISPLAY_POTENTIAL_COLOUR) {
        
    }

    if (wavefunctionDisplayMode == DISPLAY_PHASE) {
        wavefunction = probDensity*(brightness/16.0)*complexToColour(re, im);
    } else if (wavefunctionDisplayMode == DISPLAY_ONLY_PROB_DENSITY) {
        wavefunction = probDensity*probColour*(brightness/16.0);
    } else if (wavefunctionDisplayMode == DISPLAY_PROB_DENSITY_HEIGHT_MAP) {
        float val = -pi*probDensity*brightness/(4.0*10.0) - 2.0*pi/3.0;
        if (val < -pi) {
            val = 2.0*pi + val;
            if (val < 0.0) {
                val = 0.0;
            }
        }
        wavefunction = min(probDensity*(brightness/16.0), 1.25)*
                           argumentToColour(val);
        // wavefunction = probDensity*(brightness/16.0)*
        //                vec3(probDensity, 5.0 - probDensity, 0.0);
    }

    vec3 background;
    if (backgroundDisplayMode == DISPLAY_BACKGROUND) {
        background = texture2D(backgroundTex, fragTexCoord).rgb;
    }

    vec4 pix = vec4(wavefunction + potential + background/4.0, 1.0);
    if (vectorDisplayMode == DISPLAY_VECTOR) {
        pix += 10.0*texture2D(vecTex, fragTexCoord);
    }

    fragColor = drawWindow(pix, fragTexCoord.x, fragTexCoord.y,
                              x0, y0, w, h, lineWidth) +
                              texture2D(textTex, fragTexCoord);
}
)";
const char *complex_multiply_shader_source = R"(

precision highp float;
#if __VERSION__ >= 300
#define texture2D texture
in vec2 fragTexCoord;
out vec4 fragColor;
#else
#define fragColor gl_FragColor
varying highp vec2 fragTexCoord;
#endif
uniform sampler2D tex1;
uniform sampler2D tex2;


void main() {
    vec4 col1 = texture2D(tex1, fragTexCoord);
    vec4 col2 = texture2D(tex2, fragTexCoord);
    fragColor = vec4(col1.r*col2.r - col1.g*col2.g, 
                     col1.r*col2.g + col1.g*col2.r, 0.0, col2.a);

})";
const char *jacobi_iteration_shader_source = R"(

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
uniform float dt;
uniform float w;
uniform float h;
uniform float m;
uniform float hbar;
uniform float rScaleV;
uniform sampler2D texPsi;
uniform sampler2D texPsiIter;
uniform sampler2D texV;
uniform sampler2D texA;
uniform int useAField;
uniform int laplacePoints;

vec2 mult(vec2 z1, vec2 z2) {
    return vec2(z1.x*z2.x - z1.y*z2.y, 
                z1.x*z2.y + z1.y*z2.x);
}

vec2 conj(vec2 z) {
    return vec2(z.r, -z.g);
}


float reValueAt(sampler2D texComplexFunc, vec2 location) {
    vec4 tmp = texture2D(texComplexFunc, location);
    return tmp.r*tmp.a;
}

float imagValueAt(sampler2D texComplexFunc, vec2 location) {
    vec4 tmp = texture2D(texComplexFunc, location);
    return tmp.g*tmp.a;
}

float getImagValuesAround(sampler2D texComplexFunc) {
    float u = imagValueAt(texComplexFunc, fragTexCoord + vec2(0.0, dy/h));
    float d = imagValueAt(texComplexFunc, fragTexCoord + vec2(0.0, -dy/h));
    float l = imagValueAt(texComplexFunc, fragTexCoord + vec2(-dx/w, 0.0));
    float r = imagValueAt(texComplexFunc, fragTexCoord + vec2(dx/w, 0.0));
    if (laplacePoints <= 5) {
        return u + d + l + r;
    } else {
        float ul = imagValueAt(texComplexFunc, 
                               fragTexCoord + vec2(-dx/w, dy/h));
        float ur = imagValueAt(texComplexFunc, 
                               fragTexCoord + vec2(dx/w, dy/h));
        float dl = imagValueAt(texComplexFunc, 
                               fragTexCoord + vec2(-dx/w, -dy/h));
        float dr = imagValueAt(texComplexFunc, 
                               fragTexCoord + vec2(dx/w, -dy/h));
        return 0.25*ur + 0.5*u + 0.25*ul + 0.5*l + 
                0.25*dl + 0.5*d + 0.25*dr + 0.5*r;

    }
}

float getReValuesAround(sampler2D texComplexFunc) {
    float u = reValueAt(texComplexFunc, fragTexCoord + vec2(0.0, dy/h));
    float d = reValueAt(texComplexFunc, fragTexCoord + vec2(0.0, -dy/h));
    float l = reValueAt(texComplexFunc, fragTexCoord + vec2(-dx/w, 0.0));
    float r = reValueAt(texComplexFunc, fragTexCoord + vec2(dx/w, 0.0));
    if (laplacePoints <= 5) {
        return u + d + l + r;
    } else {
        float ul = reValueAt(texComplexFunc, 
                             fragTexCoord + vec2(-dx/w, dy/h));
        float ur = reValueAt(texComplexFunc, 
                             fragTexCoord + vec2(dx/w, dy/h));
        float dl = reValueAt(texComplexFunc, 
                             fragTexCoord + vec2(-dx/w, -dy/h));
        float dr = reValueAt(texComplexFunc, 
                             fragTexCoord + vec2(dx/w, -dy/h));
        return 0.25*ur + 0.5*u + 0.25*ul + 0.5*l + 
                0.25*dl + 0.5*d + 0.25*dr + 0.5*r;
    }
}


vec2 valueAt(sampler2D texComplexFunc, vec2 location) {
    vec4 tmp = texture2D(texComplexFunc, location);
    return tmp.xy*tmp.a;
}

/* To approximage the vector potential, Peierls substitution is used where
very basically the non-diagonal elements are multiplied by a phase that is 
determined by a path from the diagonal to the non-diagonal element
using the vector potential. 

Feynman R., Leighton R., Sands M. (2011).
The Schrödinger Equation in a Classical Context: 
A Seminar on Superconductivity
https://www.feynmanlectures.caltech.edu/III_21.html.
In The Feynman Lectures on Physics: The New Millennium Edition, 
Volume 3, chapter 21. Basic Books.

Wikipedia contributors. (2021, April 21). Peierls substitution
https://en.wikipedia.org/wiki/Peierls_substitution. 
In Wikipedia, The Free Encyclopedia
*/

vec4 getAngles(vec2 location) {
    float q = 1.0;
    vec2 xy = location;
    vec4 c = texture2D(texA, xy);
    vec4 u = texture2D(texA, xy + vec2(0.0, dy/h));
    vec4 d = texture2D(texA, xy + vec2(0.0, -dy/h));
    vec4 l = texture2D(texA, xy + vec2(-dx/w, 0.0));
    vec4 r = texture2D(texA, xy + vec2(dx/w, 0.0));
    float thetaR = 0.5*q*(r + c).x*dx/hbar;
    float thetaU = 0.5*q*(u + c).y*dy/hbar;
    float thetaD = -0.5*q*(c + d).y*dy/hbar;
    float thetaL = -0.5*q*(c + l).x*dx/hbar;
    return vec4(thetaR, thetaU, thetaD, thetaL);
}

vec2 getPhase(float theta) {
    return vec2(cos(theta), -sin(theta));
}

vec2 getValuesAround(sampler2D texComplexFunc) {
    vec2 xy = fragTexCoord;
    vec4 theta = getAngles(xy);
    vec2 phaseR = getPhase(theta[0]);
    vec2 phaseU = getPhase(theta[1]);
    vec2 phaseD = getPhase(theta[2]);
    vec2 phaseL = getPhase(theta[3]);
    vec2 u = mult(valueAt(texComplexFunc, xy + vec2(0.0, dy/h)), phaseU);
    vec2 d = mult(valueAt(texComplexFunc, xy + vec2(0.0, -dy/h)), phaseD);
    vec2 l = mult(valueAt(texComplexFunc, xy + vec2(-dx/w, 0.0)), phaseL);
    vec2 r = mult(valueAt(texComplexFunc, xy + vec2(dx/w, 0.0)), phaseR);
    if (laplacePoints <= 5) {
        return u + d + l + r;
    } else {
        vec2 ul = valueAt(texComplexFunc, xy + vec2(-dx/w, dy/h));
        vec2 ur = valueAt(texComplexFunc, xy + vec2(dx/w, dy/h));
        vec2 dl = valueAt(texComplexFunc, xy + vec2(-dx/w, -dy/h));
        vec2 dr = valueAt(texComplexFunc, xy + vec2(dx/w, -dy/h));
        return 0.25*ur + 0.5*u + 0.25*ul + 0.5*l + 
                0.25*dl + 0.5*d + 0.25*dr + 0.5*r;
    }
}

void main() {
    float V = (1.0 - rScaleV)*texture2D(texV, fragTexCoord).r + 
                rScaleV*texture2D(texV, fragTexCoord).g;
    vec4 psiIter = texture2D(texPsiIter, fragTexCoord);
    vec4 psi = texture2D(texPsi, fragTexCoord);
    float c1 = (laplacePoints <= 5)? 1.0: 0.75;
    float imDiag = dt*V/(2.0*hbar) + c1*hbar*dt/(m*dx*dx);
    if (useAField == 0) {
        float reInvDiag = 1.0/(1.0 + imDiag*imDiag);
        float imInvDiag = -imDiag/(1.0 + imDiag*imDiag);
        float reTmp = psi.r;
        reTmp -= hbar*dt/(4.0*m*dx*dx)*getImagValuesAround(texPsiIter);
        float imTmp = psi.g;
        imTmp += hbar*dt/(4.0*m*dx*dx)*getReValuesAround(texPsiIter);
        fragColor = vec4(reInvDiag*reTmp - imInvDiag*imTmp,
                        imInvDiag*reTmp + reInvDiag*imTmp, 0.0, psi.a);
    } else {
        vec2 invDiag = vec2(1.0/(1.0 + imDiag*imDiag), 
                            -imDiag/(1.0 + imDiag*imDiag));
        vec2 I = vec2(0.0, 1.0);
        vec2 tmp = psi.xy + hbar*dt/(4.0*m*dx*dx)*
                   mult(I, getValuesAround(texPsiIter));
        fragColor = vec4(mult(invDiag, tmp), 0.0, psi.a);
    }
}
)";
const char *exp_potential_shader_source = R"(

precision highp float;
#if __VERSION__ >= 300
#define texture2D texture
in vec2 fragTexCoord;
out vec4 fragColor;
#else
#define fragColor gl_FragColor
varying highp vec2 fragTexCoord;
#endif
uniform sampler2D texV;
uniform float dt;
uniform float hbar;

void main() {
    vec4 potential = texture2D(texV, fragTexCoord);
    float reV = potential[0];
    float imV = potential[2];
    // Arg = -i*0.5*(reV + i*imV)*dt/hbar = 0.5*(-i*reV + imV)*dt/hbar
    float imArg = -0.5*reV*dt/hbar;
    float reArg = 0.5*imV*dt/hbar;
    fragColor = vec4(exp(reArg)*cos(imArg), exp(reArg)*sin(imArg), 0.0, 1.0);
}
)";
const char *cn_explicit_part_shader_source = R"(

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
uniform float dt;
uniform float w;
uniform float h;
uniform float m;
uniform float hbar;
uniform float rScaleV;
uniform sampler2D texPsi;
uniform sampler2D texV;
uniform sampler2D texA;
uniform int useAField;
uniform int laplacePoints;


vec2 mult(vec2 z1, vec2 z2) {
    return vec2(z1.x*z2.x - z1.y*z2.y, 
                z1.x*z2.y + z1.y*z2.x);
}

vec2 conj(vec2 z) {
    return vec2(z.r, -z.g);
}

float realValueAt(vec2 location) {
    vec4 tmp = texture2D(texPsi, location);
    return tmp.r*tmp.a;
}

float imagValueAt(vec2 location) {
    vec4 tmp = texture2D(texPsi, location);
    return tmp.g*tmp.a;
}

vec2 valueAt(vec2 location) {
    vec4 tmp = texture2D(texPsi, location);
    return tmp.xy*tmp.a;
}

/* To approximage the vector potential, Peierls substitution is used where
very basically the non-diagonal elements are multiplied by a phase that is 
determined by a path from the diagonal to the non-diagonal element
using the vector potential. 

Feynman R., Leighton R., Sands M. (2011).
The Schrödinger Equation in a Classical Context: 
A Seminar on Superconductivity
https://www.feynmanlectures.caltech.edu/III_21.html.
In The Feynman Lectures on Physics: The New Millennium Edition, 
Volume 3, chapter 21. Basic Books.

Wikipedia contributors. (2021, April 21). Peierls substitution
https://en.wikipedia.org/wiki/Peierls_substitution. 
In Wikipedia, The Free Encyclopedia
*/

vec4 getAngles(vec2 location) {
    float q = 1.0;
    vec2 xy = location;
    vec4 c = texture2D(texA, xy);
    vec4 u = texture2D(texA, xy + vec2(0.0, dy/h));
    vec4 d = texture2D(texA, xy + vec2(0.0, -dy/h));
    vec4 l = texture2D(texA, xy + vec2(-dx/w, 0.0));
    vec4 r = texture2D(texA, xy + vec2(dx/w, 0.0));
    float thetaR = 0.5*q*(r + c).x*dx/hbar;
    float thetaU = 0.5*q*(u + c).y*dy/hbar;
    float thetaD = -0.5*q*(c + d).y*dy/hbar;
    float thetaL = -0.5*q*(c + l).x*dx/hbar;
    return vec4(thetaR, thetaU, thetaD, thetaL);
}

vec2 getPhase(float theta) {
    return vec2(cos(theta), -sin(theta));
}

vec2 getDiv2Psi() {
    vec2 xy = fragTexCoord;
    vec4 theta = getAngles(xy);
    vec2 phaseR = getPhase(theta[0]);
    vec2 phaseU = getPhase(theta[1]);
    vec2 phaseD = getPhase(theta[2]);
    vec2 phaseL = getPhase(theta[3]);
    vec2 u = mult(valueAt(xy + vec2(0.0, dy/h)), phaseU);
    vec2 d = mult(valueAt(xy + vec2(0.0, -dy/h)), phaseD);
    vec2 l = mult(valueAt(xy + vec2(-dx/w, 0.0)), phaseL);
    vec2 r = mult(valueAt(xy + vec2(dx/w, 0.0)), phaseR);
    vec2 c = valueAt(xy);
    if (laplacePoints <= 5) {
        return (u + d + l + r - 4.0*c)/(dx*dx);
    } else {
        vec2 ul = valueAt(xy + vec2(-dx/w, dy/h));
        vec2 ur = valueAt(xy + vec2(dx/w, dy/h));
        vec2 dl = valueAt(xy + vec2(-dx/w, -dy/h));
        vec2 dr = valueAt(xy + vec2(dx/w, -dy/h));
        return (0.25*ur + 0.5*u + 0.25*ul + 0.5*l + 
                0.25*dl + 0.5*d + 0.25*dr + 0.5*r - 3.0*c)/(dx*dx);
    }

}

float getDiv2RePsi(float rePsi) {
    float u = realValueAt(fragTexCoord + vec2(0.0, dy/h));
    float d = realValueAt(fragTexCoord + vec2(0.0, -dy/h));
    float l = realValueAt(fragTexCoord + vec2(-dx/w, 0.0));
    float r = realValueAt(fragTexCoord + vec2(dx/w, 0.0));
    if (laplacePoints <= 5) {
        return (u + d + l + r - 4.0*rePsi)/(dx*dx);
    } else {
        float ul = realValueAt(fragTexCoord + vec2(-dx/w, dy/h));
        float ur = realValueAt(fragTexCoord + vec2(dx/w, dy/h));
        float dl = realValueAt(fragTexCoord + vec2(-dx/w, -dy/h));
        float dr = realValueAt(fragTexCoord + vec2(dx/w, -dy/h));
        return (0.25*ur + 0.5*u + 0.25*ul + 0.5*l + 
                0.25*dl + 0.5*d + 0.25*dr + 0.5*r - 3.0*rePsi)/(dx*dx);
    }
}

float getDiv2ImPsi(float imPsi) {
    float u = imagValueAt(fragTexCoord + vec2(0.0, dy/h));
    float d = imagValueAt(fragTexCoord + vec2(0.0, -dy/h));
    float l = imagValueAt(fragTexCoord + vec2(-dx/w, 0.0));
    float r = imagValueAt(fragTexCoord + vec2(dx/w, 0.0));
    if (laplacePoints <= 5) {
        return (u + d + l + r - 4.0*imPsi)/(dx*dx);
    } else {
        float ul = imagValueAt(fragTexCoord + vec2(-dx/w, dy/h));
        float ur = imagValueAt(fragTexCoord + vec2(dx/w, dy/h));
        float dl = imagValueAt(fragTexCoord + vec2(-dx/w, -dy/h));
        float dr = imagValueAt(fragTexCoord + vec2(dx/w, -dy/h));
        return (0.25*ur + 0.5*u + 0.25*ul + 0.5*l + 
                0.25*dl + 0.5*d + 0.25*dr + 0.5*r - 3.0*imPsi)/(dx*dx);
    }
}

void main() {
    float V = (1.0 - rScaleV)*texture2D(texV, fragTexCoord).r + 
               rScaleV*texture2D(texV, fragTexCoord).g;
    vec4 psi = texture2D(texPsi, fragTexCoord);
    // TODO: do an electromagnetic field where
    // H = e**2*A**2/(2*m) - e*A*p/(2*m) - e*p*(A/(2*m)) + p**2/(2*m) + V
    // H = p**2/(2*m) - e*A*p/(2*m) - e*p*(A/(2*m)) + (e**2*A**2/(2*m) + V)
    // V_A = (e**2*A**2/(2*m) + V)
    // H = (1/(2*m))*p**2 - (e/(2*m))*A*p - (e/(2*m))*p*A + V_A
    // H psi = (1/(2*m))*p**2 psi - (e/(2*m))*A*p psi
    //          - (e/(2*m))*p (A psi) + V_A psi
    if (useAField == 0) {
        float reKinetic = (-hbar*hbar/(2.0*m))*getDiv2RePsi(psi.r);
        float imKinetic = (-hbar*hbar/(2.0*m))*getDiv2ImPsi(psi.g);
        float hamiltonRePsi = reKinetic + V*psi.r;
        float hamiltonImPsi = imKinetic + V*psi.g;
        // 1 - i*dt*H/(2.0*hbar)
        fragColor = vec4(psi.r + dt/(2.0*hbar)*hamiltonImPsi,
                         psi.g - dt/(2.0*hbar)*hamiltonRePsi, 0.0, psi.a
                         );
    } else {
        vec2 kinetic = (-hbar*hbar/(2.0*m))*getDiv2Psi();
        vec2 hamiltonPsi = kinetic + V*psi.xy;
        vec2 I = vec2(0.0, 1.0);
        fragColor = vec4(psi.xy - (dt/(2.0*hbar))*mult(I, hamiltonPsi), 0.0,
                         psi.a);
    }
}
)";
#endif