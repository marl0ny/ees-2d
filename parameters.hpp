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
    bool show3D = (bool)(false);
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
        SHOW3_D=6,
        HBAR=7,
        WIDTH=8,
        HEIGHT=9,
        LINE_DIVIDER1=10,
        SIM_RESET_LABEL=11,
        SIM_RESET_LABEL_SUBTEXT=12,
        NUMBER_OF_STATES=13,
        LAPLACIAN_STENCIL=14,
        GRID_WIDTH=15,
        GRID_HEIGHT=16,
        M=17,
        SIM_RESET_BUTTON=18,
        LINE_DIVIDER2=19,
        SCALAR_POTENTIAL=20,
        ENTER_SCALAR_POTENTIAL=21,
        X_RANGE_LABEL=22,
        Y_RANGE_LABEL=23,
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
            case SHOW3_D:
            show3D = val.b32;
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
            case SHOW3_D:
            return {(bool)show3D};
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
