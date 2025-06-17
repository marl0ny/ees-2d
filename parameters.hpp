#include "gl_wrappers.hpp"

namespace sim_2d {

#ifndef _PARAMETERS_
#define _PARAMETERS_

struct Button {};

typedef std::string Label;

typedef bool BoolRecord;

typedef std::vector<std::string> EntryBoxes;

struct SelectionList {
    int selected;
    std::vector<std::string> options;
};

struct SimParams {

struct LineDivider {};

struct NotUsed {};
    int stepsPerFrame = (int)(1);
    float brightness = (float)(0.15F);
    float t = (float)(0.0F);
    float dt = (float)(0.01F);
    bool invertTimeStep = (bool)(false);
    Button normalizeWaveFunction = Button{};
    bool colorPhase = (bool)(true);
    bool show3D = (bool)(false);
    Vec2 heightScales = (Vec2)(Vec2 {.ind={1.0, 0.0}});
    float brightness2 = (float)(0.01F);
    float hbar = (float)(1.0F);
    float width = (float)(10.0F);
    float height = (float)(10.0F);
    LineDivider lineDivider1 = LineDivider{};
    Label simResetLabel = Label{};
    Label simResetLabelSubtext = Label{};
    int numberOfStates = (int)(16);
    SelectionList laplacianStencil = SelectionList{1, {"2nd order 5 pt.", "4th order 9 pt."}};
    int gridWidth = (int)(128);
    int gridHeight = (int)(128);
    float m = (float)(1.0F);
    Button simResetButton = Button{};
    LineDivider lineDivider2 = LineDivider{};
    SelectionList presetPotential = SelectionList{0, {"x^2 + y^2", "5*(x^2 + y^2)/2", "sqrt(x^2 + y^2)", "x^4 + y^4", "(x+3/2)^2*(x-3/2)^2", "5-5*exp(-(x^2+y^2)/9)", "Finite circular well", "Heart"}};
    EntryBoxes scalarPotential = EntryBoxes{"0"};
    Button enterScalarPotential = Button{};
    Label xRangeLabel = Label{};
    Label yRangeLabel = Label{};
    enum {
        STEPS_PER_FRAME=0,
        BRIGHTNESS=1,
        T=2,
        DT=3,
        INVERT_TIME_STEP=4,
        NORMALIZE_WAVE_FUNCTION=5,
        COLOR_PHASE=6,
        SHOW3_D=7,
        HEIGHT_SCALES=8,
        BRIGHTNESS2=9,
        HBAR=10,
        WIDTH=11,
        HEIGHT=12,
        LINE_DIVIDER1=13,
        SIM_RESET_LABEL=14,
        SIM_RESET_LABEL_SUBTEXT=15,
        NUMBER_OF_STATES=16,
        LAPLACIAN_STENCIL=17,
        GRID_WIDTH=18,
        GRID_HEIGHT=19,
        M=20,
        SIM_RESET_BUTTON=21,
        LINE_DIVIDER2=22,
        PRESET_POTENTIAL=23,
        SCALAR_POTENTIAL=24,
        ENTER_SCALAR_POTENTIAL=25,
        X_RANGE_LABEL=26,
        Y_RANGE_LABEL=27,
    };
    void set(int enum_val, Uniform val) {
        switch(enum_val) {
            case STEPS_PER_FRAME:
            stepsPerFrame = val.i32;
            break;
            case BRIGHTNESS:
            brightness = val.f32;
            break;
            case T:
            t = val.f32;
            break;
            case DT:
            dt = val.f32;
            break;
            case INVERT_TIME_STEP:
            invertTimeStep = val.b32;
            break;
            case COLOR_PHASE:
            colorPhase = val.b32;
            break;
            case SHOW3_D:
            show3D = val.b32;
            break;
            case HEIGHT_SCALES:
            heightScales = val.vec2;
            break;
            case BRIGHTNESS2:
            brightness2 = val.f32;
            break;
            case HBAR:
            hbar = val.f32;
            break;
            case WIDTH:
            width = val.f32;
            break;
            case HEIGHT:
            height = val.f32;
            break;
            case NUMBER_OF_STATES:
            numberOfStates = val.i32;
            break;
            case GRID_WIDTH:
            gridWidth = val.i32;
            break;
            case GRID_HEIGHT:
            gridHeight = val.i32;
            break;
            case M:
            m = val.f32;
            break;
        }
    }
    Uniform get(int enum_val) const {
        switch(enum_val) {
            case STEPS_PER_FRAME:
            return {(int)stepsPerFrame};
            case BRIGHTNESS:
            return {(float)brightness};
            case T:
            return {(float)t};
            case DT:
            return {(float)dt};
            case INVERT_TIME_STEP:
            return {(bool)invertTimeStep};
            case COLOR_PHASE:
            return {(bool)colorPhase};
            case SHOW3_D:
            return {(bool)show3D};
            case HEIGHT_SCALES:
            return {(Vec2)heightScales};
            case BRIGHTNESS2:
            return {(float)brightness2};
            case HBAR:
            return {(float)hbar};
            case WIDTH:
            return {(float)width};
            case HEIGHT:
            return {(float)height};
            case NUMBER_OF_STATES:
            return {(int)numberOfStates};
            case GRID_WIDTH:
            return {(int)gridWidth};
            case GRID_HEIGHT:
            return {(int)gridHeight};
            case M:
            return {(float)m};
        }
        return Uniform(0);
    }
    void set(int enum_val, int index, std::string val) {
        switch(enum_val) {
            case SIM_RESET_LABEL:
            simResetLabel = val;
            break;
            case SIM_RESET_LABEL_SUBTEXT:
            simResetLabelSubtext = val;
            break;
            case SCALAR_POTENTIAL:
            scalarPotential[index] = val;
            break;
            case X_RANGE_LABEL:
            xRangeLabel = val;
            break;
            case Y_RANGE_LABEL:
            yRangeLabel = val;
            break;
        }
    }
};
#endif
}
