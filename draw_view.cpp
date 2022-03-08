#include "draw_view.hpp"


void draw_view(Quad &view_frame,  ViewFrameReads &read_frames,
               GLuint view_program) {
    GLuint frame1 = Quad::get_blank();
    if (read_frames.frame1 != nullptr)
        frame1 = read_frames.frame1->get_value();
    GLuint frame2 = Quad::get_blank();
    if (read_frames.frame2 != nullptr)
        frame2 = read_frames.frame1->get_value();
    GLuint frame3 = Quad::get_blank();
    if (read_frames.frame3 != nullptr)
        frame3 = read_frames.frame1->get_value();
    GLuint frame_potential = Quad::get_blank();
    if (read_frames.frame_potential != nullptr)
        frame_potential = read_frames.frame_potential->get_value();
    GLuint frame_vec = Quad::get_blank();
    if (read_frames.frame_vec!= nullptr)
        frame_vec = read_frames.frame_vec->get_value();
    GLuint frame_text = Quad::get_blank();
    if (read_frames.frame_text != nullptr)
        frame_text = read_frames.frame_text->get_value();
    GLuint frame_background = Quad::get_blank();
    if (read_frames.frame_background != nullptr)
        frame_background = read_frames.frame_background->get_value();
    view_frame.set_program(view_program);
    view_frame.bind();
    view_frame.set_float_uniform("brightness", 5.0);
    view_frame.set_float_uniform("brightness2", 1.0);
    view_frame.set_int_uniform("tex1", frame1);
    view_frame.set_int_uniform("tex2", frame2);
    view_frame.set_int_uniform("tex3", frame3);
    view_frame.set_int_uniform("texV", frame_potential);
    view_frame.set_int_uniform("vecTex", frame_vec);
    view_frame.set_int_uniform("textTex", frame_text);
    view_frame.set_int_uniform("backgroundTex", frame_background);
    view_frame.set_int_uniform("displayMode", 1);
    view_frame.set_int_uniform("wavefunctionDisplayMode", 1);
    view_frame.set_vec3_uniform("probColour", 1.0, 1.0, 1.0);
    view_frame.set_vec3_uniform("potColour", 1.0, 1.0, 1.0);
    // view_frame.set_int_uniform("potentialDisplayMode", 0);
    view_frame.draw();
    unbind();
}